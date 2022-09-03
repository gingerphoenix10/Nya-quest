#include "NyaFloatingUI.hpp"
#include "NyaConfig.hpp"
#include "main.hpp"
#include "EndpointConfig.hpp"

#include "Utils/Utils.hpp"
#include "ImageView.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "Utils/FileUtils.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"

using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Nya, NyaFloatingUI);

namespace Nya {
    // Function gets url for the current selected category

    void NyaFloatingUI::ctor()
    {
        instance = this;
        screenhandle = nullptr;
        UIScreen = nullptr;
        UINoGlow = nullptr;
        hoverClickHelper = nullptr;
        INFO("Created NyaFloatingUI instance");
    }

    void NyaFloatingUI::initScreen(){
        if (this->isInitialized){
            return;
        }

        UIScreen = QuestUI::BeatSaberUI::CreateFloatingScreen({40.0f, 32.0f}, {0.0f, 1.0f, 1.0f}, {0, 0, 0}, 0.0f, true, true, 0);
        UIScreen->set_active(false);
        UIScreen->GetComponent<UnityEngine::Canvas*>()->set_sortingOrder(31);
        UnityEngine::GameObject::DontDestroyOnLoad(UIScreen);
        screenhandle = UIScreen->GetComponent<QuestUI::FloatingScreen*>()->handle;
        UIScreen->GetComponent<QuestUI::FloatingScreen*>()->bgGo->GetComponentInChildren<QuestUI::Backgroundable*>()->ApplyBackgroundWithAlpha("round-rect-panel", 0.0f);
        screenhandle->get_transform()->set_localPosition(UnityEngine::Vector3(0.0f, -23.0f, 0.0f));
        screenhandle->get_transform()->set_localScale(UnityEngine::Vector3(5.3f, 3.3f, 5.3f));
        QuestUI::FloatingScreen* thing = UIScreen->GetComponent<QuestUI::FloatingScreen*>();

        auto* vert = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(UIScreen->get_transform());
    
        vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        NYA = QuestUI::BeatSaberUI::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
        NYA->set_preserveAspect(true);
        auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
        auto view = NYA->get_gameObject()->AddComponent<NyaUtils::ImageView*>();
        ele->set_preferredHeight(50);
        ele->set_preferredWidth(50);

        auto horz = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);


        // Get new picture
        this->nyaButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), "Nya", "PlayButton",
        [this, view]() {
            Nya::Utils::onNyaClick(this->nyaButton, view);
        });

        // Settings button
        this->settingsButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), to_utf16("Settings"), "PracticeButton",
        [this]() {
            Nya::Utils::onSettingsClick(
                this->settingsModal,
                this->api_switch,
                this->sfw_endpoint,
                this->nsfw_endpoint,
                this->nsfw_toggle
            );
        });

        // TODO: Move modal to its own component
        {
            this->settingsModal =  QuestUI::BeatSaberUI::CreateModal(thing->get_transform(),  { 65, 65 }, nullptr);
            // Create a text that says "Hello World!" and set the parent to the container.
            UnityEngine::UI::VerticalLayoutGroup* vert = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(this->settingsModal->get_transform());
            vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            vert->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(60.0);


            TMPro::TextMeshProUGUI* title = QuestUI::BeatSaberUI::CreateText(vert->get_transform(), "Settings");
            title->GetComponent<TMPro::TMP_Text*>()->set_alignment(TMPro::TextAlignmentOptions::Center);
            title->GetComponent<TMPro::TMP_Text*>()->set_fontSize(7.0);

            // API Selection (nothing to select for now)
            std::string API = getNyaConfig().API.GetValue();
            this->api_switch = QuestUI::BeatSaberUI::CreateDropdown(vert->get_transform(), to_utf16("API"),  "Loading..", {"Loading.."} , [this](StringW value){
                getNyaConfig().API.SetValue(value);

                auto source = NyaAPI::get_data_source(value);

                auto sfwList = Nya::Utils::listStringToStringW(source->SfwEndpoints);

                // SFW endpoints
                this->sfw_endpoint->SetTexts(sfwList->i_IReadOnlyList_1_T());

                std::string endpoint_sfw = EndpointConfig::getEndpointValue(getNyaConfig().config, value, false);
                int sfw_index = -1;
                if (endpoint_sfw != "") {
                    sfw_index = Nya::Utils::findStrIndexInListC( source->SfwEndpoints ,endpoint_sfw);
                }

                if (sfw_index >= 0) {
                    this->sfw_endpoint->SelectCellWithIdx(sfw_index); 
                }
                

                #ifdef NSFW
                   // Restore nsfw state
                   if (source->NsfwEndpoints.size() == 0) {
                        this->nsfw_endpoint->button->set_interactable(false);
                        // Reset the view
                        this->nsfw_endpoint->SetTexts(List<StringW>::New_ctor()->i_IReadOnlyList_1_T());
                        this->nsfw_endpoint->SelectCellWithIdx(0); 
                   } else {
                        this->nsfw_endpoint->button->set_interactable(true);
                        auto nsfwList = Nya::Utils::listStringToStringW(source->NsfwEndpoints);

                        this->nsfw_endpoint->SetTexts(nsfwList->i_IReadOnlyList_1_T());

                        std::string endpoint_nsfw = EndpointConfig::getEndpointValue(getNyaConfig().config, value, true);
                        int nsfw_index = -1;
                        if (endpoint_nsfw != "") {
                            nsfw_index = Nya::Utils::findStrIndexInListC(source->NsfwEndpoints, endpoint_nsfw);
                        }
                        
                        if (nsfw_index >= 0) {
                            this->nsfw_endpoint->SelectCellWithIdx(nsfw_index); 
                        }
                   }

                   this->nsfw_toggle->set_isOn(getNyaConfig().NSFWEnabled.GetValue());
                
                #endif
            });


            // SFW endpoint switch
            std::string SFWEndpoint = getNyaConfig().SFWEndpoint.GetValue();
            this->sfw_endpoint = QuestUI::BeatSaberUI::CreateDropdown(vert->get_transform(), to_utf16("SFW endpoint"),  "Loading..", {"Loading.."}, [](StringW value){
                std::string API = getNyaConfig().API.GetValue();
                EndpointConfig::updateEndpointValue(getNyaConfig().config, API, false, value);
            });

            #ifdef NSFW
                // NSFW endpoint selector
                std::string NSFWEndpoint = getNyaConfig().NSFWEndpoint.GetValue();
                this->nsfw_endpoint = QuestUI::BeatSaberUI::CreateDropdown(vert->get_transform(), to_utf16("NSFW endpoint"), "Loading..", {"Loading.."}, [](StringW value){
                    // Get current endpoint
                    std::string API = getNyaConfig().API.GetValue();
                    EndpointConfig::updateEndpointValue(getNyaConfig().config, API, true, value);
                });

                // NSFW toggle
                bool NSFWEnabled = getNyaConfig().NSFWEnabled.GetValue();
                this->nsfw_toggle = QuestUI::BeatSaberUI::CreateToggle(vert->get_transform(),  to_utf16("NSFW toggle"), NSFWEnabled,  [](bool isChecked){
                    getNyaConfig().NSFWEnabled.SetValue(isChecked);
                });
            #endif

            UnityEngine::UI::HorizontalLayoutGroup* horz = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());
            horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            horz->set_spacing(10);


            UnityEngine::UI::Button* closeButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), to_utf16("Close"), "PracticeButton",
                [this]() {
                    this->settingsModal->Hide(true, nullptr);
                }
            );
        }
        UINoGlow = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>(), [](UnityEngine::Material* x) { return x->get_name() == "UINoGlow"; });
        auto* screenthingidk = thing->get_gameObject()->AddComponent<HMUI::Screen*>();
        auto* normalpointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
        hoverClickHelper = Nya::addHoverClickHelper(normalpointer, screenhandle, thing);
        this->isInitialized = true;
    }
  
    void NyaFloatingUI::onSceneChange(Nya::FloatingUIScene scene) {
        // Do nothing if the scene did not change
        INFO("Switched from {} to {} ", this->currentScene, scene);

        this->currentScene = scene;
        if (scene == Nya::FloatingUIScene::Pause) {
            if (this->UIScreen != nullptr) {
                if (!getNyaConfig().inPause.GetValue()) {
                    UIScreen->set_active(false);
                    return;
                };
                this->initScreen();
            
                UIScreen->get_transform()->set_position(
                    UnityEngine::Vector3(
                        getNyaConfig().pausePositionX.GetValue(), 
                        getNyaConfig().pausePositionY.GetValue(),
                        getNyaConfig().pausePositionZ.GetValue()
                    )
                );
                UIScreen->get_transform()->set_rotation(
                    UnityEngine::Quaternion::Euler(
                        getNyaConfig().pauseRotationX.GetValue(), 
                        getNyaConfig().pauseRotationY.GetValue(), 
                        getNyaConfig().pauseRotationZ.GetValue()
                    )
                );
                UIScreen->set_active(true);
                hoverClickHelper->resetBools();
                auto* pausepointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(1);
                // Mover to move the ui component
                auto* mover = pausepointer->get_gameObject()->AddComponent<QuestUI::FloatingScreenMoverPointer*>();
                mover->Init(UIScreen->GetComponent<QuestUI::FloatingScreen*>(), pausepointer);
            }
        }

        if (scene == Nya::FloatingUIScene::Results) {
            if (!getNyaConfig().inResults.GetValue()) {
                UIScreen->set_active(false);
                return;
            };
            this->initScreen();

            auto* pointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
            hoverClickHelper->vrPointer = pointer;

            hoverClickHelper->resetBools();
            UIScreen->get_transform()->set_position(
                UnityEngine::Vector3(
                    getNyaConfig().resultPositionX.GetValue(), 
                    getNyaConfig().resultPositionY.GetValue(),
                    getNyaConfig().resultPositionZ.GetValue()
                )
            );
            UIScreen->get_transform()->set_rotation(
                UnityEngine::Quaternion::Euler(
                    getNyaConfig().resultRotationX.GetValue(), 
                    getNyaConfig().resultRotationY.GetValue(), 
                    getNyaConfig().resultRotationZ.GetValue()
                )
            );
            UIScreen->set_active(true);

        }
        if (scene == Nya::FloatingUIScene::MainMenu) {
            if (!getNyaConfig().inMenu.GetValue()) {
                UIScreen->set_active(false);
                return;
            };

            this->initScreen();

            auto* pointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
            hoverClickHelper->vrPointer = pointer;
            hoverClickHelper->resetBools();

            UIScreen->get_transform()->set_position(
                UnityEngine::Vector3(
                    getNyaConfig().menuPositionX.GetValue(), 
                    getNyaConfig().menuPositionY.GetValue(),
                    getNyaConfig().menuPositionZ.GetValue()
                )
            );
            UIScreen->get_transform()->set_rotation(
                UnityEngine::Quaternion::Euler(
                    getNyaConfig().menuRotationX.GetValue(), 
                    getNyaConfig().menuRotationY.GetValue(), 
                    getNyaConfig().menuRotationZ.GetValue()
                )
            );
            
            UIScreen->set_active(true);
            hoverClickHelper->resetBools();
        }

        if (scene == Nya::FloatingUIScene::InGame) {
            if (this->UIScreen != nullptr) {
                if (!getNyaConfig().inGame.GetValue()) {
                    UIScreen->set_active(false);
                    return;
                };

                this->initScreen();

                UIScreen->get_transform()->set_position(
                    UnityEngine::Vector3(
                        getNyaConfig().pausePositionX.GetValue(), 
                        getNyaConfig().pausePositionY.GetValue(),
                        getNyaConfig().pausePositionZ.GetValue()
                    )
                );
                UIScreen->get_transform()->set_rotation(
                    UnityEngine::Quaternion::Euler(
                        getNyaConfig().pauseRotationX.GetValue(), 
                        getNyaConfig().pauseRotationY.GetValue(), 
                        getNyaConfig().pauseRotationZ.GetValue()
                    )
                );
                UIScreen->set_active(true);
            }
        }
    }

    // Saves the coordinates to a config
    void NyaFloatingUI::updateCoordinates(UnityEngine::Transform* transform){
        auto position = transform->get_position();
        auto rotation = transform->get_rotation().get_eulerAngles();

        // INFO("Position: %.02f, %.02f, %.02f", position.x, position.y, position.z);
        // INFO("Rotation: %.02f, %.02f, %.02f", rotation.x, rotation.y, rotation.z);
        if (this->currentScene == Nya::FloatingUIScene::Pause){
            INFO("Saved to Pause");
            getNyaConfig().pausePositionX.SetValue(position.x);
            getNyaConfig().pausePositionY.SetValue(position.y);
            getNyaConfig().pausePositionZ.SetValue(position.z);
        
            getNyaConfig().pauseRotationX.SetValue(rotation.x);
            getNyaConfig().pauseRotationY.SetValue(rotation.y);
            getNyaConfig().pauseRotationZ.SetValue(rotation.z);
        }
        if (this->currentScene == Nya::FloatingUIScene::Results){
            INFO("Saved to Results");
            getNyaConfig().resultPositionX.SetValue(position.x);
            getNyaConfig().resultPositionY.SetValue(position.y);
            getNyaConfig().resultPositionZ.SetValue(position.z);
            
            
            getNyaConfig().resultRotationX.SetValue(rotation.x);
            getNyaConfig().resultRotationY.SetValue(rotation.y);
            getNyaConfig().resultRotationZ.SetValue(rotation.z);
        }
        if (this->currentScene == Nya::FloatingUIScene::MainMenu){
            // INFO("Saved to MainMenu");
            getNyaConfig().menuPositionX.SetValue(position.x);
            getNyaConfig().menuPositionY.SetValue(position.y);
            getNyaConfig().menuPositionZ.SetValue(position.z);
            
            getNyaConfig().menuRotationX.SetValue(rotation.x);
            getNyaConfig().menuRotationY.SetValue(rotation.y);
            getNyaConfig().menuRotationZ.SetValue(rotation.z);
        }
        // if (this->currentScene == Nya::FloatingUIScene::InGame){
        //     getNyaConfig().gamePosition.SetValue(transform->get_position());
        //     getNyaConfig().gameRotation.SetValue(transform->get_rotation().get_eulerAngles());
        // }
    }

    NyaFloatingUI* NyaFloatingUI::instance = nullptr;
    NyaFloatingUI* NyaFloatingUI::get_instance()
    {
        if (instance)
            return instance;
        auto go = GameObject::New_ctor(StringW(___TypeRegistration::get()->name()));
        Object::DontDestroyOnLoad(go);
        return go->AddComponent<NyaFloatingUI*>();
    }

//    Check if nya is enabled anywhere
    bool NyaFloatingUI::isEnabled (){
        return (
                getNyaConfig().inGame.GetValue() ||
                getNyaConfig().inMenu.GetValue() ||
                getNyaConfig().inPause.GetValue() ||
                getNyaConfig().inResults.GetValue()
                );
    }

    void NyaFloatingUI::OnActiveSceneChanged(UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
        std::string prevSceneName(prevScene.get_name());
        std::string nextSceneName(nextScene.get_name());
        INFO("scene changed from {} to {}", prevSceneName, nextSceneName);
        
   
        if (nextSceneName.find("Menu")) {
             QuestUI::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::MainMenu);
           
            });
             return;
        }
        if (nextSceneName.find("Pause")) {
             QuestUI::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::Pause);
           
            });
            return;
        }
};
}