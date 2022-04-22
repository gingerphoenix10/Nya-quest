#include "SettingsViewController.hpp"

DEFINE_TYPE(Nya, SettingsViewController);

void Nya::SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (firstActivation) {
        auto *container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(get_transform());


        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Floating in Pause Menu",
                                           getNyaConfig().inPause.GetValue(), [](bool value) {
                    getNyaConfig().inPause.SetValue(value);

                });
        QuestUI::BeatSaberUI::CreateToggle(
                container->get_transform(),
                "Floating on Results Screen", getNyaConfig().inResults.GetValue(), [](bool value) {
                    getNyaConfig().inResults.SetValue(value);

                });
        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Floating on Menu Screen",
                                           getNyaConfig().inMenu.GetValue(),
                                           [](bool value) {
                                               getNyaConfig().inMenu.SetValue(value);

                                           });
         QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Floating in Game",
                                           getNyaConfig().inGame.GetValue(),
                                           [](bool value) {
                                               getNyaConfig().inGame.SetValue(value);

                                           });
    }
}