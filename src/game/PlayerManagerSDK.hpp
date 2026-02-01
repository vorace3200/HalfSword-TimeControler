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
        if (writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Walk_Speed))) {
             writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Walk_Speed), realSpeed);
        }
        if (writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Walk_Speed_Crouched))) {
             writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Walk_Speed_Crouched), realSpeed);
        }
        if (writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Fly_Speed))) {
             writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Fly_Speed), realSpeed);
        }
        if (writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Custom_Movement_Speed))) {
             writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Custom_Movement_Speed), realSpeed);
        }
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
    
    void SetSpeedLimit(bool enabled) {
        if (!process_ || !player_) return;
        
        uintptr_t charMovementPtr = MemoryReader<uintptr_t>(process_).Read((uintptr_t)player_ + offsetof(AWillie_BP_C, CharacterMovement));
        if (!charMovementPtr) return;
        
        MemoryReader<float> writer(process_);
        if (!writer.IsValid(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Acceleration))) return;
        
        float value = enabled ? 99999.0f : 2048.0f;
        LOG_INFO("SetSpeedLimit: Writing Max_Acceleration, offset=0x", std::hex, offsetof(UCharacterMovementComponent, Max_Acceleration), std::dec, ", value=", value, ", player=0x", std::hex, (uintptr_t)player_, std::dec);
        writer.Write(charMovementPtr + offsetof(UCharacterMovementComponent, Max_Acceleration), value);
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

    void TriggerGetUp() {
        LOG_INFO("TriggerGetUp: Writing Getup_Animation_State, offset=0x", std::hex, Offsets::GetupAnimationState, std::dec, ", value=1.0, player=0x", std::hex, (uintptr_t)player_, std::dec);
        WriteProperty(Offsets::GetupAnimationState, 1.0);
    }

    void DumpDebugInfo() const {
        if (!player_ || !process_) {
            LOG_WARNING("DumpDebugInfo: No player or process attached");
            return;
        }

        uintptr_t base = (uintptr_t)player_;
        LOG_INFO("=== PLAYER DEBUG DUMP ===");
        LOG_INFO("Player Address: 0x", std::hex, base, std::dec);

        MemoryReader<int32_t> intReader(process_, "DebugDump");
        MemoryReader<uintptr_t> ptrReader(process_, "DebugDump");
        MemoryReader<double> doubleReader(process_, "DebugDump");
        MemoryReader<uint8_t> byteReader(process_, "DebugDump");

        if (intReader.IsValid(base + Offsets::ObjFlags)) {
            int32_t flags = intReader.Read(base + Offsets::ObjFlags);
            LOG_INFO("Flags (0x8): 0x", std::hex, flags, std::dec);
        } else {
            LOG_ERROR("Flags (0x8): INVALID MEMORY");
        }

        if (intReader.IsValid(base + Offsets::ObjIndex)) {
            int32_t index = intReader.Read(base + Offsets::ObjIndex);
            LOG_INFO("Internal Index (0xC): ", index);
        } else {
            LOG_ERROR("Internal Index (0xC): INVALID MEMORY");
        }

        if (ptrReader.IsValid(base + 0x10)) {
            uintptr_t classPtr = ptrReader.Read(base + 0x10);
            LOG_INFO("Class Pointer (0x10): 0x", std::hex, classPtr, std::dec);
        } else {
            LOG_ERROR("Class Pointer (0x10): INVALID MEMORY");
        }

        if (ptrReader.IsValid(base + Offsets::PawnController)) {
            uintptr_t controller = ptrReader.Read(base + Offsets::PawnController);
            LOG_INFO("Controller (0x2C8): 0x", std::hex, controller, std::dec);
        } else {
            LOG_ERROR("Controller (0x2C8): INVALID MEMORY");
        }

        if (ptrReader.IsValid(base + offsetof(AWillie_BP_C, CharacterMovement))) {
            uintptr_t charMove = ptrReader.Read(base + offsetof(AWillie_BP_C, CharacterMovement));
            LOG_INFO("CharacterMovement (0x320): 0x", std::hex, charMove, std::dec);
            
            if (charMove) {
                 MemoryReader<float> fReader(process_, "DebugDump");
                 if (fReader.IsValid(charMove + offsetof(UCharacterMovementComponent, Max_Walk_Speed))) {
                     float speed = fReader.Read(charMove + offsetof(UCharacterMovementComponent, Max_Walk_Speed));
                     LOG_INFO("  > Max_Walk_Speed (0x248): ", speed);
                 }
                 if (fReader.IsValid(charMove + offsetof(UCharacterMovementComponent, Max_Acceleration))) {
                     float accel = fReader.Read(charMove + offsetof(UCharacterMovementComponent, Max_Acceleration));
                     LOG_INFO("  > Max_Acceleration (0x25C): ", accel);
                 }
            }
        } else {
            LOG_ERROR("CharacterMovement (0x320): INVALID MEMORY");
        }

        if (doubleReader.IsValid(base + Offsets::HealthMain)) {
            double health = doubleReader.Read(base + Offsets::HealthMain);
            LOG_INFO("Health (0x1318): ", health);
        } else {
            LOG_ERROR("Health (0x1318): INVALID MEMORY");
        }

        if (byteReader.IsValid(base + Offsets::Invulnerable)) {
            uint8_t invuln = byteReader.Read(base + Offsets::Invulnerable);
            LOG_INFO("Invulnerable (0x227A): ", (int)invuln);
        } else {
            LOG_ERROR("Invulnerable (0x227A): INVALID MEMORY");
        }
        
        LOG_INFO("=========================");
    }

private:
    HANDLE process_ = nullptr;
    AWillie_BP_C* player_ = nullptr;
};
