#include "NyaFloatingUI.hpp"
#include "NyaConfig.hpp"
#include "main.hpp"
#include "Utils/WebUtils.hpp"
#include "Utils/Gif.hpp"
#include "Utils/Utils.hpp"
#include "ImageView.hpp"
#include "ModConfig.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

using namespace UnityEngine::UI;
using namespace UnityEngine;

namespace Nya {
    void NyaFloatingUI::createGridIndicator(UnityEngine::Transform* parent){
        auto* screen = QuestUI::BeatSaberUI::CreateFloatingScreen({300.0f, 200.0f}, {0.0f, 1.0f, 1.0f}, {0, 0, 0}, 0.0f, true, false, 0);
        screen->GetComponent<QuestUI::FloatingScreen*>()->bgGo->GetComponentInChildren<QuestUI::Backgroundable*>()->ApplyBackgroundWithAlpha("round-rect-panel", 0.985f);
        auto* vert = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(screen->get_transform());
        auto* line1 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());
        auto* line2 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());
        auto* line3 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());

        line1->set_spacing(60.8f); line2->set_spacing(60.8f); line3->set_spacing(60.8f); vert->set_spacing(-5.5f);

        // GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(createGridDots(line1, line2, line3)));

        screen->get_transform()->set_position(screen->get_transform()->get_localPosition() + Vector3(0.0f, 15.0f, -1.2f));
        screen->get_transform()->SetParent(parent, false);
    }

    // Function gets url for the current selected category
std::string get_api_path() {
    // Get all config parametes
    std::string API = getModConfig().API.GetValue();
    std::string SFWEndpoint = getModConfig().SFWEndpoint.GetValue();
    
    #ifdef NSFW
        std::string NSFWEndpoint = getModConfig().NSFWEndpoint.GetValue();
        bool NSFWEnabled = getModConfig().NSFWEnabled.GetValue();
    #else
        bool NSFWEnabled = false;
        std::string NSFWEndpoint = "";
    #endif

    if (API == "waifu.pics") {
        std::string url = "https://api.waifu.pics/";
        url += NSFWEnabled? "nsfw/": "sfw/";        

        if (NSFWEnabled) {
            url += NSFWEndpoint;
        } else {
            url += SFWEndpoint;
        };

        return url;
    } else {
        return "";
    }
}


    void NyaFloatingUI::createModalUI(UnityEngine::Transform* parent){
        modal = QuestUI::BeatSaberUI::CreateModal(parent, UnityEngine::Vector2(70, 32), [](HMUI::ModalView *modal) {}, true);
        modal->get_transform()->Translate({0.0f, 0.0f, -0.01f});

        auto* mainLayout = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(modal->get_transform());

        auto* leftSide = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(mainLayout->get_transform());
        auto* rightSide = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(mainLayout->get_transform());

        auto* leftLine1 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(leftSide->get_transform());
        auto* leftLine2 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(leftSide->get_transform());
        auto* leftLine3 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(leftSide->get_transform());

        auto* rightLine1 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(rightSide->get_transform());
        auto* rightLine2 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(rightSide->get_transform());
        auto* rightLine3 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(rightSide->get_transform());

        leftLine1->set_spacing(9.4f); leftLine2->set_spacing(9.4f); leftLine3->set_spacing(9.4f); leftSide->set_spacing(9.4f);

        rightLine1->set_spacing(9.4f); rightLine2->set_spacing(9.4f); rightLine3->set_spacing(9.4f); rightSide->set_spacing(9.4f);

        // GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(createNotes(leftLine1, leftLine2, leftLine3, rightLine1, rightLine2, rightLine3)));
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
        screenhandle->get_transform()->set_localPosition(UnityEngine::Vector3(0.0f, -15.0f, 0.0f));
        screenhandle->get_transform()->set_localScale(UnityEngine::Vector3(3.3f, 3.3f, 3.3f));
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

            WebUtils::GetAsync(get_api_path(), 10.0, [&](long code, std::string result){
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



        UINoGlow = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>(), [](UnityEngine::Material* x) { return x->get_name() == "UINoGlow"; });
        // GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(createPanelNotes(line1, line2, line3)));

        auto* screenthingidk = thing->get_gameObject()->AddComponent<HMUI::Screen*>();
        createModalUI(thing->get_transform());
        createGridIndicator(modal->get_transform());

        auto* normalpointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
        hoverClickHelper = Nya::addHoverClickHelper(normalpointer, screenhandle, thing);
        modalHelper = Nya::addModalHelper(normalpointer, thing);
    }
  
    void NyaFloatingUI::onPause(){
        isPaused = true;
        UIScreen->get_transform()->set_position(UnityEngine::Vector3(Nya::Main::config.pausePosX, Nya::Main::config.pausePosY, Nya::Main::config.pausePosZ));
        UIScreen->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(Nya::Main::config.pauseRotX, Nya::Main::config.pauseRotY, Nya::Main::config.pauseRotZ));
        UIScreen->set_active(true);
        modal->Show(false, true, nullptr);
        modal->Hide(false, nullptr);
        
        auto* pausepointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(1);
        auto* mover = pausepointer->get_gameObject()->AddComponent<QuestUI::FloatingScreenMoverPointer*>();
        mover->Init(UIScreen->GetComponent<QuestUI::FloatingScreen*>(), pausepointer);
        hoverClickHelper->vrPointer = pausepointer;
        modalHelper->vrPointer = pausepointer;
        hoverClickHelper->resetBools();
    }
    void NyaFloatingUI::onUnPause(){
        isPaused = false;
        if (UIScreen != nullptr){
            modal->Hide(false, nullptr);
            UIScreen->set_active(false);
        }
    }
    void NyaFloatingUI::onResultsScreenActivate(){
        auto* pointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
        hoverClickHelper->vrPointer = pointer;
        modalHelper->vrPointer = pointer;
        hoverClickHelper->resetBools();
        UIScreen->get_transform()->set_position(UnityEngine::Vector3(Nya::Main::config.resultPosX , Nya::Main::config.resultPosY, Nya::Main::config.resultPosZ));
        UIScreen->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(Nya::Main::config.resultRotX, Nya::Main::config.resultRotY, Nya::Main::config.resultRotZ));
        UIScreen->set_active(true);
        modal->Show(false, true, nullptr);
        modal->Hide(false, nullptr);
    }
    void NyaFloatingUI::onResultsScreenDeactivate(){
        if (UIScreen != nullptr){
            modal->Hide(false, nullptr);
            UIScreen->set_active(false);
        }
    }

    void NyaFloatingUI::updateCoordinates(UnityEngine::Transform* transform){
        if (Nya::Main::NyaFloatingUI->isPaused){
            setFloat(getConfig().config, "pausePosX", transform->get_position().x);
            setFloat(getConfig().config, "pausePosY", transform->get_position().y);
            setFloat(getConfig().config, "pausePosZ", transform->get_position().z);
            getConfig().Write();
            setFloat(getConfig().config, "pauseRotX", transform->get_rotation().get_eulerAngles().x);
            setFloat(getConfig().config, "pauseRotY", transform->get_rotation().get_eulerAngles().y);
            setFloat(getConfig().config, "pauseRotZ", transform->get_rotation().get_eulerAngles().z);
            getConfig().Write();
        }
        else{
            setFloat(getConfig().config, "resultPosX", transform->get_position().x);
            setFloat(getConfig().config, "resultPosY", transform->get_position().y);
            setFloat(getConfig().config, "resultPosZ", transform->get_position().z);
            getConfig().Write();
            setFloat(getConfig().config, "resultRotX", transform->get_rotation().get_eulerAngles().x);
            setFloat(getConfig().config, "resultRotY", transform->get_rotation().get_eulerAngles().y);
            setFloat(getConfig().config, "resultRotZ", transform->get_rotation().get_eulerAngles().z);
            getConfig().Write();
        }
        ConfigHelper::LoadConfig(Nya::Main::config, getConfig().config);
    }
}