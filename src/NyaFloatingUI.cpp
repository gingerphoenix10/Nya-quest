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
        pauseMover = nullptr;
        menuMover = nullptr;
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

        // Handle creation
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
        // Do nothing if hover click helper is not present
        if (this->hoverClickHelper == nullptr) {
            DEBUG("SetDefaultPos canceled");
            return;
        } 
        if (this->currentScene == Nya::FloatingUIScene::Pause) {
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
        
        // Do nothing if the scene did not change unless reinitialize is active
        if (!reinitialize && this->currentScene == scene) {
            return;
        }
        
        this->currentScene = scene;

        // Disable everything if config does not need it and return
        if (
            (
                scene == Nya::FloatingUIScene::Disabled
            ) || (
                scene == Nya::FloatingUIScene::Pause && !getNyaConfig().inPause.GetValue()
            ) || (
                scene == Nya::FloatingUIScene::MainMenu && !getNyaConfig().inMenu.GetValue()
            )) {
                INFO("DISABLING THE SCREEN");
                if (this->UIScreen != nullptr) UIScreen->set_active(false);
                if (this->hoverClickHelper != nullptr) this->hoverClickHelper->set_enabled(false);
                return;
        };
        
        // If screen does not exist, initialize the first time
        if (this->UIScreen == nullptr) {
            this->initScreen();
        }

        if (scene == Nya::FloatingUIScene::Pause) {
            INFO("Showing pause");
            this->hoverClickHelper->SetPosition(
                UnityEngine::Vector3(
                    getNyaConfig().pausePositionX.GetValue(), 
                    getNyaConfig().pausePositionY.GetValue(),
                    getNyaConfig().pausePositionZ.GetValue()
                ),
                Quaternion::Euler(
                    getNyaConfig().pauseRotationX.GetValue(), 
                    getNyaConfig().pauseRotationY.GetValue(), 
                    getNyaConfig().pauseRotationZ.GetValue()
                ),
                false
            );

            auto* pausepointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(1);
            if (pausepointer == nullptr) {
                DEBUG("Pointer is null in the transition, setting vrPointer to null");
            }
            this->hoverClickHelper->vrPointer = pausepointer;

            if (this->pauseMover != nullptr ) {
                UnityEngine::Object::Destroy(this->pauseMover);
            }
            // Mover to move the ui component
            this->pauseMover = pausepointer->get_gameObject()->AddComponent<QuestUI::FloatingScreenMoverPointer*>();
            this->pauseMover->Init(UIScreen->GetComponent<QuestUI::FloatingScreen*>(), pausepointer);
        }

        if (scene == Nya::FloatingUIScene::MainMenu) {
            INFO("Showing main menu");
            this->hoverClickHelper->SetPosition(
                UnityEngine::Vector3(
                    getNyaConfig().menuPositionX.GetValue(), 
                    getNyaConfig().menuPositionY.GetValue(),
                    getNyaConfig().menuPositionZ.GetValue()
                ),
                Quaternion::Euler(
                    getNyaConfig().menuRotationX.GetValue(), 
                    getNyaConfig().menuRotationY.GetValue(), 
                    getNyaConfig().menuRotationZ.GetValue()
                ),
                false
            );
            static bool menuMoverInitialized = false;

            // Check if the pointer is null here
            auto* pointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
            if (pointer == nullptr) {
                DEBUG("Pointer is null in the transition, setting vrPointer to null");
            }
            this->hoverClickHelper->vrPointer = pointer;

         
            // Get the last mover
            if (this->menuMover != nullptr ) {
                UnityEngine::Object::Destroy(this->menuMover);
            }
            // Mover to move the ui component
            this->menuMover = pointer->get_gameObject()->AddComponent<QuestUI::FloatingScreenMoverPointer*>();
            this->menuMover->Init(UIScreen->GetComponent<QuestUI::FloatingScreen*>(), pointer);
           
        }

        INFO("SETTING SCREEN TO ACTIVE");
        // Set UIScreen active and reset click helper state
        UIScreen->set_active(true);
        if (this->hoverClickHelper != nullptr) this->hoverClickHelper->set_enabled(true);
        INFO("RESETTING CLICK HELPER");
        hoverClickHelper->resetBools();
        INFO("Reset click helper");
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

    // singleton?
    NyaFloatingUI* NyaFloatingUI::get_instance()
    {
        if (instance)
            return instance;
        auto go = GameObject::New_ctor(StringW(___TypeRegistration::get()->name()));
        Object::DontDestroyOnLoad(go);
        return go->AddComponent<NyaFloatingUI*>();
    }

    void NyaFloatingUI::delete_instance()
    {
        if (instance){
            Object::DestroyImmediate(instance->get_gameObject());
            instance = nullptr;
        }
    }

//    Check if nya is enabled anywhere
    bool NyaFloatingUI::isEnabled (){
        return (
            getNyaConfig().inMenu.GetValue() ||
            getNyaConfig().inPause.GetValue() 
        );
    }

    void NyaFloatingUI::OnActiveSceneChanged(UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
        std::string prevSceneName(prevScene.get_name());
        std::string nextSceneName(nextScene.get_name());
        INFO("scene changed from {} to {}", prevSceneName, nextSceneName);
        
   
        if (nextSceneName.find("Menu") != std::string::npos ) {
             QuestUI::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::MainMenu);
            });
            
        } else if (nextSceneName.find("Pause") != std::string::npos ) {
            QuestUI::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::Pause);
            });
        } else {
            QuestUI::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::Disabled);
            });
        }
    };
}