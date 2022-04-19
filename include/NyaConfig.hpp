#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include <string>
#include <iostream>


std::optional<bool> getBool(rapidjson::Value& obj, std::string_view fieldName, bool required = false);
std::optional<bool> setBool(rapidjson::Value& obj, std::string_view fieldName,  bool value, bool required = false);
std::optional<float> getFloat(rapidjson::Value& obj, std::string_view fieldName, bool required = false);
std::optional<float> setFloat(rapidjson::Value& obj, std::string_view fieldName, float value, bool required = false);
std::optional<std::string> getString(rapidjson::Value& obj, std::string_view fieldName, bool required = false);
std::optional<std::string> setString(rapidjson::Value& obj, std::string_view fieldName, std::string value, bool required = false);
std::optional<int> getInt(rapidjson::Value& obj, std::string_view fieldName, bool required = false);
std::optional<int> setInt(rapidjson::Value& obj, std::string_view fieldName, int value, bool required = false);

namespace Nya {
    class Config {
    public:
        bool isEnabled;
        bool inPause;
        bool inResults;
        bool inGame;
        bool inMenu;
        float pausePosX;
        float pausePosY;
        float pausePosZ;
        float pauseRotX;
        float pauseRotY;
        float pauseRotZ;

        float gamePosX;
        float gamePosY;
        float gamePosZ;
        float gameRotX;
        float gameRotY;
        float gameRotZ;

        float menuPosX;
        float menuPosY;
        float menuPosZ;
        float menuRotX;
        float menuRotY;
        float menuRotZ;

        float resultPosX;
        float resultPosY;
        float resultPosZ;
        float resultRotX;
        float resultRotY;
        float resultRotZ;

        // Variables for nya things
        std::string API;
        std::string SFWEndpoint;

        // Auto refresh and delay
        bool AutoNya;
        int AutoNyaDelay;
        
        #ifdef NSFW
        std::string NSFWEndpoint;
        bool NSFWEnabled;
        bool SkipNSFWWarning;
        #endif
    };
}
class ConfigHelper {
public:
    static bool LoadConfig(Nya::Config& con, ConfigDocument& config);
    static void CreateDefaultConfig(ConfigDocument& config);
};