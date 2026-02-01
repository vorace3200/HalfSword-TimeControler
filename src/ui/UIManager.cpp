#include "UIManager.hpp"
#include "../game/AddressResolver.hpp"
#include "../core/MemoryReader.hpp"
#include "../core/Offsets.hpp"
#include "../core/Logger.hpp"
#include "../../imgui/imgui.h"
#include <windows.h>

void UIManager::Init(LPDIRECT3DDEVICE9 device) {
    device_ = device;
    ImGui::StyleColorsDark();
}

void UIManager::Render() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(350, 270), ImGuiCond_FirstUseEver);
    
    if (!ImGui::Begin("TimeControler - Version 1.0.4 (Early Access)", &isOpen_)) {
        ImGui::End();
        return;
    }
    
    const auto& addrs = gameState_.GetAddresses();
    if (addrs.IsValid() && addrs.HasPlayer()) {
        const char* tabs[] = { "Time", "Player", "Debug" };
        
        if (ImGui::BeginTabBar("MainTabs")) {
            for (int i = 0; i < 3; i++) {
                if (ImGui::BeginTabItem(tabs[i])) {
                    config_.currentTab = i;
                    switch (i) {
                        case 0: RenderTimeTab(); break;
                        case 1: RenderPlayerTab(); break;
                        case 2: RenderDebugTab(); break;
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    } else if (addrs.IsValid() && !addrs.HasPlayer()) {
        ImGui::Text("Waiting for game to start...");
        ImGui::Text("Load into a match to enable features.");
    } else {
        ImGui::Text("Open halfsword");
    }
    
    RenderFooter();
    ImGui::End();
}

void UIManager::Update() {
    
    const auto& addrs = gameState_.GetAddresses();
    if (addrs.player == nullptr) {
        
        return;
    }
    
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

    
    
    const auto& addrs = gameState_.GetAddresses();
    if (addrs.IsValid() && addrs.player != nullptr) {
        timeController_.Update(config_.timeActive, config_.slowTime);
    }
}

void UIManager::UpdatePlayerCheats() {
    if (!playerManager_.HasPlayer()) {
        lastPlayerUpdateTime_ += PLAYER_UPDATE_INTERVAL * 5;
        return;
    }
    
    if (config_.playerConfig.godModeEnabled) {
        playerManager_.SetGodMode(config_.playerConfig.godModeEnabled);
    }

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
    
    ImGui::Text("Hotkey: %s", HotkeyManager::GetKeyName(hotkeyManager_.GetKey()).c_str());
    
    if (hotkeyManager_.IsListening()) {
        ImGui::Button("Press any key...", ImVec2(120, 0));
    } else {
        if (ImGui::Button("Change Hotkey")) {
            hotkeyManager_.StartListening();
        }
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("TimeControler active: %s", config_.timeActive ? "ON" : "OFF");
}

void UIManager::RenderPlayerTab() {
    ImGui::Spacing();
    
    if (!playerManager_.HasPlayer()) {
        ImGui::Text("Player not found. Start a match first.");
        return;
    }
    
    if (ImGui::Checkbox("God Mode (Invulnerable)", &config_.playerConfig.godModeEnabled)) {
        LOG_INFO("UIManager", "God Mode toggled: ", config_.playerConfig.godModeEnabled ? "ON" : "OFF");
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Blocks all damage");
    
    ImGui::Spacing();
    if (ImGui::Checkbox("Infinite Stamina", &config_.playerConfig.infiniteStamina)) {
        LOG_INFO("UIManager", "Infinite Stamina toggled: ", config_.playerConfig.infiniteStamina ? "ON" : "OFF");
    }
    if (ImGui::Checkbox("No Pain", &config_.playerConfig.noPain)) {
        LOG_INFO("UIManager", "No Pain toggled: ", config_.playerConfig.noPain ? "ON" : "OFF");
    }
    if (ImGui::Checkbox("Max Consciousness", &config_.playerConfig.maxConsciousness)) {
        LOG_INFO("UIManager", "Max Consciousness toggled: ", config_.playerConfig.maxConsciousness ? "ON" : "OFF");
    }
    if (ImGui::Checkbox("Infinite Kick", &config_.playerConfig.infiniteKick)) {
        LOG_INFO("UIManager", "Infinite Kick toggled: ", config_.playerConfig.infiniteKick ? "ON" : "OFF");
    }
    
    ImGui::Separator();
    ImGui::Spacing();
    
    static bool oneHitKill = false;
    if (ImGui::Checkbox("One-Hit Kill", &oneHitKill)) {
        LOG_INFO("UIManager", "One-Hit Kill toggled: ", oneHitKill ? "ON" : "OFF");
        uintptr_t gameInstance = gameState_.GetAddresses().gameInstance;
        if (gameInstance != 0 && processManager_.IsAttached()) {
            double damageRate = oneHitKill ? 9999.0 : 1.0;
            LOG_INFO("UIManager", "Writing damage rate: ", damageRate);
            MemoryReader<double> writer(processManager_.GetProcess(), "UIManager");
            bool success = writer.Write(gameInstance + Offsets::GameInstanceDamageRate, damageRate);
            if (success) {
                LOG_INFO("UIManager", "One-Hit Kill applied successfully");
            } else {
                LOG_ERROR("UIManager", "Failed to apply One-Hit Kill");
            }
        } else {
            LOG_WARNING("UIManager", "Cannot apply One-Hit Kill - GameInstance=", gameInstance, " Attached=", processManager_.IsAttached());
        }
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Massive damage multiplier");
    
    ImGui::Spacing();
    
    if (ImGui::Button("Heal Player")) {
        if (playerManager_.HasPlayer()) {
            playerManager_.FullHeal();
            LOG_INFO("UIManager", "Heal Player executed");
        } else {
            LOG_WARNING("UIManager", "Heal Player failed - No valid player");
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Restore Stamina")) {
        if (playerManager_.HasPlayer()) {
            playerManager_.RestoreStamina();
            LOG_INFO("UIManager", "Restore Stamina executed");
        } else {
            LOG_WARNING("UIManager", "Restore Stamina failed - No valid player");
        }
    }
}

void UIManager::RenderMovementTab() {
    if (!playerManager_.HasPlayer()) {
        ImGui::Text("Player not found. Start a match first.");
        return;
    }
    
    ImGui::Spacing();
    
    float displaySpeed = config_.playerConfig.superSpeed;
    if (ImGui::SliderFloat("Walk Speed", &displaySpeed, 10.0f, 1000.0f, "%.0f")) {
        LOG_INFO("UIManager", "Walk Speed changed: ", displaySpeed);
        config_.playerConfig.superSpeed = displaySpeed;
    }
    if (ImGui::SliderFloat("Jump Power", &config_.playerConfig.superJump, 100.0f, 4000.0f, "%.0f")) {
        LOG_INFO("UIManager", "Jump Power changed: ", config_.playerConfig.superJump);
    }
    if (ImGui::SliderFloat("Gravity Scale", &config_.playerConfig.gravityScale, 0.1f, 3.0f, "%.1f")) {
        LOG_INFO("UIManager", "Gravity Scale changed: ", config_.playerConfig.gravityScale);
    }
    if (ImGui::SliderFloat("Player Mass", &config_.playerConfig.playerMass, 1.0f, 500.0f, "%.0f")) {
        LOG_INFO("UIManager", "Player Mass changed: ", config_.playerConfig.playerMass);
    }
    if (ImGui::SliderFloat("Ground Friction", &config_.playerConfig.groundFriction, 0.0f, 20.0f, "%.1f")) {
        LOG_INFO("UIManager", "Ground Friction changed: ", config_.playerConfig.groundFriction);
    }
    
    ImGui::Spacing();
    if (ImGui::Checkbox("Remove Speed Limit", &config_.playerConfig.removeSpeedLimit)) {
        LOG_INFO("UIManager", "Remove Speed Limit toggled: ", config_.playerConfig.removeSpeedLimit ? "ON" : "OFF");
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Removes acceleration limits for faster movement");
    
    ImGui::Spacing();
    if (ImGui::Button("Reset to Default")) {
        LOG_INFO("UIManager", "Reset to Default button clicked");
        config_.playerConfig.superSpeed = 60.0f;
        config_.playerConfig.superJump = 420.0f;
        config_.playerConfig.gravityScale = 1.0f;
        config_.playerConfig.playerMass = 100.0f;
        config_.playerConfig.groundFriction = 8.0f;
        config_.playerConfig.removeSpeedLimit = false;
    }
}

void UIManager::RenderCombatTab() {
}

void UIManager::RenderEnemyTab() {
}

void UIManager::RenderDebugTab() {
    ImGui::Text("Debug Information");
    ImGui::Separator();
    ImGui::Spacing();
    
    
    if (ImGui::Checkbox("Enable Debug Console", &config_.debugEnabled)) {
        LOG_INFO("UIManager", "Debug mode ", config_.debugEnabled ? "enabled" : "disabled", " by user");
        Logger::GetInstance().SetDebugEnabled(config_.debugEnabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Opens a console window with detailed debug logs. Use this to diagnose crashes.");
    }
    
    ImGui::Spacing();
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
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::Button("Dump Player Info (Check Logs)")) {
        if (playerManager_.HasPlayer()) {
            playerManager_.DumpDebugInfo();
        } else {
            LOG_WARNING("Cannot dump info: No player found");
        }
    }
}

void UIManager::RenderFooter() {
    ImGui::Spacing();
    ImGui::SetCursorPos(ImVec2(
        ImGui::GetWindowSize().x - ImGui::CalcTextSize("made by vorace32").x - 10,
        ImGui::GetWindowSize().y - ImGui::GetTextLineHeight() - 10
    ));
    ImGui::Text("made by vorace32");
}
