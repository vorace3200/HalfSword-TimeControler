#pragma once
#include <windows.h>
#include "../sdk/PlayerSDK.hpp"
#include "../core/MemoryReader.hpp"

using namespace HalfSword::SDK;

class GameModeManagerSDK {
public:
    void SetProcess(HANDLE process) { process_ = process; }
    void SetWorld(UWorld* world) { world_ = world; }
    
    ABP_HalfSwordGameMode_C* GetGameMode() {
        if (!process_ || !world_) return nullptr;
        
        if (!MemoryReader<uintptr_t>(process_).IsValid(reinterpret_cast<uintptr_t>(world_))) return nullptr;
        
        uintptr_t gmPtr = MemoryReader<uintptr_t>(process_).Read(
            reinterpret_cast<uintptr_t>(world_) + offsetof(UWorld, AuthorityGameMode));
            
        if (!gmPtr) return nullptr;
        
        if (!MemoryReader<int32_t>(process_).IsValid(gmPtr + offsetof(ABP_HalfSwordGameMode_C, Ranks_Unlocked))) return nullptr;
            
        return reinterpret_cast<ABP_HalfSwordGameMode_C*>(gmPtr);
    }
    
    void UnlockAllWeapons() {
        ABP_HalfSwordGameMode_C* gm = GetGameMode();
        if (!gm) return;
        
        uintptr_t gmAddr = reinterpret_cast<uintptr_t>(gm);
        
        MemoryReader<int32_t>(process_).Write(gmAddr + offsetof(ABP_HalfSwordGameMode_C, Ranks_Unlocked), 100);
        MemoryReader<int32_t>(process_).Write(gmAddr + offsetof(ABP_HalfSwordGameMode_C, Player_Rank), 100);
        MemoryReader<int32_t>(process_).Write(gmAddr + offsetof(ABP_HalfSwordGameMode_C, Current_Points), 999999);
        MemoryReader<bool>(process_).Write(gmAddr + offsetof(ABP_HalfSwordGameMode_C, Free_Armor), true);
    }
    
    void InstantWin() {
        ABP_HalfSwordGameMode_C* gm = GetGameMode();
        if (!gm) return;
        
        MemoryReader<bool>(process_).Write(
            reinterpret_cast<uintptr_t>(gm) + offsetof(ABP_HalfSwordGameMode_C, All_Enemies_Dead), true);
    }

private:
    HANDLE process_ = nullptr;
    UWorld* world_ = nullptr;
};
