#pragma once
#include <windows.h>
#include <d3d9.h>
#include <string>
#include <memory>
#include "core/ProcessManager.hpp"
#include "core/HotkeyManager.hpp"
#include "game/GameState.hpp"
#include "game/PlayerManagerSDK.hpp"
#include "game/TimeController.hpp"
#include "game/EnemyManagerSDK.hpp"
#include "game/GameModeManagerSDK.hpp"
#include "game/AddressResolver.hpp"
#include "ui/UIManager.hpp"
#include "core/Logger.hpp"

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool Initialize(HWND window);
    void Shutdown();
    void Reset();
    void BeginFrame();
    void EndFrame();
    void Present();
    bool IsDeviceLost() const;
    
    LPDIRECT3DDEVICE9 GetDevice() const { return device_; }

private:
    bool CreateDevice(HWND window);
    void CleanupDevice();
    
    LPDIRECT3D9 d3d_ = nullptr;
    LPDIRECT3DDEVICE9 device_ = nullptr;
    D3DPRESENT_PARAMETERS params_ = {};
};

class Window {
public:
    Window();
    ~Window();
    
    bool Create(HINSTANCE instance, const std::string& title);
    void Destroy();
    void Show();
    void Hide();
    
    HWND GetHandle() const { return handle_; }
    HINSTANCE GetInstance() const { return instance_; }
    bool ProcessMessages();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    HWND handle_ = nullptr;
    HINSTANCE instance_ = nullptr;
    std::string className_;
};

class Application {
public:
    Application();
    ~Application();
    
    bool Initialize();
    void Run();
    void Shutdown();
    
    static Application& GetInstance() {
        static Application instance;
        return instance;
    }
    
    HotkeyManager* GetHotkeyManager() const { return hotkeyManager_.get(); }

private:
    void InitializeImGui();
    void ShutdownImGui();
    void Update();
    void Render();
    void HandleHotkey();
    
    std::unique_ptr<Window> window_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<ProcessManager> processManager_;
    std::unique_ptr<GameState> gameState_;
    std::unique_ptr<PlayerManagerSDK> playerManager_;
    std::unique_ptr<TimeController> timeController_;
    std::unique_ptr<EnemyManagerSDK> enemyManager_;
    std::unique_ptr<GameModeManagerSDK> gameModeManager_;
    std::unique_ptr<HotkeyManager> hotkeyManager_;
    std::unique_ptr<AddressResolver> addressResolver_;
    std::unique_ptr<UIManager> uiManager_;
    
    bool initialized_ = false;
    bool running_ = true;
    bool debugMode_ = false;
};
