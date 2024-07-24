#pragma once

#include "main.hpp"
#include "NyaConfig.hpp"
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "HMUI/ModalView.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "API.hpp"
#include "Utils/Utils.hpp"
#include "ImageView.hpp"
#include "UI/Modals/SettingsMenu.hpp"

DECLARE_CLASS_CODEGEN(Nya, ModifiersMenu, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, NYA);
    // DECLARE_INSTANCE_FIELD(bool, );
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, nyaButton);
    DECLARE_INSTANCE_FIELD(UnityW<Nya::ImageView>, imageView);
    DECLARE_INSTANCE_FIELD(bool, initialized);


    

    DECLARE_INSTANCE_METHOD(void, DidActivate, bool firstActivation);

    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, OnEnable);

    DECLARE_INSTANCE_METHOD(void, OnIsLoadingChange, bool isLoading);    

    DECLARE_CTOR(ctor);
    DECLARE_DTOR(dtor);

    // Settings buttons and modal
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, settingsButton);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::ModalView>, settingsModal);
    DECLARE_INSTANCE_FIELD(UnityW<Nya::SettingsMenu>, settingsMenu);
    
)
