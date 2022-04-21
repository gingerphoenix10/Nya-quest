#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
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
    
    CONFIG_VALUE(pausePosition, UnityEngine::Vector3, "Pause Position", UnityEngine::Vector3(-1.648f, 1.516, 0.916f));
    CONFIG_VALUE(pauseRotation, UnityEngine::Vector3, "Pause Rotation", UnityEngine::Vector3(1.120f, 286.746f, 2.421f));

    CONFIG_VALUE(gamePosition, UnityEngine::Vector3, "Game Position", UnityEngine::Vector3(-1.648f, 1.516, 0.916f));
    CONFIG_VALUE(gameRotation, UnityEngine::Vector3, "Game Rotation", UnityEngine::Vector3(1.120f, 286.746f, 2.421f));

    CONFIG_VALUE(menuPosition, UnityEngine::Vector3, "Menu Position", UnityEngine::Vector3(-1.648f, 1.516, 0.916f));
    CONFIG_VALUE(menuRotation, UnityEngine::Vector3, "Menu Rotation", UnityEngine::Vector3(1.120f, 286.746f, 2.421f));

    CONFIG_VALUE(resultPosition, UnityEngine::Vector3, "Result Position", UnityEngine::Vector3(2.321f, 2.883f, 3.851f));
    CONFIG_VALUE(resultRotation, UnityEngine::Vector3, "Result Rotation", UnityEngine::Vector3(338.810f, 30.401f, 359.280f));

    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(pausePosition);
        CONFIG_INIT_VALUE(pauseRotation);
        CONFIG_INIT_VALUE(gamePosition);
        CONFIG_INIT_VALUE(gameRotation);
        CONFIG_INIT_VALUE(menuPosition);
        CONFIG_INIT_VALUE(menuRotation);
        CONFIG_INIT_VALUE(resultPosition);
        CONFIG_INIT_VALUE(resultRotation);

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
    )
)