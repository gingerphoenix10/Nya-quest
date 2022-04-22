#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"
#include <string>
#include <iostream>
#include "config-utils/shared/config-utils.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"

#define NYA_PATH_FORMAT "/sdcard/ModData/%s/Mods/Nya/"

namespace NyaGlobals {
    static std::string nyaPath = string_format(NYA_PATH_FORMAT, Modloader::getApplicationId().c_str());;
    static std::string imagesPath = nyaPath + "Images/";
}

DECLARE_CONFIG(NyaConfig,
    CONFIG_VALUE(inPause, bool, "Show in pause", true);
    CONFIG_VALUE(inResults, bool, "Show in results", true);
    CONFIG_VALUE(inGame, bool, "Show in game", false);
    CONFIG_VALUE(inMenu, bool, "Show in menu", false);

    CONFIG_VALUE(API, std::string, "Selected API", "waifu.pics");
    CONFIG_VALUE(SFWEndpoint, std::string, "SFW endpoint", "neko");
    CONFIG_VALUE(NSFWEndpoint, std::string, "NSFW endpoint", "neko");

    CONFIG_VALUE(AutoNya, bool, "Auto Nya", false);
    CONFIG_VALUE(AutoNyaInGame, bool, "Auto Nya in game", false);
    CONFIG_VALUE(AutoNyaDelay, int, "Auto Nya delay", 10);

    CONFIG_VALUE(NSFWEnabled, bool, "NSFW Enabled", false);
    CONFIG_VALUE(SkipNSFWWarning, bool, "Skip NSFW warning", false);
    
    CONFIG_VALUE(pausePositionX, float, "Pause PositionX", -2.12f);
    CONFIG_VALUE(pausePositionY, float, "Pause PositionY", 1.52f);
    CONFIG_VALUE(pausePositionZ, float, "Pause PositionZ", 1.65f);

    CONFIG_VALUE(pauseRotationX, float, "Pause RotationX", 4.52f);
    CONFIG_VALUE(pauseRotationY, float, "Pause RotationY", 312.22f);
    CONFIG_VALUE(pauseRotationZ, float, "Pause RotationZ", 359.63f);


    CONFIG_VALUE(menuPositionX, float, "Menu PositionX", 0.09f);
    CONFIG_VALUE(menuPositionY, float, "Menu PositionY", 2.92f);
    CONFIG_VALUE(menuPositionZ, float, "Menu PositionZ", 1.60f);

    CONFIG_VALUE(menuRotationX, float, "Menu RotationX", 324.01f);
    CONFIG_VALUE(menuRotationY, float, "Menu RotationY", 1.62f);
    CONFIG_VALUE(menuRotationZ, float, "Menu RotationZ", 358.97f);


    CONFIG_VALUE(resultPositionX, float, "Result PositionX", 0.09f);
    CONFIG_VALUE(resultPositionY, float, "Result PositionY", 2.92f);
    CONFIG_VALUE(resultPositionZ, float, "result PositionZ", 1.60f);

    CONFIG_VALUE(resultRotationX, float, "Result RotationX", 324.01f);
    CONFIG_VALUE(resultRotationY, float, "Result RotationY", 1.62f);
    CONFIG_VALUE(resultRotationZ, float, "Result RotationZ", 358.97f);


    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(inPause);
        CONFIG_INIT_VALUE(inResults);
        CONFIG_INIT_VALUE(inGame);
        CONFIG_INIT_VALUE(inMenu);

        CONFIG_INIT_VALUE(API);
        CONFIG_INIT_VALUE(SFWEndpoint);
        CONFIG_INIT_VALUE(NSFWEndpoint);
        CONFIG_INIT_VALUE(AutoNya);
        CONFIG_INIT_VALUE(AutoNyaInGame);
        CONFIG_INIT_VALUE(AutoNyaDelay);
        CONFIG_INIT_VALUE(NSFWEnabled);
        CONFIG_INIT_VALUE(SkipNSFWWarning);


        // Positions because vectors crash

        CONFIG_INIT_VALUE(pausePositionX);
        CONFIG_INIT_VALUE(pausePositionY);
        CONFIG_INIT_VALUE(pausePositionZ);

        CONFIG_INIT_VALUE(pauseRotationX);
        CONFIG_INIT_VALUE(pauseRotationY);
        CONFIG_INIT_VALUE(pauseRotationZ);


        CONFIG_INIT_VALUE(menuPositionX);
        CONFIG_INIT_VALUE(menuPositionY);
        CONFIG_INIT_VALUE(menuPositionZ);

        CONFIG_INIT_VALUE(menuRotationX);
        CONFIG_INIT_VALUE(menuRotationY);
        CONFIG_INIT_VALUE(menuRotationZ);


        CONFIG_INIT_VALUE(resultPositionX);
        CONFIG_INIT_VALUE(resultPositionY);
        CONFIG_INIT_VALUE(resultPositionZ);

        CONFIG_INIT_VALUE(resultRotationX);
        CONFIG_INIT_VALUE(resultRotationY);
        CONFIG_INIT_VALUE(resultRotationZ);
    )
)