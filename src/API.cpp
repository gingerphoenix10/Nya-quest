#include "API.hpp"


    // Function gets url for the current selected category
    std::string NyaAPI::get_api_path() {
        // Get all config parametes
        std::string API = getNyaConfig().API.GetValue();
        std::string SFWEndpoint = getNyaConfig().SFWEndpoint.GetValue();
        getLogger().info("Selected sfw category: %s", SFWEndpoint.data());
        getLogger().info("Selected api :%s", API.data());
#ifdef NSFW

        std::string NSFWEndpoint = getNyaConfig().NSFWEndpoint.GetValue();
        getLogger().info("Selected nsfw category :%s", NSFWEndpoint.data());
        bool NSFWEnabled = getNyaConfig().NSFWEnabled.GetValue();
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
