#include "UI/FlowCoordinators/NyaSettingsFlowCoordinator.hpp"
#include "bsml/shared/Helpers/creation.hpp"

DEFINE_TYPE(Nya::UI::FlowCoordinators, NyaSettingsFlowCoordinator);


void Nya::UI::FlowCoordinators::NyaSettingsFlowCoordinator::Awake(){
    if (!SettingsViewController) {
        SettingsViewController = BSML::Helpers::CreateViewController<ViewControllers::SettingsViewController*>();
    }
}

void Nya::UI::FlowCoordinators::NyaSettingsFlowCoordinator::DidActivate(bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
    if (!firstActivation) return;

    SetTitle("Nya Settings", HMUI::ViewController::AnimationType::In);
    showBackButton = true;
    ProvideInitialViewControllers(SettingsViewController, nullptr, nullptr, nullptr, nullptr);
}
void Nya::UI::FlowCoordinators::NyaSettingsFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
    if (this->_parentFlowCoordinator) {
        this->_parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
    }
}