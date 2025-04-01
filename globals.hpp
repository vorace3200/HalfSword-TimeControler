#pragma once
#include <unordered_map>
#include <string>
class c_globals {
public:
    bool active = true;
    std::string statusMessage = "OK";
    HANDLE hProcess = NULL;
    uintptr_t uWorldAddress = 0;
    uintptr_t persistentLevelAddress = 0;
    uintptr_t worldSettingsAddress = 0;
    uintptr_t timeDilationAddress = 0;
    bool toggleMode = true;
    bool timeActive = false;
    bool lastKeyState = false;
    float currentTimeDilation = 1.0f;
    float slowTime = 0.5f;
    int keybind = VK_CAPITAL;
    bool waitingForKey = false;
};

inline c_globals globals;