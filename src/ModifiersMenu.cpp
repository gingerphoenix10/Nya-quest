#define RAPIDJSON_HAS_STDSTRING 1
#include "ModifiersMenu.hpp"
#include "Utils/FileUtils.hpp"
#include "API.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "System/IO/File.hpp"
#include "Utils/Utils.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "HMUI/ModalView.hpp"
#include "System/Action.hpp"
#include "ImageView.hpp"

#include <dirent.h>
#include <string>
#include <iostream>

using namespace UnityEngine;

DEFINE_TYPE(Nya, ModifiersMenu);



// Disable
void Nya::ModifiersMenu::OnDisable() {
    // getLogger().debug("Disabled");
}

// Enable (runs when the component appears)
void Nya::ModifiersMenu::OnEnable() {
    // getLogger().debug("Enabled");
}

void Nya::ModifiersMenu::ctor() {
    getLogger().debug("Creator runs");

        // std::experimental::filesystem

    // APIS: waifu.pics
    this->api_list =  Nya::Utils::vectorToList({ "waifu.pics", "local"  });

    this->sfw_endpoints = Nya::Utils::vectorToList({ 
        "waifu", "neko", "shinobu", "megumin", "bully", "cuddle", "cry", "hug", "awoo", "kiss", "lick", "pat", "smug", "bonk", "yeet", "blush", "smile", "wave", "highfive", "handhold", "nom", "bite", "glomp", "slap", "kill", "kick", "happy", "wink", "poke", "dance", "cringe" 
    });
    this->nsfw_endpoints = Nya::Utils::vectorToList({
        "waifu",
        "neko",
        "trap",
        "blowjob"
    });

    auto vert = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_transform());
    vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

    NYA = QuestUI::BeatSaberUI::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
    NYA->set_preserveAspect(true);
    auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
    getLogger().debug("Adds component");
    auto view = NYA->get_gameObject()->AddComponent<NyaUtils::ImageView*>();
    ele->set_preferredHeight(50);
    ele->set_preferredWidth(50);

    auto horz = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());
    horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
    horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
    horz->set_spacing(10);

    this->nyaButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), "Nya", "PlayButton",
        [this, view]() {
            this->nyaButton->set_interactable(false);
            auto view = NYA->get_gameObject()->GetComponent<NyaUtils::ImageView*>();
            if (getNyaConfig().API.GetValue() == "local") {
                auto fileList = FileUtils::getAllFilesInFolder(NyaGlobals::imagesPath);
                int randomIndex = Utils::random(0, fileList.size()-1);
                getLogger().debug("Index is %i", randomIndex);
                getLogger().debug("File is %s", fileList[randomIndex].c_str());
                view->LoadFile(fileList[randomIndex], [this](bool success) {
                    this->nyaButton->set_interactable(true);
                });

            } else {
                NyaAPI::get_path_from_api(NyaAPI::get_api_path(), 10.0f, [this, view](bool success, std::string url) {
                    if (success) {
                        GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(
                            view->DownloadImage(url, 10.0f, [this](bool success, long code) {
                                this->nyaButton->set_interactable(true);
                            })
                        ));
                    } else {
                        this->nyaButton->set_interactable(true);
                    }
                });
            };
        });

    // Settings button
    this->settingsButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), to_utf16("Settings"), "PracticeButton",
    [this]() {
        getLogger().debug("Settings button clicked");
        // Run UI on the main thread
        QuestUI::MainThreadScheduler::Schedule([this]
           {
               try {

                   this->settingsModal->Show(true, true, nullptr);

                   std::string API = getNyaConfig().API.GetValue();
                   std::string SFWEndpoint = getNyaConfig().SFWEndpoint.GetValue();
                   getLogger().info("Selected sfw category: %s", SFWEndpoint.data());
                   getLogger().info("Selected api: %s", API.data());

                   // Restore api endpoint state
                   if (this->api_switch != nullptr) {
                   
                       if (this->api_list == nullptr) {
                      
                       } else {
                           this->api_switch->SetTexts(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<StringW>*>(this->api_list));
                           int index = Nya::Utils::findStrIndexInList(this->api_list,API);
                           this->api_switch->SelectCellWithIdx(index);
                       }

                   } else {
                       getLogger().info("api_switch is null");
                   }

                   // SFW endpoints

                   this->sfw_endpoint->SetTexts(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<StringW>*>(this->sfw_endpoints));
                   this->sfw_endpoint->SelectCellWithIdx(Nya::Utils::findStrIndexInList(this->sfw_endpoints,SFWEndpoint));
#ifdef NSFW
                   // Restore nsfw state
                   this->nsfw_endpoint->SetTexts(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<StringW>*>(this->nsfw_endpoints));
                   this->nsfw_endpoint->SelectCellWithIdx(Nya::Utils::findStrIndexInList(this->nsfw_endpoints, getNyaConfig().NSFWEndpoint.GetValue()));
                   this->nsfw_toggle->set_isOn(getNyaConfig().NSFWEnabled.GetValue());
#endif

               } catch (Il2CppException& e) {
                   getLogger().debug("Error caught in Floating settings %s", to_utf8(csstrtostr(e.message)).c_str());
               }

           });
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
        this->api_switch = QuestUI::BeatSaberUI::CreateDropdown(vert->get_transform(), to_utf16("API"),  "Loading..", {"Loading.."} , [](StringW value){
            getNyaConfig().API.SetValue(value);
        });


        // SFW endpoint switch
        std::string SFWEndpoint = getNyaConfig().SFWEndpoint.GetValue();
        this->sfw_endpoint = QuestUI::BeatSaberUI::CreateDropdown(vert->get_transform(), to_utf16("SFW endpoint"),  "Loading..", {"Loading.."}, [](StringW value){
            getNyaConfig().SFWEndpoint.SetValue(value);
        });

#ifdef NSFW
        // NSFW endpoint selector
        std::string NSFWEndpoint = getNyaConfig().NSFWEndpoint.GetValue();
        this->nsfw_endpoint = QuestUI::BeatSaberUI::CreateDropdown(vert->get_transform(), to_utf16("NSFW endpoint"), "Loading..", {"Loading.."}, [](StringW value){
            getNyaConfig().NSFWEndpoint.SetValue(value);
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

void Nya::ModifiersMenu::DidActivate(bool firstActivation)
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

void Nya::ModifiersMenu::dtor(){
  il2cpp_utils::getLogger().debug("Destruct");
}