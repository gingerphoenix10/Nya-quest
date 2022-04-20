#include "main.hpp"
#include "ModifiersMenu.hpp"
#include "SettingsViewController.hpp"
#include "questui/shared/QuestUI.hpp"
#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/PauseMenuManager.hpp"
#include "GlobalNamespace/GamePause.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/NoteCutDirection.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/MultiplayerResultsViewController.hpp"
#include "GlobalNamespace/GameServerLobbyFlowCoordinator.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "NyaConfig.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace Nya;

DEFINE_CONFIG(NyaConfig);

Nya::NyaFloatingUI* Nya::Main::NyaFloatingUI = nullptr;

MAKE_HOOK_MATCH(Pause, &GamePause::Pause, void, GamePause* self) {
    Pause(self);
    if (getNyaConfig().inPause.GetValue()) Nya::Main::NyaFloatingUI->onPause();
}

MAKE_HOOK_MATCH(Unpause, &GamePause::Resume, void, GlobalNamespace::GamePause* self) {
    Unpause(self);
    if (!getNyaConfig().inGame.GetValue()) Nya::Main::NyaFloatingUI->onUnPause();
}

MAKE_HOOK_MATCH(Menubutton, &PauseMenuManager::MenuButtonPressed , void, PauseMenuManager* self) {
    Menubutton(self);
    if (!getNyaConfig().inMenu.GetValue()) Nya::Main::NyaFloatingUI->onUnPause();
}

MAKE_HOOK_MATCH(Restartbutton, &PauseMenuManager::RestartButtonPressed, void, PauseMenuManager* self) {
    Restartbutton(self);
    if (!getNyaConfig().inGame.GetValue()) Nya::Main::NyaFloatingUI->onUnPause();
}

MAKE_HOOK_MATCH(Results, &ResultsViewController::Init, void, ResultsViewController* self, LevelCompletionResults* levelCompletionResults, IReadonlyBeatmapData* transformedBeatmapData, IDifficultyBeatmap* difficultyBeatmap, bool practice, bool newHighScore) {
    Results(self, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, practice, newHighScore);
    if (getNyaConfig().inResults.GetValue()) Nya::Main::NyaFloatingUI->onResultsScreenActivate();
}

MAKE_HOOK_MATCH(Unresults, &ResultsViewController::DidDeactivate, void, ResultsViewController* self, bool removedFromHierarchy, bool screenSystemDisabling) {
    Unresults(self, removedFromHierarchy, screenSystemDisabling);
    if (Nya::NyaFloatingUI::isEnabled()) Nya::Main::NyaFloatingUI->onResultsScreenDeactivate();
}

MAKE_HOOK_MATCH(MultiResults, &MultiplayerResultsViewController::DidActivate, void, MultiplayerResultsViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MultiResults(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (Nya::NyaFloatingUI::isEnabled()) Nya::Main::NyaFloatingUI->onResultsScreenActivate();
}

MAKE_HOOK_MATCH(BackToLobbyButtonPressed, &MultiplayerResultsViewController::BackToLobbyPressed, void, MultiplayerResultsViewController* self) {
    BackToLobbyButtonPressed(self);
    if (Nya::NyaFloatingUI::isEnabled()) Nya::Main::NyaFloatingUI->onResultsScreenDeactivate();
}


MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self,  bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
     MainMenuViewController_DidActivate(self, firstActivation, addedToHeirarchy, screenSystemEnabling);
     getLogger().info("Main menu show!");
//     if (Nya::Main::config.isEnabled) Nya::Main::NyaFloatingUI->onPause();
}

MAKE_HOOK_MATCH(BackToMenuButtonPressed, &MultiplayerResultsViewController::BackToMenuPressed, void, MultiplayerResultsViewController* self) {
    BackToMenuButtonPressed(self);
    // if (Nya::Main::config.isEnabled) Nya::Main::NyaFloatingUI->onResultsScreenDeactivate();
}

MAKE_HOOK_MATCH(UnMultiplayer, &GameServerLobbyFlowCoordinator::DidDeactivate, void, GameServerLobbyFlowCoordinator* self, bool removedFromHierarchy, bool screenSystemDisabling){
    UnMultiplayer(self, removedFromHierarchy, screenSystemDisabling);
    if (Nya::NyaFloatingUI::isEnabled() && Nya::Main::NyaFloatingUI != nullptr) Nya::Main::NyaFloatingUI->onResultsScreenDeactivate();
}

MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE(GameplayCoreSceneSetupData_ctor, "", "GameplayCoreSceneSetupData", ".ctor", void, GameplayCoreSceneSetupData* self, IDifficultyBeatmap* difficultyBeatmap, IPreviewBeatmapLevel* previewBeatmapLevel, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects, EnvironmentInfoSO* environmentInfo, ColorScheme* colorScheme, MainSettingsModelSO* mainSettingsModel)
{
    GameplayCoreSceneSetupData_ctor(self, difficultyBeatmap, previewBeatmapLevel, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects, environmentInfo, colorScheme, mainSettingsModel);

    bool firstActivation = false;
    if (getNyaConfig().inGame.GetValue() ||
            getNyaConfig().inMenu.GetValue() ||
            getNyaConfig().inPause.GetValue() ||
            getNyaConfig().inResults.GetValue()) {
        if (Nya::Main::NyaFloatingUI == nullptr){
            firstActivation = true;
            Nya::Main::NyaFloatingUI = Nya::NyaFloatingUI::get_instance();
            Nya::Main::NyaFloatingUI->initScreen();
        }
        Nya::Main::NyaFloatingUI->UIScreen->set_active(false);

        // if (firstActivation) Nya::Main::NyaFloatingUI->initNoteData();
            // else Nya::Main::NyaFloatingUI->refreshNoteData();
            // Nya::Main::NyaFloatingUI->leftHand = colorScheme->get_saberAColor();
            // Nya::Main::NyaFloatingUI->rightHand = colorScheme->get_saberBColor();
        }
    else if (!Nya::NyaFloatingUI::isEnabled() && Nya::Main::NyaFloatingUI != nullptr){
        GameObject::Destroy(Nya::Main::NyaFloatingUI->UIScreen->get_gameObject());

        GameObject::Destroy(Nya::Main::NyaFloatingUI->get_gameObject());
        delete Nya::Main::NyaFloatingUI;
        Nya::Main::NyaFloatingUI = nullptr;
    }
}

MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &MenuTransitionsHelper::RestartGame, void, MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer*>* finishCallback)
{
    if (Nya::Main::NyaFloatingUI != nullptr){
        GameObject::Destroy(Nya::Main::NyaFloatingUI->UIScreen->get_gameObject());
        delete Nya::Main::NyaFloatingUI;
        Nya::Main::NyaFloatingUI = nullptr;
    }
    MenuTransitionsHelper_RestartGame(self, finishCallback);
}


// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    // Load the config - make sure this is after il2cpp_functions::Init();
    getNyaConfig().Init(modInfo);
    QuestUI::Init();

    QuestUI::Register::RegisterGameplaySetupMenu<Nya::ModifiersMenu*>(modInfo, "Nya");
    QuestUI::Register::RegisterModSettingsViewController<Nya::SettingsViewController*>(modInfo, "Nya");


    custom_types::Register::AutoRegister();

    getLogger().info("Installing hooks...");
    // Install our hooks
    INSTALL_HOOK(getLogger(), Pause);
    INSTALL_HOOK(getLogger(), Results);
    INSTALL_HOOK(getLogger(), GameplayCoreSceneSetupData_ctor);
    INSTALL_HOOK(getLogger(), Unpause);
    INSTALL_HOOK(getLogger(), Menubutton);
    INSTALL_HOOK(getLogger(), Restartbutton);
    INSTALL_HOOK(getLogger(), Unresults);
    INSTALL_HOOK(getLogger(), MultiResults);
    INSTALL_HOOK(getLogger(), BackToLobbyButtonPressed);
    INSTALL_HOOK(getLogger(), BackToMenuButtonPressed);
    INSTALL_HOOK(getLogger(), UnMultiplayer);
    INSTALL_HOOK(getLogger(), MenuTransitionsHelper_RestartGame);
    INSTALL_HOOK(getLogger(), MainMenuViewController_DidActivate);

    getLogger().info("Installed all hooks!");
}
