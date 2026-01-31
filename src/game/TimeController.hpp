#pragma once
#include <cstdint>
#include <windows.h>
#include "../core/MemoryReader.hpp"
#include "../core/Offsets.hpp"
#include "../core/Logger.hpp"

class TimeController {
public:
    TimeController() = default;
    
    void SetProcess(HANDLE process) {
        process_ = process;
    }
    
    void SetTimeDilationAddress(uintptr_t address) {
        timeDilationAddress_ = address;
    }
    
    float GetCurrentTimeDilation() const {
        if (!process_ || timeDilationAddress_ == 0) {
            return 1.0f;
        }
        
        MemoryReader<float> reader(process_, "TimeController");
        
        
        if (!reader.IsValid(timeDilationAddress_)) {
            return 1.0f;
        }
        
        float value = reader.Read(timeDilationAddress_);
        return value;
    }
    
    void SetTimeDilation(float value) {
        if (!process_ || timeDilationAddress_ == 0) {
            return;
        }
        
        MemoryReader<float> writer(process_, "TimeController");
        if (!writer.IsValid(timeDilationAddress_)) {
            timeDilationAddress_ = 0;
            return;
        }
        
        float current = writer.Read(timeDilationAddress_);
        
        if (current != value) {
            writer.Write(timeDilationAddress_, value);
        }
    }
    
    void Update(bool timeActive, float slowTime) {
        
        if (!process_ || timeDilationAddress_ == 0) {
            return;
        }
        
        
        MemoryReader<float> reader(process_, "TimeController");
        if (!reader.IsValid(timeDilationAddress_)) {
            timeDilationAddress_ = 0;
            return;
        }
        
        float desiredTime = timeActive ? slowTime : 1.0f;
        SetTimeDilation(desiredTime);
    }

private:
    HANDLE process_ = nullptr;
    uintptr_t timeDilationAddress_ = 0;
};
