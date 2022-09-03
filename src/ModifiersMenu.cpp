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
        getLogger().fmtLog<Paper::LogLevel::DBG>("Creator runs");

        auto vert = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_transform());
        vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        NYA = QuestUI::BeatSaberUI::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
        NYA->set_preserveAspect(true);
        auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
        getLogger().fmtLog<Paper::LogLevel::DBG>("Adds component");
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

        {
            this->settingsModal =  QuestUI::BeatSaberUI::CreateModal(get_transform(),  { 65, 65 }, nullptr);

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
                                                                                        });
        }
        
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
        getLogger().fmtLog<Paper::LogLevel::INF>("Modifiers menu destroyed");
    }
}

