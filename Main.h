#pragma once

// Windows & System Headers
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl_core.h>
#include <dwmapi.h>

// C++ Standard Headers
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <stdexcept>

// GDI+ Headers
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// Forward declarations
class ToolIconManager;
class ToolScanner;
class ToolRenderer;

// ────────────────────────────────────────────────────────────────
// Constants — Layout & Theme (Windows 11 Style)
// ────────────────────────────────────────────────────────────────
constexpr int HEADER_HEIGHT = 80;
constexpr int SEARCH_BOX_HEIGHT = 10;
constexpr int TOOL_BUTTON_SIZE = 150;
constexpr int COLS_PER_ROW = 8;

// Windows 11 Color Palette
constexpr COLORREF win11_background = RGB(249, 249, 249);
constexpr COLORREF win11_surface = RGB(255, 255, 255);
constexpr COLORREF win11_hover = RGB(243, 242, 241);
constexpr COLORREF win11_accent = RGB(0, 120, 212);
constexpr COLORREF win11_text = RGB(32, 31, 30);
constexpr COLORREF win11_text_secondary = RGB(96, 94, 92);
constexpr COLORREF TOOLS_AVAILABLE_COLOR = RGB(26, 26, 255);

// ────────────────────────────────────────────────────────────────
// Enums
// ────────────────────────────────────────────────────────────────
enum class ViewMode {
    List,
    VIEW_GRID,
    Details
};

// ────────────────────────────────────────────────────────────────
// ToolInfo — Tool Metadata & Drawing Info
// ────────────────────────────────────────────────────────────────
struct ToolInfo {
    std::wstring filename;
    std::wstring displayName;
    std::wstring extension;
    RECT rect = { 0, 0, 0, 0 };
    HBITMAP icon = nullptr;
};

// ────────────────────────────────────────────────────────────────
// ToolLauncher — Main Application Class
// ────────────────────────────────────────────────────────────────
class ToolLauncher {
public:
    ToolLauncher();
    ~ToolLauncher();

    bool CreateMainWindow();
    int MessageLoop();
    void Show(int nCmdShow);

private:
    // Window handles
    HWND hwnd = nullptr;
    HWND searchBox = nullptr;
    HWND statusBar = nullptr;
    HWND searchPanel = nullptr;
    HWND searchLabel = nullptr;
    HWND clearButton = nullptr;

    // GDI+ resources
    ULONG_PTR gdiplusToken = 0;
    GdiplusStartupInput gdiplusStartupInput;

    // GDI objects
    HBRUSH backgroundBrush = nullptr;
    HBRUSH buttonBrush = nullptr;
    HBRUSH hoverBrush = nullptr;
    HBRUSH accentBrush = nullptr;
    HFONT headerFont = nullptr;
    HFONT toolFont = nullptr;
    HFONT searchFont = nullptr;
    HFONT modernFont = nullptr;

    // Tool data
    std::vector<ToolInfo> tools;
    std::vector<ToolInfo> filteredTools;
    std::unique_ptr<ToolIconManager> iconManager;
    std::unique_ptr<ToolScanner> scanner;
    std::unique_ptr<ToolRenderer> renderer;

    // Double buffering
    HDC hBufferDC = nullptr;
    HBITMAP hBufferBitmap = nullptr;
    int lastWidth = 0;
    int lastHeight = 0;

    // State tracking
    int hoveredTool = -1;
    int lastHoveredTool = -1;
    int selectedTool = -1;
    bool isTrackingMouse = false;
    ViewMode viewMode = ViewMode::VIEW_GRID;

    // Scroll bars
    int scrollX = 0, scrollY = 0;
    int maxScrollX = 0, maxScrollY = 0;
    int virtualWidth = 0, virtualHeight = 0;
    bool showHScrollBar = false;
    bool showVScrollBar = false;

    // Core methods
    void ScanForTools();
    void FilterTools(const std::wstring& searchText);
    void LaunchTool(int index);
    void CalculateToolPositions();
    int GetToolAtPoint(POINT pt);

    // Drawing
    void OnPaint(HDC hdc);
    void UpdateDoubleBuffer(int width, int height);
    void CleanupDoubleBuffer();
    void UpdateStatusText(const std::wstring& message, int toolCount);
    void DrawSearchIcon(HDC hdc, int x, int y);
    void ConvertTopropercase(std::wstring& str);

    // Scroll methods
    void UpdateScrollBars();
    void HandleHorizontalScroll(WPARAM wParam);
    void HandleVerticalScroll(WPARAM wParam);
    void InvalidateToolRegion(int toolId);
    void CalculateVirtualSize();

    // Message handling
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    friend class ToolRenderer; // Grant ToolRenderer access if needed
};

