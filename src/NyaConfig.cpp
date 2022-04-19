#include <tuple>
#include "NyaConfig.hpp"
#include "main.hpp"

#define GET(obj, fieldName, method, required) auto itr = obj.FindMember(fieldName.data()); \
if (itr == obj.MemberEnd()) { \
    if (required) { \
    } \
    return std::nullopt; \
} \
return itr->value.method()

#define SET(obj, fieldName, val, method, required) auto itr = obj.FindMember(fieldName.data());     \
    if (itr == obj.MemberEnd()) {                                                                   \
        if (required) {                                                                             \
        }                                                                                           \
    }                                                                                               \
    itr->value.method(val);                                                                         \
    ConfigHelper::LoadConfig(Nya::Main::config, getConfig().config);                                \
    return true;

using namespace rapidjson;

std::optional<int> getInt(Value& obj, std::string_view fieldName, bool required) {
    GET(obj, fieldName, GetInt, required);
}

std::optional<std::string> getString(Value& obj, std::string_view fieldName, bool required) {
    GET(obj, fieldName, GetString, required);
}

std::optional<bool> getBool(Value& obj, std::string_view fieldName, bool required) {
    GET(obj, fieldName, GetBool, required);
}

std::optional<float> getFloat(Value& obj, std::string_view fieldName, bool required) {
    GET(obj, fieldName, GetFloat, required);
}

std::optional<bool> setBool(Value& obj, std::string_view fieldName, bool value, bool required) {
    SET(obj, fieldName, value, SetBool, required);
}

std::optional<float> setFloat(Value& obj, std::string_view fieldName, float value, bool required) {
    SET(obj, fieldName, value, SetFloat, required);
}

std::optional<std::string> setString(Value& obj, std::string_view fieldName, std::string val, bool required) {
     auto itr = obj.FindMember(fieldName.data()); 
    if (itr == obj.MemberEnd()) {                                                                 
        if (required) {                                                                           
        }                                                                                         
    }


    itr->value.SetString(val.data(), val.length(), getConfig().config.GetAllocator());
    ConfigHelper::LoadConfig(Nya::Main::config, getConfig().config);
    return "true";
}

std::optional<int> setInt(Value& obj, std::string_view fieldName, int value, bool required) {
    SET(obj, fieldName, value, SetInt, required);
}

bool ConfigHelper::LoadConfig(Nya::Config& con, ConfigDocument& config) {
    if (!config.HasMember("inPause")) ConfigHelper::CreateDefaultConfig(config);
    
    con.inPause = getBool(config, "inPause").value_or(false);
    con.inResults = getBool(config, "inResults").value_or(false);
    con.inGame = getBool(config, "inGame").value_or(false);
    con.inMenu = getBool(config, "inMenu").value_or(false);

    con.pausePosX = getFloat(config, "pausePosX").value_or(0);
    con.pausePosY = getFloat(config, "pausePosY").value_or(1);
    con.pausePosZ = getFloat(config, "pausePosZ").value_or(1);
    con.pauseRotX = getFloat(config, "pauseRotX").value_or(0);
    con.pauseRotY = getFloat(config, "pauseRotY").value_or(0);
    con.pauseRotZ = getFloat(config, "pauseRotZ").value_or(0);

    con.menuPosX = getFloat(config, "menuPosX").value_or(0);
    con.menuPosY = getFloat(config, "menuPosY").value_or(1);
    con.menuPosZ = getFloat(config, "menuPosZ").value_or(1);
    con.menuRotX = getFloat(config, "menuRotX").value_or(0);
    con.menuRotY = getFloat(config, "menuRotY").value_or(0);
    con.menuRotZ = getFloat(config, "menuRotZ").value_or(0);

    con.gamePosX = getFloat(config, "gamePosX").value_or(0);
    con.gamePosY = getFloat(config, "gamePosY").value_or(1);
    con.gamePosZ = getFloat(config, "gamePosZ").value_or(1);
    con.gameRotX = getFloat(config, "gameRotX").value_or(0);
    con.gameRotY = getFloat(config, "gameRotY").value_or(0);
    con.gameRotZ = getFloat(config, "gameRotZ").value_or(0);

    con.resultPosX = getFloat(config, "resultPosX").value_or(0);
    con.resultPosY = getFloat(config, "resultPosY").value_or(1);
    con.resultPosZ = getFloat(config, "resultPosZ").value_or(1);
    con.resultRotX = getFloat(config, "resultRotX").value_or(0);
    con.resultRotY = getFloat(config, "resultRotY").value_or(0);
    con.resultRotZ = getFloat(config, "resultRotZ").value_or(0);

    // Variables for nya things
    con.API = getString(config, "API").value_or("waifu.pics");
    con.SFWEndpoint =  getString(config, "SFWEndpoint").value_or("neko");
    
    // Auto refresh and delay
    con.AutoNya = getBool(config, "AutoNya").value_or(false);
    con.AutoNyaDelay = getInt(config, "AutoNyaDelay").value_or(false);
    

    #ifdef NSFW
    con.NSFWEndpoint =  getString(config, "NSFWEndpoint").value_or("neko");
    con.NSFWEnabled = getBool(config, "NSFWEnabled").value_or(false);
    con.SkipNSFWWarning = getBool(config, "SkipNSFWWarning").value_or(false);
    #endif

    con.isEnabled = con.inPause || con.inResults || con.inGame || con.inMenu; 
    return true;
}

void ConfigHelper::CreateDefaultConfig(ConfigDocument& config){
    config.SetObject();
    config.RemoveAllMembers();
    config.AddMember("inPause", true, config.GetAllocator());
    config.AddMember("inResults", true, config.GetAllocator());
    config.AddMember("inGame", true, config.GetAllocator());
    config.AddMember("inMenu", true, config.GetAllocator());
    getConfig().Write();
    config.AddMember("pausePosX", -1.648f, config.GetAllocator());
    config.AddMember("pausePosY", 1.516, config.GetAllocator());
    config.AddMember("pausePosZ", 0.916f, config.GetAllocator());
    config.AddMember("pauseRotX", 1.120f, config.GetAllocator());
    config.AddMember("pauseRotY", 286.746f, config.GetAllocator());
    config.AddMember("pauseRotZ", 2.421f, config.GetAllocator());
    getConfig().Write();
    config.AddMember("menuPosX", -1.648f, config.GetAllocator());
    config.AddMember("menuPosY", 1.516, config.GetAllocator());
    config.AddMember("menuPosZ", 0.916f, config.GetAllocator());
    config.AddMember("menuRotX", 1.120f, config.GetAllocator());
    config.AddMember("menuRotY", 286.746f, config.GetAllocator());
    config.AddMember("menuRotZ", 2.421f, config.GetAllocator());
    getConfig().Write();
    config.AddMember("gamePosX", -1.648f, config.GetAllocator());
    config.AddMember("gamePosY", 1.516, config.GetAllocator());
    config.AddMember("gamePosZ", 0.916f, config.GetAllocator());
    config.AddMember("gameRotX", 1.120f, config.GetAllocator());
    config.AddMember("gameRotY", 286.746f, config.GetAllocator());
    config.AddMember("gameRotZ", 2.421f, config.GetAllocator());
    getConfig().Write();
    config.AddMember("resultPosX", 2.321f, config.GetAllocator());
    config.AddMember("resultPosY", 2.883f, config.GetAllocator());
    config.AddMember("resultPosZ", 3.851f, config.GetAllocator());
    config.AddMember("resultRotX", 338.810f, config.GetAllocator());
    config.AddMember("resultRotY", 30.401f, config.GetAllocator());
    config.AddMember("resultRotZ", 359.280f, config.GetAllocator());
    getConfig().Write();
    

    config.AddMember("API", "waifu.pics", config.GetAllocator());
    config.AddMember("SFWEndpoint", "neko", config.GetAllocator());
    config.AddMember("AutoNya", false, config.GetAllocator());
    config.AddMember("AutoNyaDelay", 5, config.GetAllocator());
    
    #ifdef NSFW
    config.AddMember("NSFWEnabled", false, config.GetAllocator());
    config.AddMember("NSFWEndpoint", "neko", config.GetAllocator());
    config.AddMember("SkipNSFWWarning", false, config.GetAllocator());
    #endif

    getConfig().Write();
}