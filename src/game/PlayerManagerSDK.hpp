#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
#include "../sdk/PlayerSDK.hpp"
#include "../core/MemoryReader.hpp"
#include "../core/Logger.hpp"
#include "../core/Offsets.hpp"

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
        if (player_ == nullptr) {
            return false;
        }
        return MemoryReader<double>(process_, "PlayerManagerSDK").IsValid((uintptr_t)player_ + offsetof(AWillie_BP_C, Health));
    }
    
    template<typename T>
    void WriteProperty(size_t offset, const T& value, const char* propertyName = "unknown") {
        if (!process_ || !player_) {
            return;
        }
        
        uintptr_t address = (uintptr_t)player_ + offset;
        
        MemoryReader<T> reader(process_, "PlayerManagerSDK");
        if (!reader.IsValid(address)) {
            return;
        }
        
        reader.Write(address, value);
    }
    
    template<typename T>
    T ReadProperty(size_t offset) const {
        if (!process_ || !player_) return T{};
        
        MemoryReader<T> reader(process_);
        if (!reader.IsValid((uintptr_t)player_ + offset)) return T{};
        
        return reader.Read((uintptr_t)player_ + offset);
    }
    
    bool IsSafeToWrite() const {
        if (!process_ || !player_) return false;
        
        uintptr_t playerAddr = (uintptr_t)player_;
        
        
        MemoryReader<int32_t> intReader(process_, "PlayerManagerSDK");
        if (intReader.IsValid(playerAddr + Offsets::ObjFlags)) {
            int32_t flags = intReader.Read(playerAddr + Offsets::ObjFlags);
            if (flags & 0x18000) return false; 
        } else {
            return false;
        }
        
        
        MemoryReader<uint8_t> byteReader(process_, "PlayerManagerSDK");
        if (byteReader.IsValid(playerAddr + Offsets::Invulnerable)) {
            uint8_t val = byteReader.Read(playerAddr + Offsets::Invulnerable);
            if (val > 1) return false; 
        } else {
            return false;
        }
        
        return true;
    }

    void SetGodMode(bool enabled) {
        
        if (!IsSafeToWrite()) {
            LOG_WARNING("SetGodMode: Safety check failed - skipping write");
            return;
        }

        LOG_INFO("SetGodMode: Writing Invulnerable, offset=0x", std::hex, offsetof(AWillie_BP_C, Invulnerable), std::dec, ", value=", enabled, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, Invulnerable), enabled);
        if (enabled) {
            LOG_INFO("SetGodMode: Writing Head_Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Head_Health), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Head_Health), 100.0);
            LOG_INFO("SetGodMode: Writing Neck_Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Neck_Health), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Neck_Health), 100.0);
            LOG_INFO("SetGodMode: Writing Body_Upper_Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Body_Upper_Health), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Body_Upper_Health), 100.0);
            LOG_INFO("SetGodMode: Writing Body_Lower_Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Body_Lower_Health), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Body_Lower_Health), 100.0);
            LOG_INFO("SetGodMode: Writing Arm_R_Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Arm_R_Health), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Arm_R_Health), 100.0);
            LOG_INFO("SetGodMode: Writing Arm_L_Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Arm_L_Health), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Arm_L_Health), 100.0);
            LOG_INFO("SetGodMode: Writing Leg_R_Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Leg_R_Health), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Leg_R_Health), 100.0);
            LOG_INFO("SetGodMode: Writing Leg_L_Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Leg_L_Health), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Leg_L_Health), 100.0);
            LOG_INFO("SetGodMode: Writing Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Health), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Health), 100.0);
            
            LOG_INFO("SetGodMode: Writing Health_Threshold_For_Dismemberment, offset=0x", std::hex, offsetof(AWillie_BP_C, Health_Threshold_For_Dismemberment), std::dec, ", value=-999999.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Health_Threshold_For_Dismemberment), -999999.0);
            
            LOG_INFO("SetGodMode: Writing Arm_R_Broken, offset=0x", std::hex, offsetof(AWillie_BP_C, Arm_R_Broken), std::dec, ", value=false, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Arm_R_Broken), false);
            LOG_INFO("SetGodMode: Writing Arm_L_Broken, offset=0x", std::hex, offsetof(AWillie_BP_C, Arm_L_Broken), std::dec, ", value=false, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Arm_L_Broken), false);
            LOG_INFO("SetGodMode: Writing Leg_R_Broken, offset=0x", std::hex, offsetof(AWillie_BP_C, Leg_R_Broken), std::dec, ", value=false, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Leg_R_Broken), false);
            LOG_INFO("SetGodMode: Writing Leg_L_Broken, offset=0x", std::hex, offsetof(AWillie_BP_C, Leg_L_Broken), std::dec, ", value=false, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Leg_L_Broken), false);
            LOG_INFO("SetGodMode: Writing Back_Broken, offset=0x", std::hex, offsetof(AWillie_BP_C, Back_Broken), std::dec, ", value=false, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Back_Broken), false);
            LOG_INFO("SetGodMode: Writing Head_Broken, offset=0x", std::hex, offsetof(AWillie_BP_C, Head_Broken), std::dec, ", value=false, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Head_Broken), false);
        } else {
            LOG_INFO("SetGodMode: Writing Health_Threshold_For_Dismemberment (disable), offset=0x", std::hex, offsetof(AWillie_BP_C, Health_Threshold_For_Dismemberment), std::dec, ", value=0.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
            WriteProperty(offsetof(AWillie_BP_C, Health_Threshold_For_Dismemberment), 0.0);
        }
    }
    
    void SetInfiniteStamina() {
        LOG_INFO("SetInfiniteStamina: Writing Stamina, offset=0x", std::hex, offsetof(AWillie_BP_C, Stamina), std::dec, ", value=100.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, Stamina), 100.0);
    }

    void SetInfiniteKick(bool enabled) {
        LOG_INFO("SetInfiniteKick: Writing Kick_Cooldown, offset=0x", std::hex, offsetof(AWillie_BP_C, Kick_Cooldown), std::dec, ", value=", !enabled, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, Kick_Cooldown), !enabled);
    }
    
    void SetNoPain() {
        LOG_INFO("SetNoPain: Writing Pain, offset=0x", std::hex, offsetof(AWillie_BP_C, Pain), std::dec, ", value=0.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
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
        LOG_INFO("SetWalkSpeed: Writing Max_Walk_Speed, offset=0x", std::hex, offsetof(UCharacterMovementComponent, Max_Walk_Speed), std::dec, ", value=", realSpeed, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Walk_Speed), realSpeed);
    }
    
    void SetJumpPower(float power) {
        if (!process_ || !player_) return;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Jump_Z_Velocity))) return;
        
        LOG_INFO("SetJumpPower: Writing Jump_Z_Velocity, offset=0x", std::hex, offsetof(UCharacterMovementComponent, Jump_Z_Velocity), std::dec, ", value=", power, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Jump_Z_Velocity), power);
    }
    
    void SetGravityScale(float scale) {
        if (!process_ || !player_) return;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Gravity_Scale))) return;
        
        LOG_INFO("SetGravityScale: Writing Gravity_Scale, offset=0x", std::hex, offsetof(UCharacterMovementComponent, Gravity_Scale), std::dec, ", value=", scale, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Gravity_Scale), scale);
    }
    
    void SetMass(float mass) {
        if (!process_ || !player_) return;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Mass))) return;
        
        LOG_INFO("SetMass: Writing Mass, offset=0x", std::hex, offsetof(UCharacterMovementComponent, Mass), std::dec, ", value=", mass, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Mass), mass);
    }
    
    void SetGroundFriction(float friction) {
        if (!process_ || !player_) return;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Ground_Friction))) return;
        
        LOG_INFO("SetGroundFriction: Writing Ground_Friction, offset=0x", std::hex, offsetof(UCharacterMovementComponent, Ground_Friction), std::dec, ", value=", friction, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Ground_Friction), friction);
    }
    
    void SetMusclePower(double power) {
        LOG_INFO("SetMusclePower: Writing Muscle_Power, offset=0x", std::hex, offsetof(AWillie_BP_C, Muscle_Power), std::dec, ", value=", power, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, Muscle_Power), power);
    }
    
    void SetHealth(double health) {
        LOG_INFO("SetHealth: Writing Health, offset=0x", std::hex, offsetof(AWillie_BP_C, Health), std::dec, ", value=", health, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, Health), health);
    }
    
    void FullHeal() {
        SetHealth(100.0);
    }
    
    void RestoreStamina() {
        SetInfiniteStamina();
    }

    void SetPunchForce(double force) {
        LOG_INFO("SetPunchForce: Writing R_Fisting_Rate, offset=0x", std::hex, offsetof(AWillie_BP_C, R_Fisting_Rate), std::dec, ", value=", force, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, R_Fisting_Rate), force);
        LOG_INFO("SetPunchForce: Writing L_Fisting_Rate, offset=0x", std::hex, offsetof(AWillie_BP_C, L_Fisting_Rate), std::dec, ", value=", force, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, L_Fisting_Rate), force);
    }

    void SetKickForce(float force) {
        LOG_INFO("SetKickForce: Writing Kick_Rate_R, offset=0x", std::hex, offsetof(AWillie_BP_C, Kick_Rate_R), std::dec, ", value=", force, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, Kick_Rate_R), force);
        LOG_INFO("SetKickForce: Writing Kick_Rate_L, offset=0x", std::hex, offsetof(AWillie_BP_C, Kick_Rate_L), std::dec, ", value=", force, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, Kick_Rate_L), force);
    }

    void SetDamageMultiplier(double multiplier) {
        LOG_INFO("SetDamageMultiplier: Writing Damage_Rate_Additional, offset=0x", std::hex, offsetof(AWillie_BP_C, Damage_Rate_Additional), std::dec, ", value=", multiplier, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(offsetof(AWillie_BP_C, Damage_Rate_Additional), multiplier);
    }

private:
    HANDLE process_ = nullptr;
    AWillie_BP_C* player_ = nullptr;
};
