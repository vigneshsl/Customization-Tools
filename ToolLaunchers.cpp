///////////////////////////////////////////////////////////////////////////////
// ToolLaunchers.cpp — ToolLauncher Lifecycle & Core Logic
//
// Implements construction, destruction, window creation, tool scanning,
// layout calculation, search filtering, and tool launching.
///////////////////////////////////////////////////////////////////////////////

#include "Main.h"
#include "ToolIconManager.h"
#include "ToolScanner.h"
#include "ToolRenderer.h"
#include "Resource.h"
#include <algorithm>
#include <memory>
#include <cwctype>
#undef max

using namespace std;
using namespace Gdiplus;

// ── Library Dependencies ────────────────────────────────────────────────────
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")

///////////////////////////////////////////////////////////////////////////////
// Constructor — Initialize GDI+, create all brushes/fonts, instantiate helpers
///////////////////////////////////////////////////////////////////////////////
ToolLauncher::ToolLauncher()
    : hwnd(nullptr), searchBox(nullptr), statusBar(nullptr),
    hoveredTool(-1), selectedTool(-1),
    hBufferBitmap(nullptr), hBufferDC(nullptr),
    lastWidth(0), lastHeight(0),
    viewMode(ViewMode::VIEW_GRID)
{
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    // Pre-create all brushes (avoids per-paint allocation & GDI leaks)
    backgroundBrush  = CreateSolidBrush(win11_background);
    buttonBrush      = CreateSolidBrush(win11_surface);
    hoverBrush       = CreateSolidBrush(win11_hover);
    accentBrush      = CreateSolidBrush(win11_accent);
    statusBarBrush   = CreateSolidBrush(STATUS_BAR_BG);
    searchPanelBrush = CreateSolidBrush(RGB(252, 252, 252));

    // Pre-create all fonts (avoids per-paint allocation & GDI leaks)
    headerFont = CreateFont(32, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable");

    toolFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");

    searchFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");

    subtitleFont = CreateFont(
        23, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");

    // Instantiate helper components
    iconManager = make_unique<ToolIconManager>();
    scanner     = make_unique<ToolScanner>(iconManager.get());
    renderer    = make_unique<ToolRenderer>(this);
}

///////////////////////////////////////////////////////////////////////////////
// Destructor — Release all GDI resources, icon bitmaps, and offscreen buffer
///////////////////////////////////////////////////////////////////////////////
ToolLauncher::~ToolLauncher()
{
    GdiplusShutdown(gdiplusToken);

    // Delete brushes
    DeleteObject(backgroundBrush);
    DeleteObject(buttonBrush);
    DeleteObject(hoverBrush);
    DeleteObject(accentBrush);
    DeleteObject(statusBarBrush);
    DeleteObject(searchPanelBrush);

    // Delete fonts
    DeleteObject(headerFont);
    DeleteObject(toolFont);
    DeleteObject(searchFont);
    DeleteObject(subtitleFont);

    // Delete tool icon bitmaps
    for (auto& tool : tools)
    {
        if (tool.icon)
            DeleteObject(tool.icon);
    }

    CleanupDoubleBuffer();
}

///////////////////////////////////////////////////////////////////////////////
// CreateMainWindow — Register window class, create HWND, set DWM attributes
///////////////////////////////////////////////////////////////////////////////
bool ToolLauncher::CreateMainWindow()
{
    const wchar_t CLASS_NAME[] = L"Win11ToolLauncher";

    WNDCLASS wc = {};
    wc.lpfnWndProc   = ToolLauncher::WndProc;
    wc.hInstance      = GetModuleHandle(nullptr);
    wc.lpszClassName  = CLASS_NAME;
    wc.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground  = backgroundBrush;
    wc.hIcon          = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_UI));

    if (!RegisterClass(&wc)) {
        MessageBoxW(nullptr, L"Window Registration Failed!", L"Error", MB_ICONERROR);
        return false;
    }

    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW | WS_EX_CONTEXTHELP,
        CLASS_NAME, L"Customization Tools",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 700,
        nullptr, nullptr, GetModuleHandle(nullptr), this);

    if (!hwnd) {
        MessageBoxW(nullptr, L"Window Creation Failed!", L"Error", MB_ICONERROR);
        return false;
    }

    // Set window icon (taskbar + title bar)
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_UI)));
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_UI)));

    // Enable Windows 11 rounded corners
    DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// MessageLoop — Standard Win32 message pump (blocks until WM_QUIT)
///////////////////////////////////////////////////////////////////////////////
int ToolLauncher::MessageLoop()
{
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

///////////////////////////////////////////////////////////////////////////////
// Show — Display and force-update the window
///////////////////////////////////////////////////////////////////////////////
void ToolLauncher::Show(int nCmdShow)
{
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}

///////////////////////////////////////////////////////////////////////////////
// ScanForTools — Discover all tool files and refresh the display
///////////////////////////////////////////////////////////////////////////////
void ToolLauncher::ScanForTools()
{
    tools = scanner->ScanForTools();

    if (tools.empty())
    {
        MessageBox(hwnd, L"Tools not available!", L"Warning", MB_ICONWARNING);
    }

    filteredTools = tools;
    scrollX = scrollY = 0;

    CalculateVirtualSize();
    CalculateToolPositions();           // Also calls UpdateScrollBars() internally
    InvalidateRect(hwnd, nullptr, TRUE);
}

///////////////////////////////////////////////////////////////////////////////
// CalculateToolPositions — Assign screen rects to each tool based on view mode
//
// Grid mode: cards laid out in COLS_PER_ROW columns with TOOL_GRID_SPACING gap
// List mode: single column of 600px-wide rows
///////////////////////////////////////////////////////////////////////////////
void ToolLauncher::CalculateToolPositions()
{
    int maxX = 0, maxY = 0;
    const int startX = CONTENT_MARGIN;
    const int startY = HEADER_HEIGHT + SEARCH_BOX_HEIGHT + 70;

    if (viewMode == ViewMode::VIEW_GRID)
    {
        for (size_t i = 0; i < filteredTools.size(); ++i)
        {
            int col = static_cast<int>(i % COLS_PER_ROW);
            int row = static_cast<int>(i / COLS_PER_ROW);

            int x = startX + col * (TOOL_BUTTON_SIZE + TOOL_GRID_SPACING);
            int y = startY + row * (TOOL_BUTTON_SIZE + TOOL_ROW_EXTRA);

            filteredTools[i].rect = {
                x - scrollX, y - scrollY,
                x - scrollX + TOOL_BUTTON_SIZE,
                y - scrollY + TOOL_BUTTON_SIZE + TOOL_LABEL_HEIGHT
            };

            maxX = max(maxX, x + TOOL_BUTTON_SIZE + CONTENT_MARGIN);
            maxY = max(maxY, y + TOOL_BUTTON_SIZE + TOOL_ROW_EXTRA + 12);
        }
    }
    else
    {
        maxX = startX + 600 + CONTENT_MARGIN;
        maxY = startY;

        for (size_t i = 0; i < filteredTools.size(); ++i)
        {
            int y = startY + static_cast<int>(i * 60);

            filteredTools[i].rect = {
                startX - scrollX, y - scrollY,
                startX - scrollX + 600,
                y - scrollY + 50
            };

            maxY = max(maxY, y + 82);
        }
    }

    virtualWidth  = maxX;
    virtualHeight = maxY;

    UpdateScrollBars();
}

///////////////////////////////////////////////////////////////////////////////
// FilterTools — Show only tools matching the search query (case-insensitive)
///////////////////////////////////////////////////////////////////////////////
void ToolLauncher::FilterTools(const std::wstring& searchText)
{
    filteredTools.clear();

    if (searchText.empty()) {
        filteredTools = tools;
    }
    else {
        // Lowercase the search query once
        std::wstring lowerSearch = searchText;
        std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::towlower);

        for (const auto& tool : tools) {
            std::wstring name = tool.displayName;
            std::replace(name.begin(), name.end(), L'_', L' ');
            std::transform(name.begin(), name.end(), name.begin(), ::towlower);

            if (name.find(lowerSearch) != std::wstring::npos) {
                filteredTools.push_back(tool);
            }
        }
    }

    scrollX = scrollY = 0;
    CalculateVirtualSize();
    CalculateToolPositions();           // Also calls UpdateScrollBars() internally
    InvalidateRect(hwnd, nullptr, TRUE);
}

///////////////////////////////////////////////////////////////////////////////
// LaunchTool — Execute the selected tool using ShellExecuteEx
///////////////////////////////////////////////////////////////////////////////
void ToolLauncher::LaunchTool(int index)
{
    if (index >= 0 && index < static_cast<int>(filteredTools.size()))
    {
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = L"open";
        sei.lpFile = filteredTools[index].filename.c_str();
        sei.nShow  = SW_SHOWNORMAL;

        std::wstring status = ShellExecuteEx(&sei)
            ? L"✓ Launched: " + filteredTools[index].displayName
            : L"✗ Failed to launch: " + filteredTools[index].displayName;

        SendMessage(statusBar, SB_SETTEXT, 0, (LPARAM)status.c_str());
    }
}

///////////////////////////////////////////////////////////////////////////////
// GetToolAtPoint — Hit-test: return the index of the tool at the given point,
//                  or -1 if no tool is under the cursor.
///////////////////////////////////////////////////////////////////////////////
int ToolLauncher::GetToolAtPoint(POINT pt) const
{
    for (size_t i = 0; i < filteredTools.size(); ++i)
    {
        if (PtInRect(&filteredTools[i].rect, pt))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}
