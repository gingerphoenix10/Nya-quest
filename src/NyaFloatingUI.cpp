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

        this->settingsMenu = NYA->get_gameObject()->AddComponent<Nya::SettingsMenu*>();

        // Settings button
        this->settingsButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), to_utf16("Settings"), "PracticeButton",
        [this]() {
            this->settingsMenu->Show();
        });

        UINoGlow = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>(), [](UnityEngine::Material* x) { return x->get_name() == "UINoGlow"; });
        auto* screenthingidk = thing->get_gameObject()->AddComponent<HMUI::Screen*>();
        auto* normalpointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
        hoverClickHelper = Nya::addHoverClickHelper(normalpointer, screenhandle, thing);
        this->isInitialized = true;
    }
    
    void NyaFloatingUI::SetDefaultPos () {
        if (this->currentScene == Nya::FloatingUIScene::Pause || this->currentScene == Nya::FloatingUIScene::InGame ) {
            this->hoverClickHelper->SetPosition(
                UnityEngine::Vector3(
                    getNyaConfig().pausePositionX.GetDefaultValue(), 
                    getNyaConfig().pausePositionY.GetDefaultValue(),
                    getNyaConfig().pausePositionZ.GetDefaultValue()
                ),
                UnityEngine::Quaternion::Euler(
                    getNyaConfig().pauseRotationX.GetDefaultValue(), 
                    getNyaConfig().pauseRotationY.GetDefaultValue(), 
                    getNyaConfig().pauseRotationZ.GetDefaultValue()
                )
            );
        }

        if (this->currentScene == Nya::FloatingUIScene::Results) {
            this->hoverClickHelper->SetPosition(
                UnityEngine::Vector3(
                    getNyaConfig().resultPositionX.GetDefaultValue(), 
                    getNyaConfig().resultPositionY.GetDefaultValue(),
                    getNyaConfig().resultPositionZ.GetDefaultValue()
                ),
                UnityEngine::Quaternion::Euler(
                    getNyaConfig().resultRotationX.GetDefaultValue(), 
                    getNyaConfig().resultRotationY.GetDefaultValue(), 
                    getNyaConfig().resultRotationZ.GetDefaultValue()
                )
            );
        }
        if (this->currentScene == Nya::FloatingUIScene::MainMenu) {
            this->hoverClickHelper->SetPosition(
                UnityEngine::Vector3(
                    getNyaConfig().menuPositionX.GetDefaultValue(), 
                    getNyaConfig().menuPositionY.GetDefaultValue(),
                    getNyaConfig().menuPositionZ.GetDefaultValue()
                ),
                UnityEngine::Quaternion::Euler(
                    getNyaConfig().menuRotationX.GetDefaultValue(), 
                    getNyaConfig().menuRotationY.GetDefaultValue(), 
                    getNyaConfig().menuRotationZ.GetDefaultValue()
                )
            );
        }

        this->updateCoordinates(this->hoverClickHelper->handleTransform->get_transform());
    }

    void NyaFloatingUI::onSceneChange(Nya::FloatingUIScene scene, bool reinitialize) {
        INFO("Switched from {} to {} ", this->currentScene, scene);
        
        // Do nothing if the scene did not change
        if (!reinitialize && this->currentScene == scene) {
            return;
        }

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
            INFO("Saved to MainMenu");
            getNyaConfig().menuPositionX.SetValue(position.x);
            getNyaConfig().menuPositionY.SetValue(position.y);
            getNyaConfig().menuPositionZ.SetValue(position.z);
            
            getNyaConfig().menuRotationX.SetValue(rotation.x);
            getNyaConfig().menuRotationY.SetValue(rotation.y);
            getNyaConfig().menuRotationZ.SetValue(rotation.z);
        }
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