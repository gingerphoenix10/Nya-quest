#include "NyaFloatingUI.hpp"
#include "NyaConfig.hpp"
#include "main.hpp"
#include "Utils/WebUtils.hpp"
#include "Utils/Gif.hpp"
#include "Utils/Utils.hpp"
#include "ImageView.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "API.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"

using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Nya, NyaFloatingUI);

namespace Nya {
    // Function gets url for the current selected category

    void NyaFloatingUI::ctor()
    {
//        DEBUG("Created NyaFloatingUI instance: {}", fmt::ptr(this));

        getLogger().debug("Created NyaFloatingUI instance");
        instance = this;
        screenhandle = nullptr;
        UIScreen = nullptr;
        UINoGlow = nullptr;
        hoverClickHelper = nullptr;

        // Init screen
        initScreen();
    }

    void NyaFloatingUI::initScreen(){
        // APIS: waifu.pics
        this->api_list =  Nya::Utils::vectorToList({"waifu.pics" });

        this->sfw_endpoints = Nya::Utils::vectorToList({ 
            "waifu", "neko", "shinobu", "megumin", "bully", "cuddle", "cry", "hug", "awoo", "kiss", "lick", "pat", "smug", "bonk", "yeet", "blush", "smile", "wave", "highfive", "handhold", "nom", "bite", "glomp", "slap", "kill", "kick", "happy", "wink", "poke", "dance", "cringe" 
        });
        this->nsfw_endpoints = Nya::Utils::vectorToList({
            "waifu",
            "neko",
            "trap",
            "blowjob"
        });

        UIScreen = QuestUI::BeatSaberUI::CreateFloatingScreen({40.0f, 32.0f}, {0.0f, 1.0f, 1.0f}, {0, 0, 0}, 0.0f, true, true, 0);
        UIScreen->set_active(false);
        UIScreen->GetComponent<UnityEngine::Canvas*>()->set_sortingOrder(31);
        UnityEngine::GameObject::DontDestroyOnLoad(UIScreen);
        screenhandle = UIScreen->GetComponent<QuestUI::FloatingScreen*>()->handle;
        UIScreen->GetComponent<QuestUI::FloatingScreen*>()->bgGo->GetComponentInChildren<QuestUI::Backgroundable*>()->ApplyBackgroundWithAlpha("round-rect-panel", 1.0f);
        screenhandle->get_transform()->set_localPosition(UnityEngine::Vector3(0.0f, -23.0f, 0.0f));
        screenhandle->get_transform()->set_localScale(UnityEngine::Vector3(5.3f, 3.3f, 5.3f));
        QuestUI::FloatingScreen* thing = UIScreen->GetComponent<QuestUI::FloatingScreen*>();

        auto* vert = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(UIScreen->get_transform());
    
        vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        NYA = QuestUI::BeatSaberUI::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
        NYA->set_preserveAspect(true);
        auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
        auto view = NYA->get_gameObject()->AddComponent<Nya::ImageView*>();
        ele->set_preferredHeight(50);
        ele->set_preferredWidth(50);

        auto horz = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);

        this->nyaButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), "Nya", "PlayButton",
        [this]() {
            this->nyaButton->set_interactable(false);

            WebUtils::GetAsync(NyaAPI::get_api_path(), 10.0, [&](long code, std::string result){
                switch (code)
                {
                    case 200:
                        rapidjson::Document document;
                        document.Parse(result);
                        if(document.HasParseError() || !document.IsObject())
                            return;
                        std::string url = "";
                        if(document.HasMember("url"))
                        {
                            url = document.FindMember("url")->value.GetString();
                        }
                        // url = "https://giffiles.alphacoders.com/163/163685.gif";
                        getLogger().debug("%s", url.c_str());
                        
                        WebUtils::GetAsync(url, 10.0, [this, url](long code, std::string result){
                            std::vector<uint8_t> bytes(result.begin(), result.end());
                            
                            getLogger().debug("Downloaded Image!");
                            getLogger().debug("%lu", bytes.size());
                            switch (code)
                            {
                                case 200:
                                    if(url.find(".gif") != std::string::npos) {
                                        getLogger().debug("Found a gif");

                                        
                                            QuestUI::MainThreadScheduler::Schedule([this, result]
                                            {
                                                std::string resCopy = result;

                                                // Decode the gif
                                                Gif gif(resCopy);
                                                int parseResult = gif.Parse();
                                                int slurpResult = gif.Slurp();
                                                int width = gif.get_width();
                                                int height = gif.get_height();
                                                int length = gif.get_length();
                                                AllFramesResult result = gif.get_all_frames();
                                
                                                Nya::ImageView* view = NYA->get_gameObject()->GetComponent<Nya::ImageView*>();
                                                view->UpdateImage(result.frames,result.timings,  (float)width, (float)height);
                                            });
                                    } else {
                                        getLogger().debug("Static image");
                                        QuestUI::MainThreadScheduler::Schedule([this, bytes]
                                        {
                                            Nya::ImageView* view = NYA->get_gameObject()->GetComponent<Nya::ImageView*>();
                                            UnityEngine::Sprite* sprite = QuestUI::BeatSaberUI::VectorToSprite(bytes);
                                            view->UpdateStaticImage(sprite);
                                        });
                                    }

                                    QuestUI::MainThreadScheduler::Schedule([this]
                                    {
                                        this->nyaButton->set_interactable(true);
                                    });
                            }
                        });
                        break;
                }
            });
        });

        // Settings button
        this->settingsButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), to_utf16("Settings"), "PracticeButton",
        [this]() {
            getLogger().debug("Settings button clicked");
            // Run UI on the main thread
            QuestUI::MainThreadScheduler::Schedule([this]
            {
                try {
                getLogger().debug("Run main thread");

                   this->settingsModal->Show(true, true, nullptr);


                std::string API = getNyaConfig().API.GetValue();
                std::string SFWEndpoint = getNyaConfig().SFWEndpoint.GetValue();
                getLogger().info("Selected sfw category: %s", SFWEndpoint.data());
                getLogger().info("Selected api: %s", API.data());


                getLogger().info("Checking api switch is null");
                // Restore api endpoint state
                if (this->api_switch != nullptr) {
                    getLogger().info("2");
                    if (this->api_list == nullptr) {
                        getLogger().info("api_list is null");
                    } else {
                        this->api_switch->SetTexts(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<StringW>*>(this->api_list));
                        getLogger().info("4");
                        int index = Nya::Utils::findStrIndexInList(this->api_list,API);
                        getLogger().info("Selected index: %i", index);
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

        UINoGlow = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>(), [](UnityEngine::Material* x) { return x->get_name() == "UINoGlow"; });
        // GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(createPanelNotes(line1, line2, line3)));

        auto* screenthingidk = thing->get_gameObject()->AddComponent<HMUI::Screen*>();
//        createModalUI(thing->get_transform());
//        createGridIndicator(modal->get_transform());

        auto* normalpointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
//        hoverClickHelper = Nya::addHoverClickHelper(normalpointer, screenhandle, thing);
//        modalHelper = Nya::addModalHelper(normalpointer, thing);
    }
  
    void NyaFloatingUI::onPause(){
        isPaused = true;
        if (this->UIScreen != nullptr) {
            UIScreen->get_transform()->set_position(getNyaConfig().pausePosition.GetValue());
            UIScreen->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(getNyaConfig().resultRotation.GetValue()));
            UIScreen->set_active(true);
            auto* pausepointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(1);
            // Mover to move the ui component
            auto* mover = pausepointer->get_gameObject()->AddComponent<QuestUI::FloatingScreenMoverPointer*>();
            mover->Init(UIScreen->GetComponent<QuestUI::FloatingScreen*>(), pausepointer);
        } else {

        }
    }
    void NyaFloatingUI::onUnPause(){
        isPaused = false;
        if (UIScreen != nullptr){
//            modal->Hide(false, nullptr);
            UIScreen->set_active(false);
        }
    }

    // S
    void NyaFloatingUI::onResultsScreenActivate(){
//        auto* pointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
//        hoverClickHelper->vrPointer = pointer;
//        modalHelper->vrPointer = pointer;
//        hoverClickHelper->resetBools();
        UIScreen->get_transform()->set_position(getNyaConfig().resultPosition.GetValue());
        UIScreen->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(getNyaConfig().resultRotation.GetValue()));
        UIScreen->set_active(true);
//        modal->Show(false, true, nullptr);
//        modal->Hide(false, nullptr);
    }

    // When results screen os deactivated
    void NyaFloatingUI::onResultsScreenDeactivate(){
        if (UIScreen != nullptr){
            // TODO:Hide modal window
//            modal->Hide(false, nullptr);
            UIScreen->set_active(false);
        }
    }

    // Saves the coordinates to a config
    void NyaFloatingUI::updateCoordinates(UnityEngine::Transform* transform){
        if (Nya::Main::NyaFloatingUI->isPaused){
            getNyaConfig().pausePosition.SetValue(transform->get_position());
            getNyaConfig().pauseRotation.SetValue(transform->get_rotation().get_eulerAngles());
        }
        else{
            getNyaConfig().resultPosition.SetValue(transform->get_position());
            getNyaConfig().resultPosition.SetValue(transform->get_rotation().get_eulerAngles());
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
}