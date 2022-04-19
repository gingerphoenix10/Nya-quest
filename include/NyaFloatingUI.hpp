// This code is mostly taken from https://github.com/Royston1999/SliceDetails-Quest

#pragma once
#include "UI/HoverClickHelper.hpp"
#include <cmath>
#define _USE_MATH_DEFINES
#include "HMUI/ModalView.hpp"
#include "UI/PanelUI.hpp"
#include "custom-types/shared/macros.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreenMoverPointer.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/Vector3.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "HMUI/Screen.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Button.hpp"

using namespace UnityEngine::UI;


DECLARE_CLASS_CODEGEN(Nya, NyaFloatingUI, UnityEngine::MonoBehaviour,
        public:
            void initScreen();
            void onPause();
            void onUnPause();
            void onResultsScreenActivate();
            void onResultsScreenDeactivate();
            void updateCoordinates(UnityEngine::Transform* transform);

            DECLARE_CTOR(ctor);

            DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, screenhandle);
            DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, UIScreen);
            DECLARE_INSTANCE_FIELD(UnityEngine::Material*, UINoGlow);
            bool isPaused = false;
            DECLARE_INSTANCE_FIELD(Nya::HoverClickHelper*, hoverClickHelper);
//            Nya::ModalHelper* modalHelper = nullptr;

            // NYA
            DECLARE_INSTANCE_FIELD(HMUI::ImageView*, NYA);

            DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, nyaButton);
            // Settings buttons and modal
            DECLARE_INSTANCE_FIELD(HMUI::ModalView*, settingsModal);
            DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, settingsButton);
            DECLARE_INSTANCE_FIELD(HMUI::SimpleTextDropdown*, api_switch);
            DECLARE_INSTANCE_FIELD(HMUI::SimpleTextDropdown*, sfw_endpoint);
            #ifdef NSFW
                DECLARE_INSTANCE_FIELD(HMUI::SimpleTextDropdown*, nsfw_endpoint);
                DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, nsfw_toggle);
            #endif
            DECLARE_INSTANCE_FIELD(List<StringW>*, sfw_endpoints);
            DECLARE_INSTANCE_FIELD(List<StringW>*, nsfw_endpoints);
            DECLARE_INSTANCE_FIELD(List<StringW>*, api_list);

        static NyaFloatingUI* get_instance();
private:
        static NyaFloatingUI* instance;
)