#include "Application.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx9.h"
#include "core/Logger.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application* g_App = nullptr;

Renderer::Renderer() = default;

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize(HWND window) {
    return CreateDevice(window);
}

void Renderer::Shutdown() {
    CleanupDevice();
}

void Renderer::Reset() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = device_->Reset(&params_);
    if (hr == D3DERR_INVALIDCALL) {
        IM_ASSERT(0);
    }
    ImGui_ImplDX9_CreateDeviceObjects();
}

void Renderer::BeginFrame() {
    device_->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
    device_->BeginScene();
}

void Renderer::EndFrame() {
    device_->EndScene();
}

void Renderer::Present() {
    HRESULT result = device_->Present(nullptr, nullptr, nullptr, nullptr);
    if (result == D3DERR_DEVICELOST && device_->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
        Reset();
    }
}

bool Renderer::IsDeviceLost() const {
    return device_->TestCooperativeLevel() == D3DERR_DEVICELOST;
}

bool Renderer::CreateDevice(HWND window) {
    d3d_ = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d_) return false;

    ZeroMemory(&params_, sizeof(params_));
    params_.Windowed = TRUE;
    params_.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params_.BackBufferFormat = D3DFMT_UNKNOWN;
    params_.EnableAutoDepthStencil = TRUE;
    params_.AutoDepthStencilFormat = D3DFMT_D16;
    params_.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
                           D3DCREATE_HARDWARE_VERTEXPROCESSING, &params_, &device_) < 0) {
        return false;
    }

    return true;
}

void Renderer::CleanupDevice() {
    if (device_) {
        device_->Release();
        device_ = nullptr;
    }
    if (d3d_) {
        d3d_->Release();
        d3d_ = nullptr;
    }
}

Window::Window() = default;

Window::~Window() {
    Destroy();
}

bool Window::Create(HINSTANCE instance, const std::string& title) {
    instance_ = instance;
    className_ = title;

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance_;
    wc.lpszClassName = className_.c_str();

    if (!RegisterClassEx(&wc)) {
        return false;
    }

    handle_ = CreateWindow(wc.lpszClassName, title.c_str(), WS_POPUP, 
                           0, 0, 5, 5, nullptr, nullptr, wc.hInstance, nullptr);

    if (!handle_) {
        UnregisterClass(className_.c_str(), instance_);
        return false;
    }

    g_App = &Application::GetInstance();
    SetWindowLongPtr(handle_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    return true;
}

void Window::Destroy() {
    if (handle_) {
        DestroyWindow(handle_);
        handle_ = nullptr;
    }
    if (!className_.empty()) {
        UnregisterClass(className_.c_str(), instance_);
    }
}

void Window::Show() {
    ShowWindow(handle_, SW_HIDE);
    UpdateWindow(handle_);
}

void Window::Hide() {
    ShowWindow(handle_, SW_HIDE);
}

bool Window::ProcessMessages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            return false;
        }
    }
    return true;
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_CLOSE) {
                PostQuitMessage(0);
                return 0;
            }
            break;
    }
    
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }

    switch (msg) {
        case WM_SIZE:
            if (g_App) {
                Application::GetInstance();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) {
                return 0;
            }
            break;
        case WM_HOTKEY:
            if (g_App && wParam == 1) {
                g_App->GetHotkeyManager()->OnHotkeyReceived();
            }
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

Application::Application() = default;

Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    g_App = this;

    
    if (debugMode_) {
        Logger::GetInstance().SetDebugEnabled(true);
        LOG_INFO("Application", "Debug mode enabled - Console opened");
    }
    LOG_FUNC_ENTRY("Application", "Initialize");

    window_ = std::make_unique<Window>();
    if (!window_->Create(GetModuleHandle(nullptr), "Half Sword trainer")) {
        return false;
    }

    renderer_ = std::make_unique<Renderer>();
    if (!renderer_->Initialize(window_->GetHandle())) {
        return false;
    }

    InitializeImGui();

    processManager_ = std::make_unique<ProcessManager>();
    gameState_ = std::make_unique<GameState>();
    playerManager_ = std::make_unique<PlayerManagerSDK>();
    timeController_ = std::make_unique<TimeController>();
    enemyManager_ = std::make_unique<EnemyManagerSDK>();
    gameModeManager_ = std::make_unique<GameModeManagerSDK>();
    hotkeyManager_ = std::make_unique<HotkeyManager>();
    addressResolver_ = std::make_unique<AddressResolver>(*processManager_, *gameState_);

    hotkeyManager_->SetWindow(window_->GetHandle());
    hotkeyManager_->RegisterKey();

    uiManager_ = std::make_unique<UIManager>(*gameState_, *playerManager_, *timeController_,
                                             *enemyManager_, *gameModeManager_, *hotkeyManager_, *processManager_);
    uiManager_->Init(renderer_->GetDevice());

    window_->Show();
    initialized_ = true;
    return true;
}

void Application::Run() {
    LOG_FUNC_ENTRY("Application", "Run");
    int frameCount = 0;
    DWORD lastStatusLogTime = GetTickCount();
    static bool wasInMenu = false;
    static DWORD menuEnterTime = 0;
    static bool hasLoggedAttach = false;
    
    while (running_ && window_->ProcessMessages()) {
        frameCount++;
        DWORD currentTime = GetTickCount();
        
        
        bool shouldLogStatus = (currentTime - lastStatusLogTime >= 5000);
        
        if (uiManager_->ShouldClose()) {
            LOG_INFO("Application", "UI requested close");
            running_ = false;
            break;
        }
        
        hotkeyManager_->Update();
        
        if (!processManager_->IsAttached()) {
            
            if (!hasLoggedAttach) {
                LOG_INFO("Application", "Waiting for HalfSword process...");
                hasLoggedAttach = true;
            }
            processManager_->Attach("HalfSwordUE5-Win64-Shipping.exe");
            if (processManager_->IsAttached()) {
                LOG_INFO("Application", "Successfully attached to process");
                hasLoggedAttach = false;  
                HANDLE process = processManager_->GetProcess();
                
                playerManager_->SetProcess(process);
                timeController_->SetProcess(process);
                enemyManager_->SetProcess(process);
                gameModeManager_->SetProcess(process);
            }
        } else {
            
            bool resolved = addressResolver_->ResolveAddresses();
            const auto& addrs = gameState_->GetAddresses();
            bool inMenu = (addrs.uWorld == 0 || addrs.player == nullptr);
            
            
            if (inMenu != wasInMenu || shouldLogStatus) {
                if (inMenu) {
                    if (!wasInMenu) {
                        LOG_INFO("Application", "Entered menu/loading screen - pausing memory operations");
                        menuEnterTime = currentTime;
                    }
                } else {
                    if (wasInMenu) {
                        LOG_INFO("Application", "Entered game - resuming memory operations");
                    }
                    LOG_INFO("Application", "Addresses resolved - Player: 0x", std::hex, (uintptr_t)addrs.player);
                }
                lastStatusLogTime = currentTime;
            }
            wasInMenu = inMenu;
            
            if (resolved && !inMenu) {
                
                playerManager_->SetPlayer(addrs.player);
                enemyManager_->SetWorld(reinterpret_cast<HalfSword::SDK::UWorld*>(addrs.uWorld));
                enemyManager_->SetPlayer(reinterpret_cast<HalfSword::SDK::AWillie_BP_C*>(addrs.playerAddress));
                gameModeManager_->SetWorld(reinterpret_cast<HalfSword::SDK::UWorld*>(addrs.uWorld));
                
                
                if (addrs.timeDilation != 0) {
                    MemoryReader<float> reader(processManager_->GetProcess());
                    if (reader.IsValid(addrs.timeDilation)) {
                        timeController_->SetTimeDilationAddress(addrs.timeDilation);
                        float currentDilation = timeController_->GetCurrentTimeDilation();
                        gameState_->SetTimeDilation(currentDilation);
                    } else {
                        LOG_WARNING("Application", "TimeDilation address invalid, skipping");
                        timeController_->SetTimeDilationAddress(0);
                    }
                }
            } else {
                
                timeController_->SetTimeDilationAddress(0);
                gameState_->SetTimeDilation(1.0f);
                playerManager_->SetPlayer(nullptr);
            }
        }

        uiManager_->Update();

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        uiManager_->Render();

        ImGui::EndFrame();

        renderer_->BeginFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        renderer_->EndFrame();

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        renderer_->Present();

        running_ = !uiManager_->ShouldClose();
    }
    
    LOG_INFO("Application", "Main loop ended - Total frames: ", frameCount);
    LOG_FUNC_EXIT("Application", "Run");
}

void Application::Shutdown() {
    LOG_FUNC_ENTRY("Application", "Shutdown");
    
    if (!initialized_) {
        LOG_WARNING("Application", "Shutdown called but not initialized");
        return;
    }

    LOG_INFO("Application", "Unregistering hotkey...");
    hotkeyManager_->UnregisterKey();
    
    LOG_INFO("Application", "Shutting down ImGui...");
    ShutdownImGui();
    
    LOG_INFO("Application", "Shutting down renderer...");
    renderer_->Shutdown();
    
    LOG_INFO("Application", "Destroying window...");
    window_->Destroy();

    initialized_ = false;
    g_App = nullptr;
    
    LOG_INFO("Application", "Shutdown complete");
    LOG_FUNC_EXIT("Application", "Shutdown");
}

void Application::InitializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplWin32_Init(window_->GetHandle());
    ImGui_ImplDX9_Init(renderer_->GetDevice());
}

void Application::ShutdownImGui() {
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
