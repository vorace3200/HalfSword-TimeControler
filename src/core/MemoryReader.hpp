#pragma once
#include <windows.h>
#include <cstdint>
#include "Logger.hpp"

template<typename T>
class MemoryReader {
public:
    MemoryReader(HANDLE process, const char* component = "MemoryReader") : process_(process), component_(component) {}

    T Read(uintptr_t address) const {
        T value{};
        SIZE_T bytesRead;
        ReadProcessMemory(process_, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead);
        return value;
    }

    bool IsValid(uintptr_t address) const {
        T value{};
        SIZE_T bytesRead;
        return ReadProcessMemory(process_, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead) && bytesRead == sizeof(T);
    }

    bool Write(uintptr_t address, const T& value) const {
        SIZE_T bytesWritten;
        DWORD oldProtect;
        
        if (!VirtualProtectEx(process_, reinterpret_cast<LPVOID>(address), sizeof(T), PAGE_READWRITE, &oldProtect))
            return false;

        bool success = WriteProcessMemory(process_, reinterpret_cast<LPVOID>(address), &value, sizeof(T), &bytesWritten);
        
        DWORD temp;
        VirtualProtectEx(process_, reinterpret_cast<LPVOID>(address), sizeof(T), oldProtect, &temp);
        
        return success && bytesWritten == sizeof(T);
    }

private:
    HANDLE process_;
    const char* component_;
};
