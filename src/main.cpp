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
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "NyaConfig.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace Nya;

DEFINE_CONFIG(NyaConfig);

Nya::NyaFloatingUI* Nya::Main::NyaFloatingUI = nullptr;

MAKE_HOOK_MATCH(Pause, &GamePause::Pause, void, GamePause* self) {
    Pause(self);
    if (getNyaConfig().inPause.GetValue()) Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::Pause);
}

MAKE_HOOK_MATCH(Unpause, &GamePause::Resume, void, GlobalNamespace::GamePause* self) {
    Unpause(self);
    if (!getNyaConfig().inGame.GetValue()) Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::InGame);;
}

MAKE_HOOK_MATCH(Restartbutton, &PauseMenuManager::RestartButtonPressed, void, PauseMenuManager* self) {
    Restartbutton(self);
    if (!getNyaConfig().inGame.GetValue()) Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::InGame);
}

MAKE_HOOK_MATCH(Results, &ResultsViewController::Init, void, ResultsViewController* self, LevelCompletionResults* levelCompletionResults, IReadonlyBeatmapData* transformedBeatmapData, IDifficultyBeatmap* difficultyBeatmap, bool practice, bool newHighScore) {
    Results(self, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, practice, newHighScore);
    // if (getNyaConfig().inResults.GetValue()) Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::MainMenu);
}

MAKE_HOOK_MATCH(MultiResults, &MultiplayerResultsViewController::DidActivate, void, MultiplayerResultsViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MultiResults(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (Nya::NyaFloatingUI::isEnabled()) Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::Results);
}

MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE(GameplayCoreSceneSetupData_ctor, "", "GameplayCoreSceneSetupData", ".ctor", void, GameplayCoreSceneSetupData* self, IDifficultyBeatmap* difficultyBeatmap, IPreviewBeatmapLevel* previewBeatmapLevel, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects, EnvironmentInfoSO* environmentInfo, ColorScheme* colorScheme, MainSettingsModelSO* mainSettingsModel)
{
    GameplayCoreSceneSetupData_ctor(self, difficultyBeatmap, previewBeatmapLevel, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects, environmentInfo, colorScheme, mainSettingsModel);

}

// Soft restart in settings
MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &MenuTransitionsHelper::RestartGame, void, MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer*>* finishCallback)
{
    // Destroy the floating UI on soft restart
    if (Main::NyaFloatingUI != nullptr){
        GameObject::Destroy(Main::NyaFloatingUI->UIScreen->get_gameObject());
        GameObject::Destroy(Main::NyaFloatingUI);
        Main::NyaFloatingUI = nullptr;
    }
    MenuTransitionsHelper_RestartGame(self, finishCallback);
}

MAKE_HOOK_MATCH(MainFlowCoordinator_DidActivate, &GlobalNamespace::MainFlowCoordinator::DidActivate, void, GlobalNamespace::MainFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    
    static bool hasInited = false;
    Nya::Main::NyaFloatingUI = Nya::NyaFloatingUI::get_instance();
    Nya::Main::NyaFloatingUI->initScreen();
    Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::MainMenu);
    hasInited = true;
}

// On scene change
MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged, &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);
    
    // If floating ui exists
    if(prevScene.IsValid() && nextScene.IsValid()) {
        std::string prevSceneName(prevScene.get_name());
        std::string nextSceneName(nextScene.get_name());
        INFO("scene changed from {} to {}", prevSceneName, nextSceneName);

        if (Nya::Main::NyaFloatingUI != nullptr && Nya::Main::NyaFloatingUI->isInitialized ) {
            Nya::Main::NyaFloatingUI->OnActiveSceneChanged(prevScene, nextScene);
        }
    }   
}

void makeFolder() 
{    
    if (!direxists(NyaGlobals::nyaPath.c_str()))
    {
        int makePath = mkpath(NyaGlobals::nyaPath.c_str());
        if (makePath == -1)
        {
            ERROR("Failed to make Nya Folder path!");
        }
    }

    if (!direxists(NyaGlobals::imagesPath.c_str()))
    {
        int makePath = mkpath(NyaGlobals::imagesPath.c_str());
        if (makePath == -1)
        {
            ERROR("Failed to make Images Folder path!");
        }
    }
}

Logger& Nya::getLoggerOld() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Returns a logger, useful for printing debug messages
Paper::ConstLoggerContext<4UL> Nya::getLogger() {
    static auto fastContext = Paper::Logger::WithContext<"Nya">();
    return fastContext;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

    INFO("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    // Load the config - make sure this is after il2cpp_functions::Init();
    getNyaConfig().Init(modInfo);

    // Init our custom config branch
    EndpointConfig::migrate(getNyaConfig().config);

    // Make local folders if they do not exist
    makeFolder();
    QuestUI::Init();

    QuestUI::Register::RegisterGameplaySetupMenu<Nya::ModifiersMenu*>(modInfo, "Nya");
    QuestUI::Register::RegisterModSettingsViewController<Nya::SettingsViewController*>(modInfo, "Nya");


    custom_types::Register::AutoRegister();

    Nya::getLoggerOld().info("Installing hooks...");
    // Install our hooks
    INSTALL_HOOK(Nya::getLoggerOld(), Pause);
    INSTALL_HOOK(Nya::getLoggerOld(), Results);
    INSTALL_HOOK(Nya::getLoggerOld(), GameplayCoreSceneSetupData_ctor);
    INSTALL_HOOK(Nya::getLoggerOld(), Unpause);
    INSTALL_HOOK(Nya::getLoggerOld(), Restartbutton);
    INSTALL_HOOK(Nya::getLoggerOld(), MultiResults);
    INSTALL_HOOK(Nya::getLoggerOld(), MenuTransitionsHelper_RestartGame);
    INSTALL_HOOK(Nya::getLoggerOld(), SceneManager_Internal_ActiveSceneChanged);
    INSTALL_HOOK(Nya::getLoggerOld(), MainFlowCoordinator_DidActivate);

    Nya::getLoggerOld().info("Installed all hooks!");
}


