#define RAPIDJSON_HAS_STDSTRING 1
#include "ModifiersMenu.hpp"
#include "Utils/FileUtils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "System/IO/File.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "HMUI/ModalView.hpp"
#include "System/Action.hpp"
#include "ImageView.hpp"
#include "EndpointConfig.hpp"
#include <dirent.h>
#include <string>
#include <iostream>

using namespace UnityEngine;
using namespace Nya;

DEFINE_TYPE(Nya, ModifiersMenu);

namespace Nya {
    // Disable
    void ModifiersMenu::OnDisable() {
        // getLogger().debug("Disabled");
    }

    // Enable (runs when the component appears)
    void ModifiersMenu::OnEnable() {
        // getLogger().debug("Enabled");
    }

    void ModifiersMenu::ctor() {
        DEBUG("Creator runs");

        auto vert = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_transform());
        vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        NYA = QuestUI::BeatSaberUI::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
        NYA->set_preserveAspect(true);
        auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
        DEBUG("Adds component");
        auto view = NYA->get_gameObject()->AddComponent<NyaUtils::ImageView*>();
        ele->set_preferredHeight(50);
        ele->set_preferredWidth(50);

        auto horz = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);

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

        
        
    }

    void ModifiersMenu::DidActivate(bool firstActivation)
    {
        if(firstActivation)
        {
    //        auto view = NYA->get_gameObject()->GetComponent<NyaUtils::ImageView*>();
    //
    //        NyaAPI::get_path_from_api(NyaAPI::get_api_path(), 10.0f, [this, view](bool success, std::string url) {
    //            if (success) {
    //                view->DownloadImage(url, 10.0f, [this](bool success, long code) {
    //                    this->nyaButton->set_interactable(true);
    //                });
    //            } else {
    //                this->nyaButton->set_interactable(true);
    //            }
    //        });
        }
    }

    void ModifiersMenu::dtor(){
        INFO("Modifiers menu destroyed");
    }
}

