#include "Utils/Utils.hpp"
#include <random>

#include "System/StringComparison.hpp"
#include "System/Uri.hpp"

namespace Nya::Utils {
    /**
     * @brief Converts vector to list
     * 
     * @param values 
     * @return List<StringW>* 
     */
    List<StringW>* vectorToList(std::vector<StringW> values) {
        List<StringW>* list = List<StringW>::New_ctor();
        for (int i = 0; i < values.size(); i++){
            auto value = values[i];
            list->Add(value);
        }
        return list;
    }

    ListWrapper<StringW> listStringToStringW(std::list<std::string> values) {
        // TODO: Fix
        int count = values.size();
        
        // Convert stuff to list
        ListWrapper<StringW> list(List<StringW>::New_ctor());
        if (count == 0) {
            return list;
        }
        list->EnsureCapacity(count);
        for (auto item : values) {list->Add(item);};

        return list;
    }

    /**
     * @brief Converts listW to vector
     * 
     * @param values 
     * @return List<StringW> 
     */
    std::vector<StringW> listWToVector(List<StringW>* values) {
        std::vector<StringW> vector = std::vector<StringW>(values->items);
        return vector;
    }

    /**
     * @brief Finds the string in a list
     * 
     * @param values 
     * @param string 
     * @return int -1 if not found anything or index of the element if the item is found
     */
    int findStrIndexInList(List<StringW>* values, StringW string ) {
        for (int i = 0; i < values->size; i++){
            auto value = values->get_Item(i);
            if (value == string) {
                return i;
            }
        }
        return -1;
    }

    /**
     * @brief Finds the string in a list
     * 
     * @param values 
     * @param string 
     * @return int -1 if not found anything or index of the element if the item is found
     */
    int findStrIndexInListC(std::list<std::string> values, StringW string ) {
        
        // Create iterator pointing to first element
        std::list<std::string>::iterator it = values.begin();

        for (int i = 0; i < values.size(); i++){
            auto value = *it;
            if (value == string) {
                return i;
            }
            std::advance(it, 1);
        }
        return -1;
    }

    // Generate random number 
    // Stolen from https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
    int random(int min, int max) //range : [min, max]
    {
        static bool first = true;
        if (first) 
        {  
            srand( time(NULL) ); //seeding for the first time only!
            first = false;
        }
        return min + rand() % (( max + 1 ) - min);
    }

    // Download new picture
    void onNyaClick(UnityEngine::UI::Button* button, NyaUtils::ImageView* imageView) {
        // Disable the button
        button->set_interactable(false);
        try
        {   
            // Get value
            std::string currentAPI = getNyaConfig().API.GetValue();
            // TODO: Make dynamic
            SourceData* source =  NyaAPI::get_data_source(currentAPI);

            // Local files
            if (source->Mode == DataMode::Local) {
                auto fileList = FileUtils::getAllFilesInFolder(NyaGlobals::imagesPath);
                int randomIndex = Utils::random(0, fileList.size()-1);

                imageView->LoadFile(fileList[randomIndex], [button](bool success) {
                    button->set_interactable(true);
                });
            } else 
            if (source->Mode == DataMode::Json) {
                bool NSFWEnabled = false;
                
                #ifdef NSFW
                    NSFWEnabled = getNyaConfig().NSFWEnabled.GetValue();
                #endif
                
                // Construct the url
                // TODO: check if endpoint from the setting exists and make it dynamic

                std::string endpointValue = EndpointConfig::getEndpointValue(getNyaConfig().config, currentAPI, NSFWEnabled);

                // If we found no nsfw, show sfw
                if (endpointValue == "" && NSFWEnabled) {
                    endpointValue = EndpointConfig::getEndpointValue(getNyaConfig().config, currentAPI, false);
                }

                std::string endpointURL = "";
                if (NSFWEnabled) {
                    endpointURL = source->BaseEndpoint + endpointValue;  
                } else {
                    endpointURL = source->BaseEndpoint + endpointValue;  
                }
                

                getLogger().fmtLog<Paper::LogLevel::INF>("Endpoint URL: {}", endpointURL.c_str());
                NyaAPI::get_path_from_api(endpointURL, 10.0f, [button, imageView](bool success, std::string url) {
                    getLogger().fmtLog<Paper::LogLevel::DBG>("Image URL: {}", url);
                    if (success) {
                        
                        QuestUI::MainThreadScheduler::Schedule([button, imageView, url]{
                            // Use coroutine to get download image
                            GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(imageView->DownloadImage(url, 10.0f, [button](bool success, long code) {
                                button->set_interactable(true);
                            })));
                        });
                    } else {
                        // Error getting things
                        getLogger().fmtLog<Paper::LogLevel::ERR>("Failed to load image from api");
                        getLogger().Backtrace(20);
                        button->set_interactable(true);
                    }
                });
            }

            
        
        }
        // TODO: If the source is not set up, set up the default
        catch(const std::exception& e)
        {
            getLogger().fmtLog<Paper::LogLevel::ERR>("Custom fail");
            getLogger().Backtrace(20);
          
        }  
    }

    // Settings ui control
    void onSettingsClick(
        HMUI::ModalView* settingsModal,
        HMUI::SimpleTextDropdown* api_switch,
        HMUI::SimpleTextDropdown* sfw_endpoint,
        HMUI::SimpleTextDropdown* nsfw_endpoint,
        UnityEngine::UI::Toggle* nsfw_toggle
    ) { 

        getLogger().fmtLog<Paper::LogLevel::INF>("Settings button clicked");
            // Run UI on the main thread
            QuestUI::MainThreadScheduler::Schedule([
                    settingsModal,
                    api_switch,
                    sfw_endpoint,
                    nsfw_endpoint,
                    nsfw_toggle
                ]
            {
            
                settingsModal->Show(true, true, nullptr);

                std::string API = getNyaConfig().API.GetValue();
                std::string SFWEndpoint = getNyaConfig().SFWEndpoint.GetValue();

                // Get current api or set the default one
                SourceData* source =  NyaAPI::get_data_source(API);

                auto sources = Nya::Utils::vectorToList(NyaAPI::get_source_list());
                auto sfwList = Nya::Utils::listStringToStringW(source->SfwEndpoints);

                api_switch->SetTexts(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<StringW>*>(sources));
           
                int index = Nya::Utils::findStrIndexInList(sources, API);
                if (index != -1) {
                    api_switch->SelectCellWithIdx(index);
                }
                
        
                // SFW endpoints
                sfw_endpoint->SetTexts(sfwList->i_IReadOnlyList_1_T());
                std::string endpoint_sfw = EndpointConfig::getEndpointValue(getNyaConfig().config, API, false);
                int sfw_index = -1;
                if (endpoint_sfw != "") {
                    sfw_index = Nya::Utils::findStrIndexInListC( source->SfwEndpoints ,endpoint_sfw);
                }

                if (sfw_index != -1) {
                    sfw_endpoint->SelectCellWithIdx(sfw_index); 
                }


                #ifdef NSFW
                   // Restore nsfw state
                   if (source->NsfwEndpoints.size() == 0) {
                        nsfw_endpoint->button->set_interactable(false);
                        // Reset the view
                        nsfw_endpoint->SetTexts(List<StringW>::New_ctor()->i_IReadOnlyList_1_T());
                        nsfw_endpoint->SelectCellWithIdx(0); 
                   } else {
                        nsfw_endpoint->button->set_interactable(true);
                        auto nsfwList = Nya::Utils::listStringToStringW(source->NsfwEndpoints);

                        nsfw_endpoint->SetTexts(nsfwList->i_IReadOnlyList_1_T());

                        std::string endpoint_nsfw = EndpointConfig::getEndpointValue(getNyaConfig().config, API, true);
                        int nsfw_index = -1;
                        if (endpoint_nsfw != "") {
                            nsfw_index = Nya::Utils::findStrIndexInListC(source->NsfwEndpoints, endpoint_nsfw);
                        }
                        
                        if (nsfw_index != -1) {
                            nsfw_endpoint->SelectCellWithIdx(nsfw_index); 
                        }
                   }

                   nsfw_toggle->set_isOn(getNyaConfig().NSFWEnabled.GetValue());
                #endif
            });
    }

    // Checks if the path is animated
    bool IsAnimated(StringW str)
    {
        return  str->EndsWith(".gif", System::StringComparison::OrdinalIgnoreCase) || 
                str->EndsWith("_gif", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".apng", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_apng", System::StringComparison::OrdinalIgnoreCase);
    }
    bool IsImage(StringW str) {
     
        return  str->EndsWith(".gif", System::StringComparison::OrdinalIgnoreCase) || 
                str->EndsWith("_gif", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".apng", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_apng", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith(".png", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith("_png", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".jpeg", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_jpeg", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".webp", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_webp", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith(".tiff", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_tiff", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".bmp", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_bmp", System::StringComparison::OrdinalIgnoreCase);
  
    }

}

