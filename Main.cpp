#include "src/Application.hpp"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Application& app = Application::GetInstance();
    
    if (!app.Initialize()) {
        return 1;
    }
    
    app.Run();
    app.Shutdown();
    
    return 0;
}
