#include "API.hpp"
#include "WebUtils.hpp"

inline std::map<std::string, SourceData> endpoint_data = {
    {"waifu.pics",
      {
         "https://api.waifu.pics/",
         DataMode::Json,
         {
            "sfw/neko", "sfw/waifu", "sfw/awoo", "sfw/shinobu", "sfw/megumin", "sfw/cuddle", "sfw/cry",
            "sfw/hug", "sfw/kiss", "sfw/lick", "sfw/pat", "sfw/smug", "sfw/bonk", "sfw/yeet", "sfw/blush",
            "sfw/smile", "sfw/wave", "sfw/highfive", "sfw/nom", "sfw/bite", "sfw/glomp", "sfw/slap",
            "sfw/kick", "sfw/happy", "sfw/wink", "sfw/poke", "sfw/dance"
         }, // Not including 'kill' because it's bad
         {
            "nsfw/neko", "nsfw/waifu", "nsfw/trap", "nsfw/blowjob"
         }
        }
    },
    {"xSky",
     {
         "https://api.nekos.cc/",
         DataMode::Json,
         {
            "neko", "catboy"
         },
         {
            "hentai", "bdsm", "furry", "ff", "feet", "trap", "gif", "futa"
         }
        }
    },
    {"Anime-Images API",
     {
         "https://anime-api.hisoka17.repl.co/img/",
         DataMode::Json,
         {
             "hug", "kiss", "slap", "wink", "pat", "kill", "cuddle", "punch", "waifu"
         },
         {
             "hentai", "boobs", "lesbian"
         }
        }
    },
    {"Catboys",
    {
         "https://api.catboys.com/",
         DataMode::Json,
         {
            "img", "baka"
         },
         {}
        }
    },
    {"Local Files",
        {
            "",
            DataMode::Local,
            {
                "/sfw"
            },
            {
                "/nsfw"
            }
        }
    }
};

SourceData NyaAPI::get_data_source(std::string name) {
    if (name.empty()) {
        throw "String is empty";
    }
    if (endpoint_data.find(name) == endpoint_data.end()) {
        throw "Endpoint not found";
    }
    else return endpoint_data[name];
}

std::vector<StringW> NyaAPI::get_source_list() {
    std::vector<StringW> keys;
    for (const auto& [key, _] : endpoint_data) {
        keys.push_back(key);
    }
    return keys;
}

    // void NyaAPI::downloadImageFile(
    //     std::string url,
    //     float timeoutInSeconds,
    //     std::function<void(bool success, std::string url)> finished) {
    //     WebUtils::GetAsync(NyaAPI::get_api_path(), 10.0, [&, finished](long code, std::string result){
            
    //     });
    // }

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

// Grabs path to image from api
void NyaAPI::get_path_from_api(
    std::string url,
    float timeoutInSeconds,
    std::function<void(bool success, std::string url)> finished) {
    WebUtils::GetAsync(url, 10.0, [&, finished](long code, std::string result){
        if (code != 200) {
            if(finished != nullptr) finished(false, "");
            return;
        }

        rapidjson::Document document;
        document.Parse(result);
        if(document.HasParseError() || !document.IsObject()) {
            if(finished != nullptr) finished(false, "");
            return;
        }
            
        std::string url = "";
        if(document.HasMember("url"))
        {
            url = document.FindMember("url")->value.GetString();
            if(finished != nullptr) finished(true, url);
        } else {
            if(finished != nullptr) finished(false, "");
            return;
        }
        
    });
}