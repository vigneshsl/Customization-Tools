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

#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")

ToolLauncher::ToolLauncher()
    : hwnd(nullptr), searchBox(nullptr), statusBar(nullptr),
    hoveredTool(-1), selectedTool(-1),
    hBufferBitmap(nullptr), hBufferDC(nullptr),
    lastWidth(0), lastHeight(0),
    viewMode(ViewMode::VIEW_GRID)
{
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    backgroundBrush = CreateSolidBrush(win11_background);
    buttonBrush = CreateSolidBrush(win11_surface);
    hoverBrush = CreateSolidBrush(win11_hover);
    accentBrush = CreateSolidBrush(win11_accent);

    headerFont = CreateFont(32, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable");

    toolFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");

    searchFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");

    iconManager = make_unique<ToolIconManager>();
    scanner = make_unique<ToolScanner>(iconManager.get());
    renderer = make_unique<ToolRenderer>(this);
}

ToolLauncher::~ToolLauncher()
{
    GdiplusShutdown(gdiplusToken);

    DeleteObject(backgroundBrush);
    DeleteObject(buttonBrush);
    DeleteObject(hoverBrush);
    DeleteObject(accentBrush);

    DeleteObject(headerFont);
    DeleteObject(toolFont);
    DeleteObject(searchFont);

    for (auto& tool : tools)
    {
        if (tool.icon)
            DeleteObject(tool.icon);
    }

    CleanupDoubleBuffer();
}

bool ToolLauncher::CreateMainWindow()
{
    const wchar_t CLASS_NAME[] = L"Win11ToolLauncher";

    WNDCLASS wc = {};
    wc.lpfnWndProc = ToolLauncher::WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = backgroundBrush;
    wc.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_UI));

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

    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_UI)));
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_UI)));

    DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));

    return true;
}

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

void ToolLauncher::Show(int nCmdShow)
{
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}

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
    UpdateScrollBars();
    CalculateToolPositions();
    InvalidateRect(hwnd, nullptr, TRUE);
}

void ToolLauncher::CalculateToolPositions()
{
    int maxX = 0, maxY = 0;
    const int startX = 32;
    const int startY = HEADER_HEIGHT + SEARCH_BOX_HEIGHT + 70;

    if (viewMode == ViewMode::VIEW_GRID)
    {
        for (size_t i = 0; i < filteredTools.size(); ++i)
        {
            int col = static_cast<int>(i % COLS_PER_ROW);
            int row = static_cast<int>(i / COLS_PER_ROW);

            int x = startX + col * (TOOL_BUTTON_SIZE + 16);
            int y = startY + row * (TOOL_BUTTON_SIZE + 60);

            filteredTools[i].rect = {
                x - scrollX, y - scrollY,
                x - scrollX + TOOL_BUTTON_SIZE,
                y - scrollY + TOOL_BUTTON_SIZE + 40
            };

            maxX = max(maxX, x + TOOL_BUTTON_SIZE + 32);
            maxY = max(maxY, y + TOOL_BUTTON_SIZE + 72);
        }
    }
    else
    {
        maxX = startX + 600 + 32;
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

    virtualWidth = maxX;
    virtualHeight = maxY;

    UpdateScrollBars();
}

void ToolLauncher::FilterTools(const std::wstring& searchText)
{
    filteredTools.clear();

    if (searchText.empty()) {
        filteredTools = tools;
    }
    else {
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
    UpdateScrollBars();
    CalculateToolPositions();
    InvalidateRect(hwnd, nullptr, TRUE);
}

void ToolLauncher::LaunchTool(int index)
{
    if (index >= 0 && index < static_cast<int>(filteredTools.size()))
    {
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = L"open";
        sei.lpFile = filteredTools[index].filename.c_str();
        sei.nShow = SW_SHOWNORMAL;

        std::wstring status = ShellExecuteEx(&sei)
            ? L"✓ Launched: " + filteredTools[index].displayName
            : L"✗ Failed to launch: " + filteredTools[index].displayName;

        SendMessage(statusBar, SB_SETTEXT, 0, (LPARAM)status.c_str());
    }
}

int ToolLauncher::GetToolAtPoint(POINT pt)
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
