#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

class Logger {
public:
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    void SetDebugEnabled(bool enabled) {
        if (enabled && !debugEnabled_) {
            AllocConsole();
            FILE* file = nullptr;
            freopen_s(&file, "CONOUT$", "w", stdout);
            freopen_s(&file, "CONOUT$", "w", stderr);
            std::cout << "=== Debug Console Opened ===" << std::endl;
        } else if (!enabled && debugEnabled_) {
            std::cout << "=== Debug Console Closing ===" << std::endl;
            FreeConsole();
        }
        debugEnabled_ = enabled;
    }

    bool IsDebugEnabled() const {
        return debugEnabled_;
    }

    template<typename... Args>
    void Log(const std::string& level, const std::string& component, Args&&... args) {
        if (!debugEnabled_) return;

        std::ostringstream oss;
        oss << "[" << GetTimestamp() << "] ";
        oss << "[" << std::setw(8) << std::left << level << "] ";
        oss << "[" << std::setw(15) << std::left << component << "] ";
        (oss << ... << args);
        
        std::cout << oss.str() << std::endl;
    }

    template<typename... Args>
    void Info(const std::string& component, Args&&... args) {
        Log("INFO", component, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void Debug(const std::string& component, Args&&... args) {
        Log("DEBUG", component, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void Warning(const std::string& component, Args&&... args) {
        Log("WARN", component, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void Error(const std::string& component, Args&&... args) {
        Log("ERROR", component, std::forward<Args>(args)...);
    }

    void LogAddress(const std::string& component, const std::string& name, uintptr_t address) {
        if (!debugEnabled_) return;
        std::ostringstream oss;
        oss << name << ": 0x" << std::hex << std::uppercase << address << std::dec;
        Debug(component, oss.str());
    }

    void LogMemoryRead(const std::string& component, uintptr_t address, size_t size) {
        
        (void)component;
        (void)address;
        (void)size;
    }

    void LogMemoryWrite(const std::string& component, uintptr_t address, size_t size) {
        
        (void)component;
        (void)address;
        (void)size;
    }

    void LogFunctionEntry(const std::string& component, const std::string& function) {
        
        (void)component;
        (void)function;
    }

    void LogFunctionExit(const std::string& component, const std::string& function) {
        
        (void)component;
        (void)function;
    }

private:
    Logger() = default;
    ~Logger() {
        if (debugEnabled_) {
            FreeConsole();
        }
    }

    std::string GetTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::tm localTime;
        localtime_s(&localTime, &time);
        
        std::ostringstream oss;
        oss << std::put_time(&localTime, "%H:%M:%S");
        oss << "." << std::setw(3) << std::setfill('0') << ms.count();
        return oss.str();
    }

    bool debugEnabled_ = false;
};

#define LOG_INFO(...) Logger::GetInstance().Info(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::GetInstance().Debug(__VA_ARGS__)
#define LOG_WARNING(...) Logger::GetInstance().Warning(__VA_ARGS__)
#define LOG_ERROR(...) Logger::GetInstance().Error(__VA_ARGS__)
#define LOG_ADDRESS(comp, name, addr) Logger::GetInstance().LogAddress(comp, name, addr)
#define LOG_MEM_READ(comp, addr, size) Logger::GetInstance().LogMemoryRead(comp, addr, size)
#define LOG_MEM_WRITE(comp, addr, size) Logger::GetInstance().LogMemoryWrite(comp, addr, size)
#define LOG_FUNC_ENTRY(comp, func) Logger::GetInstance().LogFunctionEntry(comp, func)
#define LOG_FUNC_EXIT(comp, func) Logger::GetInstance().LogFunctionExit(comp, func)
