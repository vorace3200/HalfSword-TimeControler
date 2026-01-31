#pragma once
#include <cstdint>
#include <string>
#include "../sdk/PlayerSDK.hpp"

struct GameAddresses {
    uintptr_t uWorld = 0;
    uintptr_t persistentLevel = 0;
    uintptr_t worldSettings = 0;
    uintptr_t timeDilation = 0;
    uintptr_t playerAddress = 0;
    uintptr_t gameInstance = 0;
    HalfSword::SDK::AWillie_BP_C* player = nullptr;
    
    bool IsValid() const {
        return uWorld != 0 && persistentLevel != 0 && worldSettings != 0 && 
               timeDilation != 0 && gameInstance != 0;
    }
    
    bool HasPlayer() const { return player != nullptr; }
};

class GameState {
public:
    void UpdateAddresses(const GameAddresses& addrs) { addresses_ = addrs; }
    const GameAddresses& GetAddresses() const { return addresses_; }
    
    void SetStatus(const std::string& status) { status_ = status; }
    const std::string& GetStatus() const { return status_; }
    
    void SetTimeDilation(float value) { currentTimeDilation_ = value; }
    float GetTimeDilation() const { return currentTimeDilation_; }
    
    void SetRunning(bool running) { isRunning_ = running; }
    bool IsRunning() const { return isRunning_; }

private:
    GameAddresses addresses_;
    std::string status_ = "OK";
    float currentTimeDilation_ = 1.0f;
    bool isRunning_ = true;
};
