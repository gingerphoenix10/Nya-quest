#include "API.hpp"


    // Function gets url for the current selected category
    std::string NyaAPI::get_api_path() {
        // Get all config parametes
        std::string API = Nya::Main::config.API;
        std::string SFWEndpoint = Nya::Main::config.SFWEndpoint;
        getLogger().info("Selected sfw category: %s", SFWEndpoint.data());
        getLogger().info("Selected api :%s", API.data());
#ifdef NSFW

        std::string NSFWEndpoint = Nya::Main::config.NSFWEndpoint;
        getLogger().info("Selected nsfw category :%s", NSFWEndpoint.data());
        bool NSFWEnabled = Nya::Main::config.NSFWEnabled;
        getLogger().info("NSFW enabled :%i", NSFWEnabled);
#else
        bool NSFWEnabled = false;
        std::string NSFWEndpoint = "";
#endif

        if (API == "waifu.pics") {
            std::string url = "https://api.waifu.pics/";
            url += NSFWEnabled ? "nsfw/" : "sfw/";

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
