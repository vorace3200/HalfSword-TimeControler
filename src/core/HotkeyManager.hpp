#pragma once
#include <windows.h>

class HotkeyManager {
public:
    HotkeyManager() = default;
    
    void SetWindow(HWND window) {
        window_ = window;
    }
    
    void SetKey(int key) {
        UnregisterKey();
        keybind_ = key;
        RegisterKey();
    }
    
    int GetKey() const {
        return keybind_;
    }
    
    void RegisterKey() {
        if (IsMouseButton(keybind_)) return;
        if (window_) {
            RegisterHotKey(window_, hotkeyId_, modifiers_, keybind_);
        }
    }
    
    void UnregisterKey() {
        if (IsMouseButton(keybind_)) return;
        if (window_) {
            UnregisterHotKey(window_, hotkeyId_);
        }
    }
    
    void Update() {
        if (IsMouseButton(keybind_)) {
            keyDown_ = (GetAsyncKeyState(keybind_) & 0x8000) != 0;
            if (keyDown_ && !lastKeyState_) {
                triggered_ = true;
            }
            lastKeyState_ = keyDown_;
        } else {
            keyDown_ = triggered_;
            if (triggered_) {
                triggered_ = false;
            }
        }
    }
    
    bool IsKeyDown() const {
        return keyDown_;
    }
    
    void OnHotkeyReceived() {
        triggered_ = true;
    }
    
    bool IsTriggered() {
        bool result = triggered_;
        triggered_ = false;
        return result;
    }
    
    static bool IsMouseButton(int key) {
        return key == VK_XBUTTON1 || key == VK_XBUTTON2;
    }
    
    static const char* GetKeyName(int key) {
        switch (key) {
            case VK_CAPITAL: return "Caps Lock";
            case VK_TAB: return "Tab";
            case VK_SPACE: return "Space";
            case VK_INSERT: return "Insert";
            case VK_DELETE: return "Delete";
            case VK_HOME: return "Home";
            case VK_END: return "End";
            case VK_PRIOR: return "Page Up";
            case VK_NEXT: return "Page Down";
            case VK_XBUTTON1: return "Mouse 4 (Side)";
            case VK_XBUTTON2: return "Mouse 5 (Side)";
            default: return "Custom Key";
        }
    }
    
    static const int* GetValidKeys(size_t& count) {
        static const int keys[] = {
            VK_CAPITAL, VK_TAB, VK_SPACE, VK_INSERT, VK_DELETE,
            VK_HOME, VK_END, VK_PRIOR, VK_NEXT, VK_XBUTTON1, VK_XBUTTON2
        };
        count = sizeof(keys) / sizeof(keys[0]);
        return keys;
    }

private:
    HWND window_ = nullptr;
    int keybind_ = VK_CAPITAL;
    int hotkeyId_ = 1;
    UINT modifiers_ = 0;
    bool lastKeyState_ = false;
    bool triggered_ = false;
    bool keyDown_ = false;
};
