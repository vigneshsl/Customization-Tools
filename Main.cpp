#include "Main.h"
#define UNICODE
#define _UNICODE

//////////////////////////////////////////////////////////////////////
// Function: wWinMain
// Purpose : This is the starting point of any Win32 GUI application.
// Notes   : It sets up COM, creates the main window, and runs the app loop.
//////////////////////////////////////////////////////////////////////

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{

    //////////////////////////////////////////////////////////////////////
    // Step 1: Initialize COM (Component Object Model)
    // COM is required for launching external tools using Shell APIs
    //////////////////////////////////////////////////////////////////////

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        MessageBoxW(NULL, L"Failed to initialize COM", L"Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    //////////////////////////////////////////////////////////////////////
    // Step 2: Create and run the application
    //////////////////////////////////////////////////////////////////////
    ToolLauncher launcher;                        // Create our app class

    if (!launcher.CreateMainWindow()) {           // Try to create the main window
        CoUninitialize();                         // Cleanup COM on failure
        return 1;                                 // Exit the app
    }

    launcher.Show(nCmdShow);                      // Show the main window
    int result = launcher.MessageLoop();          // Start the message loop (app stays open here)

    //////////////////////////////////////////////////////////////////////
    // Step 3: Clean up
    //////////////////////////////////////////////////////////////////////
    CoUninitialize();                             // Uninitialize COM after use

    return result;                                // Return the exit code of the app
}
