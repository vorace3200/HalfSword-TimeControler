#pragma once
#include <windows.h>
#include <string>

class HotkeyManager {
public:
    HotkeyManager() = default;
    
    void SetWindow(HWND) {
    }
    
    void SetKey(int key) {
        keybind_ = key;
        lastKeyState_ = false;
        triggered_ = false;
    }
    
    int GetKey() const {
        return keybind_;
    }
    
    void RegisterKey() {
    }
    
    void UnregisterKey() {
    }
    
    void Update() {

        bool keyDown = (GetAsyncKeyState(keybind_) & 0x8000) != 0;
        
        if (keyDown && !lastKeyState_) {
            triggered_ = true;
        }
        lastKeyState_ = keyDown;
    }
    
    bool IsKeyDown() const {
        return (GetAsyncKeyState(keybind_) & 0x8000) != 0;
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
        return key == VK_LBUTTON || key == VK_RBUTTON || key == VK_MBUTTON || 
               key == VK_XBUTTON1 || key == VK_XBUTTON2;
    }
    
    static std::string GetKeyName(int key) {
        if (key == 0) return "None";
        
        switch (key) {
            case VK_LBUTTON: return "Left Mouse";
            case VK_RBUTTON: return "Right Mouse";
            case VK_MBUTTON: return "Middle Mouse";
            case VK_XBUTTON1: return "Mouse 4";
            case VK_XBUTTON2: return "Mouse 5";
            case VK_TAB: return "Tab";
            case VK_RETURN: return "Enter";
            case VK_ESCAPE: return "Esc";
            case VK_SPACE: return "Space";
            case VK_PRIOR: return "Page Up";
            case VK_NEXT: return "Page Down";
            case VK_END: return "End";
            case VK_HOME: return "Home";
            case VK_LEFT: return "Left";
            case VK_UP: return "Up";
            case VK_RIGHT: return "Right";
            case VK_DOWN: return "Down";
            case VK_INSERT: return "Insert";
            case VK_DELETE: return "Delete";
            case VK_MULTIPLY: return "Numpad *";
            case VK_ADD: return "Numpad +";
            case VK_SEPARATOR: return "Separator";
            case VK_SUBTRACT: return "Numpad -";
            case VK_DECIMAL: return "Numpad .";
            case VK_DIVIDE: return "Numpad /";
            case VK_CAPITAL: return "Caps Lock";
        }
        
        if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z')) {
            return std::string(1, (char)key);
        }
        
        unsigned int scanCode = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
        
 
        if (scanCode != 0) {
            char buf[128];
            if (GetKeyNameTextA(scanCode << 16, buf, 128) > 0) {
                return std::string(buf);
            }
        }
        
        return "Key " + std::to_string(key);
    }

private:
    int keybind_ = VK_CAPITAL;
    bool lastKeyState_ = false;
    bool triggered_ = false;
};
