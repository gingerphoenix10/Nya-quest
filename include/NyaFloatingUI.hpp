// This code is mostly taken from https://github.com/Royston1999/SliceDetails-Quest

#pragma once

#include <cmath>
#define _USE_MATH_DEFINES
#include "UI/NoteUI.hpp"
#include "HMUI/ModalView.hpp"
#include "UI/PanelUI.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreenMoverPointer.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UI/HoverClickHelper.hpp"
#include "HMUI/Screen.hpp"
#include "UI/ModalHelper.hpp"

using namespace UnityEngine::UI;

namespace Nya {
    class NyaFloatingUI {
        public:
            custom_types::Helpers::Coroutine createGridDots(HorizontalLayoutGroup* line1, HorizontalLayoutGroup* line2, HorizontalLayoutGroup* line3);
            custom_types::Helpers::Coroutine createNotes(HorizontalLayoutGroup* leftLine1, HorizontalLayoutGroup* leftLine2, HorizontalLayoutGroup* leftLine3, HorizontalLayoutGroup* rightLine1, HorizontalLayoutGroup* rightLine2, HorizontalLayoutGroup* rightLine3);
            custom_types::Helpers::Coroutine createPanelNotes(HorizontalLayoutGroup* line1, HorizontalLayoutGroup* line2, HorizontalLayoutGroup* line3);
            void createGridIndicator(UnityEngine::Transform* parent);
            void createModalUI(UnityEngine::Transform* parent);
            void updateGridNotesInfo(int index);
            void updatePanelImages();
            void initScreen();
            void initNoteData();
            void refreshNoteData();
            void onPause();
            void onUnPause();
            void onResultsScreenActivate();
            void onResultsScreenDeactivate();
            void updateCoordinates(UnityEngine::Transform* transform);
            Nya::NoteUI* modalNotes[18];
            UnityEngine::GameObject* screenhandle = nullptr;
            UnityEngine::GameObject* UIScreen = nullptr;
            HMUI::ModalView* modal = nullptr;
            UnityEngine::Material* UINoGlow = nullptr;
            bool isPaused = false;
            Nya::HoverClickHelper* hoverClickHelper = nullptr;
            Nya::ModalHelper* modalHelper = nullptr;

            // NYA
            HMUI::ImageView* NYA;

            UnityEngine::UI::Button* nyaButton;
            // Settings buttons and modal
            HMUI::ModalView* settingsModal;
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