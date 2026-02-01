#pragma once
#include <windows.h>
#include <d3d9.h>
#include <string>
#include "../../imgui/imgui.h"
#include "../game/GameState.hpp"
#include "../game/PlayerManagerSDK.hpp"
#include "../game/Config.hpp"
#include "../game/TimeController.hpp"
#include "../game/EnemyManagerSDK.hpp"
#include "../game/GameModeManagerSDK.hpp"
#include "../core/HotkeyManager.hpp"
#include "../core/ProcessManager.hpp"

struct UIConfig {
    float slowTime = 0.5f;
    bool toggleMode = true;
    bool timeActive = false;
    bool waitingForKey = false;
    int currentTab = 0;
    PlayerConfig playerConfig;
    EnemyConfig enemyConfig;
    bool debugEnabled = false;
    DWORD keyDebounceTime = 0;
};

class UIManager {
public:
    UIManager(GameState& gameState, PlayerManagerSDK& playerManager,
              TimeController& timeController, EnemyManagerSDK& enemyManager,
              GameModeManagerSDK& gameModeManager,
              HotkeyManager& hotkeyManager, ProcessManager& processManager)
        : gameState_(gameState), playerManager_(playerManager),
          timeController_(timeController), enemyManager_(enemyManager),
          gameModeManager_(gameModeManager),
          hotkeyManager_(hotkeyManager), processManager_(processManager) {}
    
    void Init(LPDIRECT3DDEVICE9 device);
    void Render();
    void Update();
    
    bool ShouldClose() const { return !isOpen_; }

private:
    void RenderTimeTab();
    void RenderPlayerTab();
    void RenderMovementTab();
    void RenderCombatTab();
    void RenderEnemyTab();
    void RenderDebugTab();
    void RenderFooter();
    
    void UpdateTimeControl();
    void UpdatePlayerCheats();
    
    GameState& gameState_;
    PlayerManagerSDK& playerManager_;
    TimeController& timeController_;
    EnemyManagerSDK& enemyManager_;
    GameModeManagerSDK& gameModeManager_;
    HotkeyManager& hotkeyManager_;
    ProcessManager& processManager_;
    
    UIConfig config_;
    bool isOpen_ = true;
    LPDIRECT3DDEVICE9 device_ = nullptr;
    
    DWORD lastPlayerUpdateTime_ = 0;
    DWORD lastTimeUpdateTime_ = 0;
    static constexpr DWORD PLAYER_UPDATE_INTERVAL = 10;
    static constexpr DWORD TIME_UPDATE_INTERVAL = 50;
};
