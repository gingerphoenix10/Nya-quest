// This code is mostly taken from https://github.com/Royston1999/SliceDetails-Quest

#pragma once
#include "UI/HoverClickHelper.hpp"
#include <cmath>
#define _USE_MATH_DEFINES
#include "HMUI/ModalView.hpp"
#include "UI/PanelUI.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreenMoverPointer.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/Vector3.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "HMUI/Screen.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Button.hpp"

using namespace UnityEngine::UI;

namespace Nya {
    class NyaFloatingUI {
        public:
            void initScreen();

            void onPause();
            void onUnPause();
            void onResultsScreenActivate();
            void onResultsScreenDeactivate();
            void updateCoordinates(UnityEngine::Transform* transform);
            UnityEngine::GameObject* screenhandle = nullptr;
            UnityEngine::GameObject* UIScreen = nullptr;
//            HMUI::ModalView* modal = nullptr;
            UnityEngine::Material* UINoGlow = nullptr;
            bool isPaused = false;
            Nya::HoverClickHelper* hoverClickHelper = nullptr;
//            Nya::ModalHelper* modalHelper = nullptr;

            // NYA
            HMUI::ImageView* NYA;

            UnityEngine::UI::Button* nyaButton;
            // Settings buttons and modal
            HMUI::ModalView* settingsModal;
            UnityEngine::UI::Button* settingsButton;
            HMUI::SimpleTextDropdown* api_switch;
            HMUI::SimpleTextDropdown* sfw_endpoint;
            #ifdef NSFW
                HMUI::SimpleTextDropdown* nsfw_endpoint;
                UnityEngine::UI::Toggle* nsfw_toggle;
            #endif
            List<StringW>* sfw_endpoints;
            List<StringW>* nsfw_endpoints;
            List<StringW>* api_list;
    };
}