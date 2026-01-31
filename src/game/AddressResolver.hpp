#pragma once
#include <cstdint>
#include <windows.h>
#include <d3d9.h>
#include <string>
#include "../core/MemoryReader.hpp"
#include "../core/Offsets.hpp"
#include "../core/ProcessManager.hpp"
#include "../game/GameState.hpp"
#include "../sdk/PlayerSDK.hpp"

class AddressResolver {
public:
    AddressResolver(ProcessManager& processManager, GameState& gameState)
        : processManager_(processManager), gameState_(gameState) {}
    
    bool ResolveAddresses() {
        if (!processManager_.IsAttached()) {
            gameState_.SetStatus("Process not attached");
            return false;
        }
        
        HANDLE process = processManager_.GetProcess();
        uintptr_t base = processManager_.GetBaseAddress();
        MemoryReader<uintptr_t> ptrReader(process);
        
        GameAddresses addrs;
        
        addrs.uWorld = ptrReader.Read(base + Offsets::GWorld);
        if (addrs.uWorld == 0) {
            gameState_.SetStatus("Unable to read UWorld");
            return false;
        }
        
        addrs.persistentLevel = ptrReader.Read(addrs.uWorld + Offsets::PersistentLevel);
        if (addrs.persistentLevel == 0) {
            gameState_.SetStatus("Unable to read PersistentLevel");
            return false;
        }
        
        addrs.worldSettings = ptrReader.Read(addrs.persistentLevel + Offsets::WorldSettings);
        if (addrs.worldSettings == 0) {
            gameState_.SetStatus("Unable to read WorldSettings");
            return false;
        }
        
        addrs.timeDilation = addrs.worldSettings + Offsets::TimeDilation;
        
        addrs.gameInstance = ptrReader.Read(addrs.uWorld + Offsets::OwningGameInstance);
        if (addrs.gameInstance != 0) {
            addrs.playerAddress = ResolvePlayerAddress(addrs.gameInstance);
            addrs.player = (HalfSword::SDK::AWillie_BP_C*)addrs.playerAddress;
        }
        
        gameState_.UpdateAddresses(addrs);
        gameState_.SetStatus("OK");
        return true;
    }
    
    uintptr_t ResolvePlayerAddress(uintptr_t gameInstance) const {
        
        HANDLE process = processManager_.GetProcess();
        MemoryReader<uintptr_t> ptrReader(process);
        MemoryReader<int32_t> intReader(process);
        
        uintptr_t localPlayersArray = gameInstance + Offsets::LocalPlayers;
        int32_t localPlayerCount = intReader.Read(localPlayersArray + Offsets::LocalPlayersCount);
        
        if (localPlayerCount <= 0) return 0;
        
        uintptr_t localPlayerData = ptrReader.Read(localPlayersArray + Offsets::LocalPlayerData);
        if (localPlayerData == 0) return 0;
        
        uintptr_t firstLocalPlayer = ptrReader.Read(localPlayerData);
        if (firstLocalPlayer == 0) return 0;
        
        uintptr_t playerController = ptrReader.Read(firstLocalPlayer + Offsets::PlayerController);
        if (playerController == 0) return 0;
        
        uintptr_t pawnAddr = ptrReader.Read(playerController + Offsets::AcknowledgedPawn);
        if (pawnAddr == 0) return 0;

        uintptr_t charMove = ptrReader.Read(pawnAddr + offsetof(HalfSword::SDK::ACharacter, CharacterMovement));
        if (charMove == 0) return 0;

        return pawnAddr;
    }

private:
    ProcessManager& processManager_;
    GameState& gameState_;
};
