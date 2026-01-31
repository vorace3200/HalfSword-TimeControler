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
#include "../core/Logger.hpp"

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
        
        MemoryReader<uintptr_t> ptrReader(process, "AddressResolver");
        MemoryReader<int32_t> intReader(process, "AddressResolver");
        
        GameAddresses addrs;
        
        
        addrs.uWorld = ptrReader.Read(base + Offsets::GWorld);
        if (addrs.uWorld == 0) {
            
            gameState_.SetStatus("In menu");
            gameState_.UpdateAddresses(addrs);
            return false;
        }
        
        
        
        
        int32_t levelsCount = intReader.Read(addrs.uWorld + 0x180);
        if (levelsCount < 2) {
            gameState_.SetStatus("In menu");
            gameState_.UpdateAddresses(addrs);
            return false;
        }
        
        addrs.persistentLevel = ptrReader.Read(addrs.uWorld + Offsets::PersistentLevel);
        if (addrs.persistentLevel == 0) {
            gameState_.SetStatus("Unable to read PersistentLevel");
            gameState_.UpdateAddresses(addrs);
            return false;
        }
        
        addrs.worldSettings = ptrReader.Read(addrs.persistentLevel + Offsets::WorldSettings);
        if (addrs.worldSettings == 0) {
            gameState_.SetStatus("Unable to read WorldSettings");
            gameState_.UpdateAddresses(addrs);
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
        MemoryReader<uintptr_t> ptrReader(process, "AddressResolver");
        MemoryReader<int32_t> intReader(process);
        
        uintptr_t localPlayersArray = gameInstance + Offsets::LocalPlayers;
        int32_t localPlayerCount = intReader.Read(localPlayersArray + Offsets::LocalPlayersCount);
        
        if (localPlayerCount <= 0) {
            return 0;
        }
        
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
        
        
        MemoryReader<float> speedReader(process);
        if (!speedReader.IsValid(charMove + Offsets::WalkSpeed)) {
            return 0;
        }
        
        
        MemoryReader<double> healthReader(process);
        if (!healthReader.IsValid(pawnAddr + Offsets::HealthMain)) {
            return 0;
        }
        double health = healthReader.Read(pawnAddr + Offsets::HealthMain);
        if (health < 0 || health > 1000) {
            return 0;
        }
        
        
        int32_t flags = intReader.Read(pawnAddr + Offsets::ObjFlags);
        
        if (flags & 0x18000) { 
            return 0; 
        }
        
        
        
        MemoryReader<uint8_t> byteReader(process);
        if (byteReader.IsValid(pawnAddr + Offsets::Invulnerable)) {
            uint8_t invuln = byteReader.Read(pawnAddr + Offsets::Invulnerable);
            if (invuln > 1) { 
                return 0;
            }
        } else {
            return 0; 
        }

        return pawnAddr;
    }

private:
    ProcessManager& processManager_;
    GameState& gameState_;
};
