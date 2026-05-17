///////////////////////////////////////////////////////////////////////////////
// Main.cpp — Application Entry Point
//
// This is the starting point of the Customization Tools launcher.
// It initializes COM (required for ShellExecuteEx), creates the main
// window, and runs the Win32 message loop until the user closes the app.
///////////////////////////////////////////////////////////////////////////////

// Ensure Unicode mode — must be defined BEFORE including Windows headers
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "Main.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    // ── Step 1: Initialize COM ──────────────────────────────────────────────
    // COM is required for ShellExecuteEx (tool launching) and file dialogs.
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        MessageBoxW(NULL, L"Failed to initialize COM", L"Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // ── Step 2: Create and run the application ──────────────────────────────
    ToolLauncher launcher;

    if (!launcher.CreateMainWindow()) {
        CoUninitialize();
        return 1;
    }

    launcher.Show(nCmdShow);
    int result = launcher.MessageLoop();    // Blocks until WM_QUIT

    // ── Step 3: Cleanup ─────────────────────────────────────────────────────
    CoUninitialize();
    return result;
}
