#include "NyaFloatingUI.hpp"
#include "NyaConfig.hpp"
#include "main.hpp"
#include "EndpointConfigUtils.hpp"

#include "Utils/Utils.hpp"
#include "ImageView.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "bsml/shared/BSML-Lite/Creation/Image.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "Utils/FileUtils.hpp"
#include "assets.hpp"
#include "libs/magic_enum.hpp"

using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Nya, NyaFloatingUI);

namespace Nya {
    // Function gets url for the current selected category

    void NyaFloatingUI::ctor()
    {
        screenhandle = nullptr;
        UIScreen = nullptr;
        UINoGlow = nullptr;
        hoverClickHelper = nullptr;
        imageView = nullptr;
        INFO("Created NyaFloatingUI instance");
    }

    void NyaFloatingUI::initScreen(){
        if (this->isInitialized){
            return;
        }

        auto FloatingScreen =  BSML::Lite::CreateFloatingScreen(
            {40.0f, 32.0f},
            {0.0f, 1.0f, 1.0f},
            {0, 0, 0},
            0.0f,
            true,
            false);
        UIScreen = FloatingScreen->get_gameObject();
        UIScreen->set_active(false);
        UIScreen->GetComponent<UnityEngine::Canvas*>()->set_sortingOrder(31);
        UnityEngine::GameObject::DontDestroyOnLoad(UIScreen);

        // Handle creation
        screenhandle = UIScreen->GetComponent<BSML::FloatingScreen*>()->handle;
//        UIScreen->GetComponent<BSML::FloatingScreen*>()->bgGo->GetComponentInChildren<BSML::Backgroundable*>()->ApplyBackgroundWithAlpha("round-rect-panel", 0.0f);
        screenhandle->get_transform()->set_localPosition(UnityEngine::Vector3(0.0f, -23.0f, 0.0f));
        screenhandle->get_transform()->set_localScale(UnityEngine::Vector3(5.3f, 3.3f, 5.3f));



        auto* vert = BSML::Lite::CreateVerticalLayoutGroup(UIScreen->get_transform());

        vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        NYA = BSML::Lite::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
        NYA->set_preserveAspect(true);
        // Set blank sprite to avoid white screens
        NYA->set_sprite(BSML::Lite::ArrayToSprite(Assets::placeholder_png));
        auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
        imageView = NYA->get_gameObject()->AddComponent<NyaUtils::ImageView*>();
        ele->set_preferredHeight(50);
        ele->set_preferredWidth(50);

        auto horz = BSML::Lite::CreateHorizontalLayoutGroup(vert->get_transform());
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);


        // Get new picture
        this->nyaButton = BSML::Lite::CreateUIButton(horz->get_transform(), "Nya", "PlayButton",
        [this]() {
            this->imageView->GetImage(nullptr);
        });

        this->settingsMenu = NYA->get_gameObject()->AddComponent<Nya::SettingsMenu*>();

        // Settings button
        this->settingsButton = BSML::Lite::CreateUIButton(horz->get_transform(), to_utf16("Settings"), "PracticeButton",
        [this]() {
            this->settingsMenu->Show();
        });

        UINoGlow = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>()->First(
        [](UnityEngine::Material* x) {
                return x->get_name() == "UINoGlow";
            });
        auto* screenthingidk = FloatingScreen->get_gameObject()->AddComponent<HMUI::Screen*>();
        auto* normalpointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
        hoverClickHelper = Nya::addHoverClickHelper(normalpointer, screenhandle, FloatingScreen);

        this->isInitialized = true;

        this->UpdateScale();
        this->UpdateHandleVisibility();

        // Sub to events
        this->imageView->imageLoadingChange += {&NyaFloatingUI::OnIsLoadingChange, this};
    }
    
    void NyaFloatingUI::SetDefaultPos () {
        // Do nothing if hover click helper is not present
        
        // If screen does not exist, initialize the first time to reset stuff
        if (!this->UIScreen || !this->UIScreen->m_CachedPtr) {
            DEBUG("LOL IT ACTUALLY HAPPENS");
            // this->isInitialized = false;
            this->initScreen();
        }

        if ( !this->hoverClickHelper || !this->hoverClickHelper->m_CachedPtr) {
            DEBUG("SetDefaultPos canceled");
            return;
        }

        if (this->currentScene == Nya::FloatingUIScene::Pause) {
            this->hoverClickHelper->SetPosition(
                getNyaConfig().pausePosition.GetDefaultValue(),
                UnityEngine::Quaternion::Euler(
                    getNyaConfig().pauseRotation.GetDefaultValue()
                )
            );

            // Save target 
            this->updateCoordinates(
                getNyaConfig().pausePosition.GetDefaultValue(),
                getNyaConfig().pauseRotation.GetDefaultValue()
            );                
        }

        if (this->currentScene == Nya::FloatingUIScene::MainMenu) {
            this->hoverClickHelper->SetPosition(
                getNyaConfig().menuPosition.GetDefaultValue(),
                UnityEngine::Quaternion::Euler(
                    getNyaConfig().menuRotation.GetDefaultValue()
                )
            );

            // Save coordinates
            this->updateCoordinates(
                getNyaConfig().menuPosition.GetDefaultValue(),
                getNyaConfig().menuRotation.GetDefaultValue()
            );
        }
    }

    void NyaFloatingUI::onSceneChange(Nya::FloatingUIScene scene, bool reinitialize) {
        INFO("Switched from {} to {} ", magic_enum::enum_name(this->currentScene), magic_enum::enum_name(scene));
        
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
                if (
                    this->UIScreen &&
                    this->UIScreen->m_CachedPtr
                ) UIScreen->set_active(false);
                if (
                    this->hoverClickHelper &&
                    this->hoverClickHelper->m_CachedPtr
                ) this->hoverClickHelper->set_enabled(false);
                return;
        };
        
        // If screen does not exist, initialize the first time
        if (!this->UIScreen || !this->UIScreen->m_CachedPtr) {
            this->initScreen();
            // If the screen is created, get the first image
            this->imageView->GetImage(nullptr);
        }

        if (scene == Nya::FloatingUIScene::Pause) {
            INFO("Showing pause");
            this->hoverClickHelper->SetPosition(
                getNyaConfig().pausePosition.GetValue(),
                Quaternion::Euler(
                    getNyaConfig().pauseRotation.GetValue()
                ),
                false
            );

            this->hoverClickHelper->UpdatePointer();
        }

        if (scene == Nya::FloatingUIScene::MainMenu) {
            INFO("Showing main menu");
            this->hoverClickHelper->SetPosition(
                getNyaConfig().menuPosition.GetValue(),
                Quaternion::Euler(
                    getNyaConfig().menuRotation.GetValue()
                ),
                false
            );
          
            this->hoverClickHelper->UpdatePointer();
        }

        INFO("SETTING SCREEN TO ACTIVE");
        // Set UIScreen active and reset click helper state
        UIScreen->set_active(true);
        if (this->hoverClickHelper && this->hoverClickHelper->m_CachedPtr) {
            this->hoverClickHelper->set_enabled(true);
            INFO("RESETTING CLICK HELPER");
            hoverClickHelper->resetBools();
            INFO("Reset click helper");
        }
    }

    // Saves the coordinates to a config
    void NyaFloatingUI::updateCoordinates(UnityEngine::Transform* transform){
        auto position = transform->get_position();
        auto rotation = transform->get_rotation().get_eulerAngles();

        this->updateCoordinates(position, rotation);
    }

    void NyaFloatingUI::updateCoordinates(UnityEngine::Vector3 position, UnityEngine::Vector3 eulerRotation) {

        // INFO("Position: %.02f, %.02f, %.02f", position.x, position.y, position.z);
        // INFO("Rotation: %.02f, %.02f, %.02f", rotation.x, rotation.y, rotation.z);
        if (this->currentScene == Nya::FloatingUIScene::Pause){
            INFO("Saved to Pause");
            getNyaConfig().pausePosition.SetValue(position);
            getNyaConfig().pauseRotation.SetValue(eulerRotation);
        }
        if (this->currentScene == Nya::FloatingUIScene::MainMenu){
            INFO("Saved to MainMenu");
            getNyaConfig().menuPosition.SetValue(position);
            getNyaConfig().menuRotation.SetValue(eulerRotation);
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
        instance = go->AddComponent<NyaFloatingUI*>();
        return instance;
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

    void NyaFloatingUI::OnIsLoadingChange (bool isLoading) {
        BSML::MainThreadScheduler::Schedule([this, isLoading]
        {
            if (this->nyaButton && this->nyaButton->m_CachedPtr)
                this->nyaButton->set_interactable(!isLoading);
        });
    }

    void NyaFloatingUI::OnActiveSceneChanged(UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
        std::string prevSceneName(prevScene.get_name());
        std::string nextSceneName(nextScene.get_name());
        INFO("scene changed from {} to {}", prevSceneName, nextSceneName);
        
   
        if (nextSceneName.find("Menu") != std::string::npos ) {
             BSML::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::MainMenu);
            });
            
        } else if (nextSceneName.find("Pause") != std::string::npos ) {
            BSML::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::Pause);
            });
        } else {
            BSML::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::Disabled);
            });
        }
    };

    void NyaFloatingUI::ScaleFloatingScreen(float scale) {
        if (this->UIScreen && this->UIScreen->m_CachedPtr) {
            this->UIScreen->get_gameObject()->get_transform()->set_localScale(
                UnityEngine::Vector3(
                    0.03f * scale,
                    0.03f * scale,
                    0.03f * scale
                )
            );
        }
    }

    void NyaFloatingUI::UpdateScale() {
        float scale = getNyaConfig().FloatingScreenScale.GetValue();
        this->ScaleFloatingScreen(scale);
    }

    void NyaFloatingUI::UpdateHandleVisibility(){
        bool visibility = getNyaConfig().ShowHandle.GetValue();
        if (this->screenhandle && this->screenhandle->m_CachedPtr) {
            this->screenhandle->set_active(visibility);
        }
    }
}