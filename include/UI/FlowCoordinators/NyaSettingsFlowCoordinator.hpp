#pragma once
#include "HMUI/ViewController.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/types.hpp"
#include "UI/ViewControllers/SettingsViewController.hpp"

using namespace Nya::UI;

DECLARE_CLASS_CODEGEN(Nya::UI::FlowCoordinators, NyaSettingsFlowCoordinator, HMUI::FlowCoordinator) {
    DECLARE_INSTANCE_FIELD(UnityW<ViewControllers::SettingsViewController>, SettingsViewController);

    DECLARE_INSTANCE_METHOD(void, Awake);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::FlowCoordinator::DidActivate, bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed, &HMUI::FlowCoordinator::BackButtonWasPressed, HMUI::ViewController* topViewController);

};