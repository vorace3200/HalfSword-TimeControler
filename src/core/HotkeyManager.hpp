#pragma once
#include <windows.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include "../core/Logger.hpp"

class HotkeyManager {
public:
    HotkeyManager() = default;
    
    void SetWindow(HWND hwnd) {
        hwnd_ = hwnd;
    }
    
    void SetKey(int key) {
        keybind_ = key;
    }
    
    int GetKey() const {
        return keybind_;
    }
    
    void StartListening() {
        isListening_ = true;
    }

    void StopListening() {
        isListening_ = false;
    }

    bool IsListening() const {
        return isListening_;
    }

    void RegisterRawInput() {
        if (!hwnd_) return;
        
        RAWINPUTDEVICE rid[2];
        
        rid[0].usUsagePage = 0x01; 
        rid[0].usUsage = 0x06;     
        rid[0].dwFlags = RIDEV_INPUTSINK; 
        rid[0].hwndTarget = hwnd_;

        // added to you mouse button (like the mouse 5 / 4) hope that not trigger windows
        rid[1].usUsagePage = 0x01; 
        rid[1].usUsage = 0x02;     
        rid[1].dwFlags = RIDEV_INPUTSINK; 
        rid[1].hwndTarget = hwnd_;

        if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0]))) {
            LOG_ERROR("HotkeyManager", "Failed to register Raw Input. Error: ", GetLastError());
        } else {
            LOG_INFO("HotkeyManager", "Registered Raw Input (Keyboard & Mouse) successfully");
        }
    }
    
    void UnregisterKey() {
    }
    
    void Update() {
    }
    
    bool IsKeyDown() const {
        return isKeyDown_;
    }
    
    void ProcessRawInput(LPARAM lParam) {
        UINT dwSize = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        
        if (dwSize == 0) return;
        
        std::vector<BYTE> lpb(dwSize);
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
             return;

        RAWINPUT* raw = (RAWINPUT*)lpb.data();

        if (raw->header.dwType == RIM_TYPEKEYBOARD) {
            bool isDown = (raw->data.keyboard.Flags & RI_KEY_BREAK) == 0;
            
            if (isListening_) {
                if (isDown && raw->data.keyboard.VKey != 0) {
                     keybind_ = raw->data.keyboard.VKey;
                     isListening_ = false;
                     LOG_INFO("HotkeyManager", "New key bound (Keyboard): ", keybind_);
                }
                return;
            }

            if (raw->data.keyboard.VKey == keybind_) {
                UpdateKeyState(isDown);
            }
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE) {
            int pressedKey = 0;
            bool isDown = false;

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) { pressedKey = VK_LBUTTON; isDown = true; }
            else if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) { pressedKey = VK_LBUTTON; isDown = false; }
            else if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) { pressedKey = VK_RBUTTON; isDown = true; }
            else if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) { pressedKey = VK_RBUTTON; isDown = false; }
            else if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) { pressedKey = VK_MBUTTON; isDown = true; }
            else if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) { pressedKey = VK_MBUTTON; isDown = false; }
            else if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) { pressedKey = VK_XBUTTON1; isDown = true; }
            else if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) { pressedKey = VK_XBUTTON1; isDown = false; }
            else if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) { pressedKey = VK_XBUTTON2; isDown = true; }
            else if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) { pressedKey = VK_XBUTTON2; isDown = false; }

            if (pressedKey != 0) {
                if (isListening_) {
                    if (isDown) {
                        keybind_ = pressedKey;
                        isListening_ = false;
                        LOG_INFO("HotkeyManager", "New key bound (Mouse): ", keybind_);
                    }
                    return;
                }

                if (pressedKey == keybind_) {
                    UpdateKeyState(isDown);
                }
            }
        }
    }

    void UpdateKeyState(bool isDown) {
        if (isDown != isKeyDown_) {
            isKeyDown_ = isDown;
            if (isDown) {
                triggered_ = true;
            }
        }
    }
    
    bool IsTriggered() {
        bool result = triggered_;
        triggered_ = false;
        return result;
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
    HWND hwnd_ = nullptr;
    int keybind_ = VK_CAPITAL;
    bool triggered_ = false;
    bool isKeyDown_ = false;
    bool isListening_ = false;
};
