#include "SettingsMenu.hpp"
#include "Utils/Utils.hpp"

DEFINE_TYPE(Nya, SettingsMenu);


namespace Nya {
    // Constructor
    void SettingsMenu::ctor()
    {
        this->settingsModal = QuestUI::BeatSaberUI::CreateModal(get_transform(),  { 65, 65 }, nullptr);
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

    bool SettingsMenu::isShown() {
        return this->settingsModal->isShown;
    }

    void SettingsMenu::Show() {
        INFO("Settings button clicked");
        
        // Run UI on the main thread
        QuestUI::MainThreadScheduler::Schedule([this]
        {
            std::string API = getNyaConfig().API.GetValue();
            std::string SFWEndpoint = getNyaConfig().SFWEndpoint.GetValue();
            SourceData* source = nullptr;

            // Catch for invalid apis
            try
            {
                source = NyaAPI::get_data_source(API);
            }
            catch(...)
            {
                API = "waifu.pics";
                getNyaConfig().API.SetValue(API);
                source = NyaAPI::get_data_source(API);
            }

            auto sources = Nya::Utils::vectorToList(NyaAPI::get_source_list());
            auto sfwList = Nya::Utils::listStringToStringW(source->SfwEndpoints);

            this->api_switch->SetTexts(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<StringW>*>(sources));
        
            int index = Nya::Utils::findStrIndexInList(sources, API);
            if (index != -1) {
                this->api_switch->SelectCellWithIdx(index);
            }
            
    
            // SFW endpoints
            this->sfw_endpoint->SetTexts(sfwList->i_IReadOnlyList_1_T());
            std::string endpoint_sfw = EndpointConfig::getEndpointValue(getNyaConfig().config, API, false);
            int sfw_index = -1;
            if (endpoint_sfw != "") {
                sfw_index = Nya::Utils::findStrIndexInListC( source->SfwEndpoints ,endpoint_sfw);
            }

            if (sfw_index != -1) {
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

                    std::string endpoint_nsfw = EndpointConfig::getEndpointValue(getNyaConfig().config, API, true);
                    int nsfw_index = -1;
                    if (endpoint_nsfw != "") {
                        nsfw_index = Nya::Utils::findStrIndexInListC(source->NsfwEndpoints, endpoint_nsfw);
                    }
                    
                    if (nsfw_index != -1) {
                        nsfw_endpoint->SelectCellWithIdx(nsfw_index); 
                    }
                }

                this->nsfw_toggle->set_isOn(getNyaConfig().NSFWEnabled.GetValue());
            #endif
            
            settingsModal->Show(true, true, nullptr);
        });
    }

    void SettingsMenu::Hide() {
        this->settingsModal->Hide(true, nullptr);
    }

}