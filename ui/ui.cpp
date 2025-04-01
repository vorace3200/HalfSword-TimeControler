#include "ui.hpp"
#include "../globals.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include <iostream>
#include <string>
#include <TlHelp32.h>

auto GetProcessIdByName(const std::string& processName) -> DWORD {
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (_stricmp(pe.szExeFile, processName.c_str()) == 0) {
                processId = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return processId;
}

template <typename T>
auto ReadMemory(HANDLE hProcess, uintptr_t address) -> T {
    T value{};
    SIZE_T bytesRead;
    if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead) || bytesRead != sizeof(T)) {
        return T{};
    }
    return value;
}

template <typename T>
auto WriteMemory(HANDLE hProcess, uintptr_t address, T value) -> bool {
    SIZE_T bytesWritten;
    DWORD oldProtect;

    if (!VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(address), sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    bool success = WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(address), &value, sizeof(T), &bytesWritten) && (bytesWritten == sizeof(T));

    DWORD temp;
    VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(address), sizeof(T), oldProtect, &temp);

    return success;
}

void ui::updateMemoryState() {
    std::string processName = "HalfSwordUE5-Win64-Shipping.exe";
    DWORD procId = GetProcessIdByName(processName);

    if (procId == 0) return;

    if (!globals.hProcess) {
        globals.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
        if (!globals.hProcess) return;
    }

    auto GetModuleBaseAddress = [&](DWORD processId, const std::string& moduleName) -> uintptr_t {
        uintptr_t baseAddress = 0;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
        if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

        MODULEENTRY32 me;
        me.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &me)) {
            do {
                if (_stricmp(me.szModule, moduleName.c_str()) == 0) {
                    baseAddress = reinterpret_cast<uintptr_t>(me.modBaseAddr);
                    break;
                }
            } while (Module32Next(hSnapshot, &me));
        }
        CloseHandle(hSnapshot);
        return baseAddress;
        };

    uintptr_t baseModuleAddress = GetModuleBaseAddress(procId, processName);
    if (baseModuleAddress == 0) {
        CloseHandle(globals.hProcess);
        globals.hProcess = nullptr;
        return;
    }

    // Offsets (i'll update the offsets if that change) 
    const uintptr_t uWorldOffset = 0x07DC0AF0;
    const uintptr_t persistentLevelOffset = 0x30;
    const uintptr_t worldSettingsOffset = 0x2A0;
    const uintptr_t timeDilationOffset = 0x3C0;

    globals.uWorldAddress = ReadMemory<uintptr_t>(globals.hProcess, baseModuleAddress + uWorldOffset);
    if (globals.uWorldAddress == 0) {
        globals.statusMessage = "Unable to read UWorld";
        return;
    }

    globals.persistentLevelAddress = ReadMemory<uintptr_t>(globals.hProcess, globals.uWorldAddress + persistentLevelOffset);
    if (globals.persistentLevelAddress == 0) {
        globals.statusMessage = "Unable to read PersistentLevel";
        return;
    }

    globals.worldSettingsAddress = ReadMemory<uintptr_t>(globals.hProcess, globals.persistentLevelAddress + worldSettingsOffset);
    if (globals.worldSettingsAddress == 0) {
        globals.statusMessage = "Unable to read WorldSettings";
        return;
    }

    globals.timeDilationAddress = globals.worldSettingsAddress + timeDilationOffset;
    globals.currentTimeDilation = ReadMemory<float>(globals.hProcess, globals.timeDilationAddress);

    globals.statusMessage = "OK";
}


void ui::render() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 300));
    ImGui::Begin("TimeControler - Verison 1.0.1", &globals.active, ImGuiWindowFlags_NoResize);

    if (globals.hProcess) {
        ImGui::Text("PersistentLevel Address: 0x%llx", globals.persistentLevelAddress);
        ImGui::Text("WorldSettings Address: 0x%llx", globals.worldSettingsAddress);
        ImGui::Text("TimeDilation Address: 0x%llx", globals.timeDilationAddress);
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Current TimeDilation: %.2f", globals.currentTimeDilation);
        ImGui::SliderFloat("Slow Time", &globals.slowTime, 0.1f, 2.0f, "%.2f");
		ImGui::Checkbox("Toggle Mode", &globals.toggleMode);

        ImGui::Separator();
        ImGui::Spacing();

        const char* keyName = globals.keybind == VK_CAPITAL ? "Caps Lock" : "Custom Key";
        ImGui::Text("Actual keybind: %s", keyName);


        if (ImGui::Button("Change Keybind")) globals.waitingForKey = true;

        if (globals.waitingForKey) {
            ImGui::Text("Press a key...");
            for (int key = 0x01; key < 0xFF; ++key) {
                if (GetAsyncKeyState(key) & 0x8000) {
                    globals.keybind = key;
                    globals.waitingForKey = false;
                    break;
                }
            }
        }
        else {
            bool keyActive = (GetAsyncKeyState(globals.keybind) & 0x8000) != 0;

            if (globals.toggleMode) {
                if (keyActive && !globals.lastKeyState) {
                    globals.timeActive = !globals.timeActive;
                }
                globals.lastKeyState = keyActive;
            }
            else {
                globals.timeActive = keyActive;
            }
            float desiredTime = globals.timeActive ? globals.slowTime : 1.0f;
            WriteMemory<float>(globals.hProcess, globals.timeDilationAddress, desiredTime);
            ImGui::Text("TimeControler active: %s", globals.timeActive ? "ON" : "OFF");
        }
    }
    else {
        ImGui::Text("Open halfsword");
    }

    ImGui::Spacing();
    ImGui::SetCursorPos(ImVec2(
        ImGui::GetWindowSize().x - ImGui::CalcTextSize("made by vorace32").x - 10,
        ImGui::GetWindowSize().y - ImGui::GetTextLineHeight() - 10
    ));
    ImGui::Text("made by vorace32");

    ImGui::End();
}
void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;
    ImGui::StyleColorsDark();
	if (window_pos.x == 0) {
		RECT screen_rect{};
		GetWindowRect(GetDesktopWindow(), &screen_rect);
		screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
		window_pos = (screen_res - window_size) * 0.5f;
	}
}