#pragma once
#include <windows.h>
#include <string>
#include <TlHelp32.h>

class ProcessManager {
public:
    ProcessManager() = default;
    ~ProcessManager() { CloseProcess(); }

    bool Attach(const std::string& processName) {
        processName_ = processName;
        DWORD pid = FindProcessId(processName);
        if (pid == 0) return false;

        process_ = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (!process_) return false;

        baseAddress_ = FindModuleBase(pid, processName);
        return baseAddress_ != 0;
    }

    void CloseProcess() {
        if (process_) {
            CloseHandle(process_);
            process_ = nullptr;
        }
    }

    bool IsAttached() const { return process_ != nullptr; }
    HANDLE GetProcess() const { return process_; }
    uintptr_t GetBaseAddress() const { return baseAddress_; }
    const std::string& GetProcessName() const { return processName_; }

private:
    static DWORD FindProcessId(const std::string& processName) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) return 0;

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        DWORD pid = 0;

        if (Process32First(snapshot, &pe)) {
            do {
                if (_stricmp(pe.szExeFile, processName.c_str()) == 0) {
                    pid = pe.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &pe));
        }

        CloseHandle(snapshot);
        return pid;
    }

    static uintptr_t FindModuleBase(DWORD processId, const std::string& moduleName) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
        if (snapshot == INVALID_HANDLE_VALUE) return 0;

        MODULEENTRY32 me;
        me.dwSize = sizeof(MODULEENTRY32);
        uintptr_t base = 0;

        if (Module32First(snapshot, &me)) {
            do {
                if (_stricmp(me.szModule, moduleName.c_str()) == 0) {
                    base = reinterpret_cast<uintptr_t>(me.modBaseAddr);
                    break;
                }
            } while (Module32Next(snapshot, &me));
        }

        CloseHandle(snapshot);
        return base;
    }

    std::string processName_;
    HANDLE process_ = nullptr;
    uintptr_t baseAddress_ = 0;
};
