#pragma once
#include "paper2_scotland2/shared/logger.hpp"
// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "scotland2/shared/loader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "NyaFloatingUI.hpp"
#include "NyaConfig.hpp"
#include "EndpointConfigUtils.hpp"

inline modloader::ModInfo modInfo = {MOD_ID, VERSION, GIT_COMMIT}; // Stores the ID and version of our mod, and is sent to the modloader upon startup

namespace Nya {
    class Main {
        public:
            static SafePtrUnity<Nya::NyaFloatingUI> NyaFloatingUI;
    };

    // Creates/removes .nomedia based on config
    void ApplyIndexingRules();
    // Cleans temp folder
    void CleanTempFolder();
}