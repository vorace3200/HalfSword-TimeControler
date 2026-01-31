#pragma once
#include <windows.h>
#include <cstdint>
#include <cmath>
#include <cfloat>
#include <string>
#include <sstream>
#include "../sdk/PlayerSDK.hpp"
#include "../core/MemoryReader.hpp"

using namespace HalfSword::SDK;

class EnemyManagerSDK {
public:
    EnemyManagerSDK() = default;
    
    void SetProcess(HANDLE process) {
        process_ = process;
    }
    
    void SetWorld(UWorld* world) {
        world_ = world;
    }
    
    void SetPlayer(AWillie_BP_C* player) {
        player_ = player;
    }
    
    bool HasWorld() const {
        return world_ != nullptr;
    }
    
    std::string GetDebugScan() {
        if (!process_ || !world_ || !player_) return "Not initialized";
        
        uintptr_t worldAddr = reinterpret_cast<uintptr_t>(world_);
        uintptr_t levelsArrayAddr = worldAddr + offsetof(UWorld, Levels);
        
        int32_t levelCount = MemoryReader<int32_t>(process_).Read(levelsArrayAddr + 0x8);
        uintptr_t levelsData = MemoryReader<uintptr_t>(process_).Read(levelsArrayAddr);
        
        std::stringstream ss;
        ss << "Total Levels: " << levelCount << "\n\n";
        
        int totalActors = 0;
        int totalEnemies = 0;
        
        for (int levelIdx = 0; levelIdx < levelCount && levelIdx < 10; levelIdx++) {
            uintptr_t levelPtr = MemoryReader<uintptr_t>(process_).Read(levelsData + (levelIdx * 8));
            if (!levelPtr) continue;
            
            uintptr_t actorsArrayAddr = levelPtr + offsetof(ULevel, Actors);
            int32_t actorCount = MemoryReader<int32_t>(process_).Read(actorsArrayAddr + 0x8);
            uintptr_t actorsData = MemoryReader<uintptr_t>(process_).Read(actorsArrayAddr);
            
            ss << "Level " << levelIdx << ": " << actorCount << " actors\n";
            totalActors += actorCount;
            
            int enemiesInLevel = 0;
            for (int i = 0; i < actorCount && i < 50; i++) {
                uintptr_t actorAddr = MemoryReader<uintptr_t>(process_).Read(actorsData + (i * 8));
                if (!actorAddr) continue;
                if (actorAddr == reinterpret_cast<uintptr_t>(player_)) continue;
                
                if (IsValidEnemy(reinterpret_cast<AActor*>(actorAddr))) {
                    enemiesInLevel++;
                    totalEnemies++;
                }
            }
            ss << "  -> " << enemiesInLevel << " enemies found\n";
        }
        
        ss << "\nTotal: " << totalActors << " actors, " << totalEnemies << " enemies";
        return ss.str();
    }
    
    AWillie_BP_C* FindClosestEnemy() {
        if (!process_ || !world_ || !player_) return nullptr;
        
        FVector playerPos = GetActorLocation(player_);
        AWillie_BP_C* closestEnemy = nullptr;
        float closestDist = FLT_MAX;
        
        uintptr_t worldAddr = reinterpret_cast<uintptr_t>(world_);
        uintptr_t levelsArrayAddr = worldAddr + offsetof(UWorld, Levels);
        
        int32_t levelCount = MemoryReader<int32_t>(process_).Read(levelsArrayAddr + 0x8);
        uintptr_t levelsData = MemoryReader<uintptr_t>(process_).Read(levelsArrayAddr);
        
        for (int levelIdx = 0; levelIdx < levelCount; levelIdx++) {
            uintptr_t levelPtr = MemoryReader<uintptr_t>(process_).Read(levelsData + (levelIdx * 8));
            if (!levelPtr) continue;
            
            uintptr_t actorsArrayAddr = levelPtr + offsetof(ULevel, Actors);
            int32_t actorCount = MemoryReader<int32_t>(process_).Read(actorsArrayAddr + 0x8);
            uintptr_t actorsData = MemoryReader<uintptr_t>(process_).Read(actorsArrayAddr);
            
            if (actorCount > 10000) actorCount = 10000;

            for (int i = 0; i < actorCount; i++) {
                uintptr_t actorAddr = MemoryReader<uintptr_t>(process_).Read(actorsData + (i * 8));
                AActor* actor = reinterpret_cast<AActor*>(actorAddr);
                
                if (!actor || actor == reinterpret_cast<AActor*>(player_)) continue;
                
                if (IsValidEnemy(actor)) {
                    FVector enemyPos = GetActorLocation(actor);
                    float dist = Distance(playerPos, enemyPos);
                    
                    if (dist < closestDist) {
                        closestDist = dist;
                        closestEnemy = reinterpret_cast<AWillie_BP_C*>(actor);
                    }
                }
            }
        }
        
        return closestEnemy;
    }
    
    void HealEnemy(AWillie_BP_C* target, float amount) {
        if (target) {
            MemoryReader<double>(process_).Write(
                reinterpret_cast<uintptr_t>(target) + offsetof(AWillie_BP_C, Health), 
                amount);
        } else {
            AWillie_BP_C* closest = FindClosestEnemy();
            if (closest) HealEnemy(closest, amount);
        }
    }
    void HealEnemy(float amount) { HealEnemy(nullptr, amount); }
    
    void DamageEnemy(AWillie_BP_C* target, float amount) {
        if (target) {
            uintptr_t addr = reinterpret_cast<uintptr_t>(target);
            double current = MemoryReader<double>(process_).Read(addr + offsetof(AWillie_BP_C, Health));
            MemoryReader<double>(process_).Write(addr + offsetof(AWillie_BP_C, Health), current - amount);
        } else {
            AWillie_BP_C* closest = FindClosestEnemy();
            if (closest) DamageEnemy(closest, amount);
        }
    }
    void DamageEnemy(float amount) { DamageEnemy(nullptr, amount); }
    
    void BreakLimb(AWillie_BP_C* target, size_t offset) {
        if (target) {
            MemoryReader<bool>(process_).Write(
                reinterpret_cast<uintptr_t>(target) + offset, true);
        } else {
            AWillie_BP_C* closest = FindClosestEnemy();
            if (closest) BreakLimb(closest, offset);
        }
    }
    void BreakLimb(size_t offset) { BreakLimb(nullptr, offset); }

    void BreakRightArm(AWillie_BP_C* target = nullptr) { BreakLimb(target, offsetof(AWillie_BP_C, Arm_R_Broken)); }
    void BreakLeftArm(AWillie_BP_C* target = nullptr) { BreakLimb(target, offsetof(AWillie_BP_C, Arm_L_Broken)); }
    void BreakRightLeg(AWillie_BP_C* target = nullptr) { BreakLimb(target, offsetof(AWillie_BP_C, Leg_R_Broken)); }
    void BreakLeftLeg(AWillie_BP_C* target = nullptr) { BreakLimb(target, offsetof(AWillie_BP_C, Leg_L_Broken)); }
    void BreakNeck(AWillie_BP_C* target = nullptr) { BreakLimb(target, offsetof(AWillie_BP_C, Neck_Health)); }
    
    void BreakBack(AWillie_BP_C* target = nullptr) { BreakLimb(target, offsetof(AWillie_BP_C, Back_Broken)); }

    int32_t GetActorCount() const {
        if (!process_ || !world_) return 0;
        
        uintptr_t worldAddr = reinterpret_cast<uintptr_t>(world_);
        uintptr_t levelsArrayAddr = worldAddr + offsetof(UWorld, Levels);
        int32_t levelCount = MemoryReader<int32_t>(process_).Read(levelsArrayAddr + 0x8);
        uintptr_t levelsData = MemoryReader<uintptr_t>(process_).Read(levelsArrayAddr);
        
        int total = 0;
        for (int i = 0; i < levelCount; i++) {
            uintptr_t levelPtr = MemoryReader<uintptr_t>(process_).Read(levelsData + (i * 8));
            if (!levelPtr) continue;
            total += MemoryReader<int32_t>(process_).Read(levelPtr + offsetof(ULevel, Actors) + 0x8);
        }
        return total;
    }

    float GetClosestEnemyDistance() {
        AWillie_BP_C* enemy = FindClosestEnemy();
        if (!enemy) return -1.0f;
        
        FVector playerPos = GetActorLocation(player_);
        FVector enemyPos = GetActorLocation(enemy);
        return Distance(playerPos, enemyPos);
    }

private:
    FVector GetActorLocation(AActor* actor) {
        if (!actor) return FVector{};
        
        uintptr_t actorAddr = reinterpret_cast<uintptr_t>(actor);
        uintptr_t rootComp = MemoryReader<uintptr_t>(process_).Read(
            actorAddr + offsetof(AActor, RootComponent));
        
        if (!rootComp) return FVector{};
        
        return MemoryReader<FVector>(process_).Read(
            rootComp + offsetof(USceneComponent, RelativeLocation));
    }
    
    bool IsValidEnemy(AActor* actor) {
        uintptr_t actorAddr = reinterpret_cast<uintptr_t>(actor);
        
        uintptr_t mesh = MemoryReader<uintptr_t>(process_).Read(
            actorAddr + offsetof(ACharacter, Mesh));
        if (mesh == 0) return false;

        double health = MemoryReader<double>(process_).Read(
            actorAddr + offsetof(AWillie_BP_C, Health));
        if (health <= 0.0) return false;
        
        int32_t playerTeam = MemoryReader<int32_t>(process_).Read(
            reinterpret_cast<uintptr_t>(player_) + offsetof(AWillie_BP_C, Team_Int));
        int32_t actorTeam = MemoryReader<int32_t>(process_).Read(
            actorAddr + offsetof(AWillie_BP_C, Team_Int));
        
        return actorTeam != playerTeam;
    }
    
    float Distance(const FVector& a, const FVector& b) {
        float dx = a.X - b.X;
        float dy = a.Y - b.Y;
        float dz = a.Z - b.Z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }
    
    HANDLE process_ = nullptr;
    UWorld* world_ = nullptr;
    AWillie_BP_C* player_ = nullptr;
};
