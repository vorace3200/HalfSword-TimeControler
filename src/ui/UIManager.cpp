#include "UIManager.hpp"
#include "../game/AddressResolver.hpp"
#include "../core/MemoryReader.hpp"
#include "../core/Offsets.hpp"
#include "../../imgui/imgui.h"
#include <windows.h>

void UIManager::Init(LPDIRECT3DDEVICE9 device) {
    device_ = device;
    ImGui::StyleColorsDark();
}

void UIManager::Render() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(500, 500));
    
    if (!ImGui::Begin("TimeControler - Version 1.0.3 (Early Access)", &isOpen_, 
                      ImGuiWindowFlags_NoResize)) {
        ImGui::End();
        return;
    }
    
    if (gameState_.GetAddresses().IsValid()) {
        const char* tabs[] = { "Time", "Player", "Movement", "Combat", "Enemy", "Debug" };
        
        if (ImGui::BeginTabBar("MainTabs")) {
            for (int i = 0; i < 6; i++) {
                if (ImGui::BeginTabItem(tabs[i])) {
                    config_.currentTab = i;
                    switch (i) {
                        case 0: RenderTimeTab(); break;
                        case 1: RenderPlayerTab(); break;
                        case 2: RenderMovementTab(); break;
                        case 3: RenderCombatTab(); break;
                        case 4: RenderEnemyTab(); break;
                        case 5: RenderDebugTab(); break;
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    } else {
        ImGui::Text("Open halfsword");
    }
    
    RenderFooter();
    ImGui::End();
}

void UIManager::Update() {
    DWORD currentTime = GetTickCount();
    
    if (currentTime - lastTimeUpdateTime_ >= TIME_UPDATE_INTERVAL) {
        UpdateTimeControl();
        lastTimeUpdateTime_ = currentTime;
    }
    
    if (currentTime - lastPlayerUpdateTime_ >= PLAYER_UPDATE_INTERVAL) {
        UpdatePlayerCheats();
        lastPlayerUpdateTime_ = currentTime;
    }
}

void UIManager::UpdateTimeControl() {
    if (config_.toggleMode) {
        if (hotkeyManager_.IsTriggered()) {
            config_.timeActive = !config_.timeActive;
        }
    } else {
        config_.timeActive = hotkeyManager_.IsKeyDown();
    }

    if (gameState_.GetAddresses().IsValid()) {
        timeController_.Update(config_.timeActive, config_.slowTime);
    }
}

void UIManager::UpdatePlayerCheats() {

    if (!playerManager_.HasPlayer()) {

        lastPlayerUpdateTime_ += PLAYER_UPDATE_INTERVAL * 5;
        return;
    }
    
    playerManager_.SetGodMode(config_.playerConfig.godModeEnabled);

    if (config_.playerConfig.infiniteStamina) {
        playerManager_.SetInfiniteStamina();
    }
    if (config_.playerConfig.noPain) {
        playerManager_.SetNoPain();
    }
    if (config_.playerConfig.maxConsciousness) {
        playerManager_.SetMaxConsciousness();
    }
    if (config_.playerConfig.infiniteKick) {
        playerManager_.SetInfiniteKick(true);
    }
    
    playerManager_.SetWalkSpeed(config_.playerConfig.superSpeed);
    playerManager_.SetJumpPower(config_.playerConfig.superJump);
    playerManager_.SetGravityScale(config_.playerConfig.gravityScale);
    playerManager_.SetMass(config_.playerConfig.playerMass);
    playerManager_.SetGroundFriction(config_.playerConfig.groundFriction);
    
    playerManager_.SetMusclePower(config_.playerConfig.musclePower);
    playerManager_.SetPunchForce(config_.playerConfig.punchForce);
    playerManager_.SetKickForce(config_.playerConfig.kickForce);
    playerManager_.SetDamageMultiplier(config_.playerConfig.damageMultiplier);
}

void UIManager::RenderTimeTab() {
    ImGui::Text("Current TimeDilation: %.2f", gameState_.GetTimeDilation());
    ImGui::SliderFloat("Slow Time", &config_.slowTime, 0.1f, 2.0f, "%.2f");
    ImGui::Checkbox("Toggle Mode", &config_.toggleMode);
    
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Hotkey: %s", HotkeyManager::GetKeyName(hotkeyManager_.GetKey()));
    
    if (ImGui::Button("Change Hotkey")) {
        config_.waitingForKey = true;
    }
    
    if (config_.waitingForKey) {
        ImGui::Text("Press a key...");
        
        size_t keyCount;
        const int* keys = HotkeyManager::GetValidKeys(keyCount);
        
        for (size_t i = 0; i < keyCount; i++) {
            if (GetAsyncKeyState(keys[i]) & 0x8000) {
                hotkeyManager_.SetKey(keys[i]);
                config_.waitingForKey = false;
                break;
            }
        }
    } else {
        ImGui::Text("TimeControler active: %s", config_.timeActive ? "ON" : "OFF");
    }
}

void UIManager::RenderPlayerTab() {
    ImGui::Spacing();
    
    if (!playerManager_.HasPlayer()) {
        ImGui::Text("Player not found. Start a match first.");
        return;
    }
    
    ImGui::Checkbox("God Mode (Invulnerable)", &config_.playerConfig.godModeEnabled);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Blocks all damage");
    
    ImGui::Spacing();
    ImGui::Checkbox("Infinite Stamina", &config_.playerConfig.infiniteStamina);
    ImGui::Checkbox("No Pain", &config_.playerConfig.noPain);
    ImGui::Checkbox("Max Consciousness", &config_.playerConfig.maxConsciousness);
    ImGui::Checkbox("Infinite Kick", &config_.playerConfig.infiniteKick);
    
    ImGui::Separator();
    ImGui::Spacing();
    
    static bool oneHitKill = false;
    if (ImGui::Checkbox("One-Hit Kill", &oneHitKill)) {
        uintptr_t gameInstance = gameState_.GetAddresses().gameInstance;
        if (gameInstance != 0 && processManager_.IsAttached()) {
            double damageRate = oneHitKill ? 9999.0 : 1.0;
            MemoryReader<double> writer(processManager_.GetProcess());
            writer.Write(gameInstance + Offsets::GameInstanceDamageRate, damageRate);
        }
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Massive damage multiplier");
    
    ImGui::Spacing();
    
    if (ImGui::Button("Heal Player")) {
        playerManager_.FullHeal();
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Restore Stamina")) {
        playerManager_.RestoreStamina();
    }
}

void UIManager::RenderMovementTab() {
    if (!playerManager_.HasPlayer()) {
        ImGui::Text("Player not found. Start a match first.");
        return;
    }
    
    float displaySpeed = config_.playerConfig.superSpeed;
    if (ImGui::SliderFloat("Walk Speed", &displaySpeed, 10.0f, 150.0f, "%.0f")) {
        config_.playerConfig.superSpeed = displaySpeed;
    }
    ImGui::SliderFloat("Jump Power", &config_.playerConfig.superJump, 100.0f, 2000.0f, "%.0f");
    ImGui::SliderFloat("Gravity Scale", &config_.playerConfig.gravityScale, 0.1f, 3.0f, "%.1f");
    ImGui::SliderFloat("Player Mass", &config_.playerConfig.playerMass, 1.0f, 500.0f, "%.0f");
    ImGui::SliderFloat("Ground Friction", &config_.playerConfig.groundFriction, 0.0f, 20.0f, "%.1f");
    
    ImGui::Spacing();
    if (ImGui::Button("Reset to Default")) {
        config_.playerConfig.superSpeed = 60.0f;
        config_.playerConfig.superJump = 420.0f;
        config_.playerConfig.gravityScale = 1.0f;
        config_.playerConfig.playerMass = 100.0f;
        config_.playerConfig.groundFriction = 8.0f;
    }
}

void UIManager::RenderCombatTab() {
    if (!playerManager_.HasPlayer()) {
        ImGui::Text("Player not found. Start a match first.");
        return;
    }
    
    ImGui::SliderFloat("Punch Force", &config_.playerConfig.punchForce, 0.1f, 10.0f, "%.1f");
    ImGui::SliderFloat("Kick Force", &config_.playerConfig.kickForce, 0.1f, 10.0f, "%.1f");
    ImGui::SliderFloat("Damage Multiplier", &config_.playerConfig.damageMultiplier, 0.1f, 50.0f, "%.1f");
    ImGui::SliderFloat("Muscle Power", &config_.playerConfig.musclePower, 0.1f, 5.0f, "%.1f");
}

void UIManager::RenderEnemyTab() {
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "FEATURE DISABLED");
    ImGui::Text("Enemy system needs to be fixed.");
    ImGui::Text("Issues with external actor detection.");
}

void UIManager::RenderDebugTab() {
    ImGui::Text("Debug Information");
    ImGui::Separator();
    ImGui::Spacing();
    
    const auto& addrs = gameState_.GetAddresses();
    
    ImGui::Text("Base Addresses:");
    ImGui::Text("  UWorld: 0x%llX", addrs.uWorld);
    ImGui::Text("  PersistentLevel: 0x%llX", addrs.persistentLevel);
    ImGui::Text("  WorldSettings: 0x%llX", addrs.worldSettings);
    ImGui::Text("  GameInstance: 0x%llX", addrs.gameInstance);
    ImGui::Text("  Player: 0x%llX", addrs.player);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Calculated Addresses:");
    ImGui::Text("  TimeDilation: 0x%llX", addrs.timeDilation);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Offsets (NEW UPDATE):");
    ImGui::Text("  GWorld: 0x%llX", Offsets::GWorld);
    ImGui::Text("  PersistentLevel: 0x%llX", Offsets::PersistentLevel);
    ImGui::Text("  WorldSettings: 0x%llX", Offsets::WorldSettings);
    ImGui::Text("  TimeDilation: 0x%llX", Offsets::TimeDilation);
    ImGui::Text("  OwningGameInstance: 0x%llX", Offsets::OwningGameInstance);
    ImGui::Text("  LocalPlayers: 0x%llX", Offsets::LocalPlayers);
    ImGui::Text("  PlayerController: 0x%llX", Offsets::PlayerController);
    ImGui::Text("  AcknowledgedPawn: 0x%llX", Offsets::AcknowledgedPawn);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Status: %s", gameState_.GetStatus().c_str());
}

void UIManager::RenderFooter() {
    ImGui::Spacing();
    ImGui::SetCursorPos(ImVec2(
        ImGui::GetWindowSize().x - ImGui::CalcTextSize("made by vorace32").x - 10,
        ImGui::GetWindowSize().y - ImGui::GetTextLineHeight() - 10
    ));
    ImGui::Text("made by vorace32");
}
