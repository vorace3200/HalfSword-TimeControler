#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
#include "../sdk/PlayerSDK.hpp"
#include "../core/MemoryReader.hpp"

using namespace HalfSword::SDK;

class PlayerManagerSDK {
public:
    PlayerManagerSDK() = default;
    
    void SetProcess(HANDLE process) {
        process_ = process;
    }
    
    void SetPlayer(AWillie_BP_C* player) {
        player_ = player;
    }
    
    bool HasPlayer() const {
        if (player_ == nullptr) return false;
        return MemoryReader<double>(process_).IsValid((uintptr_t)player_ + offsetof(AWillie_BP_C, Health));
    }
    
    template<typename T>
    void WriteProperty(size_t offset, const T& value) {
        if (!process_ || !player_) return;
        MemoryReader<T> reader(process_);
        if (!reader.IsValid((uintptr_t)player_ + offset)) return;
        reader.Write((uintptr_t)player_ + offset, value);
    }
    
    template<typename T>
    T ReadProperty(size_t offset) const {
        if (!process_ || !player_) return T{};
        
        MemoryReader<T> reader(process_);
        if (!reader.IsValid((uintptr_t)player_ + offset)) return T{};
        
        return reader.Read((uintptr_t)player_ + offset);
    }
    
    void SetGodMode(bool enabled) {
        WriteProperty(offsetof(AWillie_BP_C, Invulnerable), enabled);
        if (enabled) {
            WriteProperty(offsetof(AWillie_BP_C, Head_Health), 100.0);
            WriteProperty(offsetof(AWillie_BP_C, Neck_Health), 100.0);
            WriteProperty(offsetof(AWillie_BP_C, Body_Upper_Health), 100.0);
            WriteProperty(offsetof(AWillie_BP_C, Body_Lower_Health), 100.0);
            WriteProperty(offsetof(AWillie_BP_C, Arm_R_Health), 100.0);
            WriteProperty(offsetof(AWillie_BP_C, Arm_L_Health), 100.0);
            WriteProperty(offsetof(AWillie_BP_C, Leg_R_Health), 100.0);
            WriteProperty(offsetof(AWillie_BP_C, Leg_L_Health), 100.0);
            WriteProperty(offsetof(AWillie_BP_C, Health), 100.0);
            
            WriteProperty(offsetof(AWillie_BP_C, Health_Threshold_For_Dismemberment), -999999.0);
            
            WriteProperty(offsetof(AWillie_BP_C, Arm_R_Broken), false);
            WriteProperty(offsetof(AWillie_BP_C, Arm_L_Broken), false);
            WriteProperty(offsetof(AWillie_BP_C, Leg_R_Broken), false);
            WriteProperty(offsetof(AWillie_BP_C, Leg_L_Broken), false);
            WriteProperty(offsetof(AWillie_BP_C, Back_Broken), false);
            WriteProperty(offsetof(AWillie_BP_C, Head_Broken), false);
        } else {
            WriteProperty(offsetof(AWillie_BP_C, Health_Threshold_For_Dismemberment), 0.0);
        }
    }
    
    void SetInfiniteStamina() {
        WriteProperty(offsetof(AWillie_BP_C, Stamina), 100.0);
    }

    void SetInfiniteKick(bool enabled) {
        WriteProperty(offsetof(AWillie_BP_C, Kick_Cooldown), !enabled);
    }
    
    void SetNoPain() {
        WriteProperty(offsetof(AWillie_BP_C, Pain), 0.0);
    }
    
    void SetMaxConsciousness() {
    }
    
    void SetWalkSpeed(float displaySpeed) {
        if (!process_ || !player_) return;
        
        float realSpeed = displaySpeed * 10.0f;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Walk_Speed))) return;
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Walk_Speed), realSpeed);
    }
    
    void SetJumpPower(float power) {
        if (!process_ || !player_) return;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Jump_Z_Velocity))) return;
        
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Jump_Z_Velocity), power);
    }
    
    void SetGravityScale(float scale) {
        if (!process_ || !player_) return;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Gravity_Scale))) return;
        
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Gravity_Scale), scale);
    }
    
    void SetMass(float mass) {
        if (!process_ || !player_) return;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Mass))) return;
        
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Mass), mass);
    }
    
    void SetGroundFriction(float friction) {
        if (!process_ || !player_) return;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Ground_Friction))) return;
        
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Ground_Friction), friction);
    }
    
    void SetMusclePower(double power) {
        WriteProperty(offsetof(AWillie_BP_C, Muscle_Power), power);
    }
    
    void SetHealth(double health) {
        WriteProperty(offsetof(AWillie_BP_C, Health), health);
    }
    
    void FullHeal() {
        SetHealth(100.0);
    }
    
    void RestoreStamina() {
        SetInfiniteStamina();
    }

    void SetPunchForce(double force) {
        WriteProperty(offsetof(AWillie_BP_C, R_Fisting_Rate), force);
        WriteProperty(offsetof(AWillie_BP_C, L_Fisting_Rate), force);
    }

    void SetKickForce(float force) {
        WriteProperty(offsetof(AWillie_BP_C, Kick_Rate_R), force);
        WriteProperty(offsetof(AWillie_BP_C, Kick_Rate_L), force);
    }

    void SetDamageMultiplier(double multiplier) {
        WriteProperty(offsetof(AWillie_BP_C, Damage_Rate_Additional), multiplier);
    }

private:
    HANDLE process_ = nullptr;
    AWillie_BP_C* player_ = nullptr;
};
