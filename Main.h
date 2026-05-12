///////////////////////////////////////////////////////////////////////////////
// Main.h — Central header for Customization Tools
//
// This file defines all shared constants, data types, and the main
// ToolLauncher application class used throughout the project.
//
// Architecture:
//   ToolLauncher  —  Main window owner, message loop, tool management
//   ToolScanner   —  Discovers .bat/.py/.exe/.ps1 files in current directory
//   ToolRenderer  —  Draws tool cards using GDI+ (rounded rects, icons, text)
//   ToolIconManager — Generates 64×64 bitmap icons dynamically per extension
///////////////////////////////////////////////////////////////////////////////

#pragma once

// ─── Windows & System Headers ───────────────────────────────────────────────
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl_core.h>
#include <dwmapi.h>

// ─── C++ Standard Headers ───────────────────────────────────────────────────
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <stdexcept>

// ─── GDI+ Headers ───────────────────────────────────────────────────────────
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// Forward declarations
class ToolIconManager;
class ToolScanner;
class ToolRenderer;

// ─── Layout Constants ───────────────────────────────────────────────────────
constexpr int HEADER_HEIGHT       = 80;     // Height of the gradient header bar
constexpr int SEARCH_BOX_HEIGHT   = 10;     // Vertical gap below header for search
constexpr int TOOL_BUTTON_SIZE    = 150;    // Width & height of each tool card
constexpr int COLS_PER_ROW        = 8;      // Number of tool cards per grid row
constexpr int TOOL_GRID_SPACING   = 16;     // Horizontal gap between tool cards
constexpr int TOOL_LABEL_HEIGHT   = 40;     // Height reserved for tool name text
constexpr int TOOL_ROW_EXTRA      = 60;     // Vertical gap between grid rows
constexpr int CONTENT_MARGIN      = 32;     // Left/right margin for tool grid
constexpr int TOOL_ICON_SIZE      = 64;     // Width & height of tool icon bitmap
constexpr int INVALIDATE_PADDING  = 4;      // Extra pixels to invalidate around cards

// ─── Color Constants ────────────────────────────────────────────────────────

// Windows 11 theme palette
constexpr COLORREF WIN11_BACKGROUND     = RGB(249, 249, 249);
constexpr COLORREF WIN11_SURFACE        = RGB(255, 255, 255);
constexpr COLORREF WIN11_HOVER          = RGB(243, 242, 241);
constexpr COLORREF WIN11_ACCENT         = RGB(0, 120, 212);
constexpr COLORREF WIN11_TEXT           = RGB(32, 31, 30);
constexpr COLORREF WIN11_TEXT_SECONDARY = RGB(96, 94, 92);

// Legacy aliases (used across older code — kept for compatibility)
constexpr COLORREF win11_background     = WIN11_BACKGROUND;
constexpr COLORREF win11_surface        = WIN11_SURFACE;
constexpr COLORREF win11_hover          = WIN11_HOVER;
constexpr COLORREF win11_accent         = WIN11_ACCENT;
constexpr COLORREF win11_text           = WIN11_TEXT;
constexpr COLORREF win11_text_secondary = WIN11_TEXT_SECONDARY;

// Application-specific colors
constexpr COLORREF TOOLS_AVAILABLE_COLOR = RGB(26, 26, 255);     // Blue subtitle text
constexpr COLORREF STATUS_BAR_BG         = RGB(214, 226, 242);   // Light blue status bar
constexpr COLORREF STATUS_BAR_TEXT       = RGB(64, 64, 64);      // Dark gray status text
constexpr COLORREF SEARCH_TEXT_COLOR     = RGB(51, 51, 1);       // Dark olive search text
constexpr COLORREF SEARCH_ICON_COLOR    = RGB(120, 120, 120);    // Gray search magnifier

// ─── Enums ──────────────────────────────────────────────────────────────────
enum class ViewMode {
    List,         // Vertical list layout (one tool per row)
    VIEW_GRID,    // Grid layout (COLS_PER_ROW cards per row) — default
    Details       // Detailed list with extra info (reserved for future use)
};

// ─── ToolInfo — Metadata for a Single Discovered Tool ───────────────────────
struct ToolInfo {
    std::wstring filename;                  // Full filename (e.g., "close.bat")
    std::wstring displayName;               // Name without extension (e.g., "close")
    std::wstring extension;                 // File extension (e.g., ".bat")
    RECT         rect = { 0, 0, 0, 0 };    // Bounding rectangle on screen (for hit-testing)
    HBITMAP      icon = nullptr;            // 64×64 icon bitmap handle
};

// ─── ToolLauncher — Main Application Class ──────────────────────────────────
//
// Owns the main window, search bar, status bar, tool data, and rendering.
// Coordinates between ToolScanner (discovery), ToolIconManager (icons),
// and ToolRenderer (drawing) to present a searchable tool grid.
//
class ToolLauncher {
public:
    ToolLauncher();
    ~ToolLauncher();

    bool CreateMainWindow();    // Register window class and create the HWND
    int  MessageLoop();         // Standard Win32 GetMessage/DispatchMessage loop
    void Show(int nCmdShow);    // Show and update the window

private:
    // ── Window Handles ──────────────────────────────────────────────────────
    HWND hwnd         = nullptr;    // Main application window
    HWND searchBox    = nullptr;    // Search edit control
    HWND statusBar    = nullptr;    // Bottom status bar
    HWND searchPanel  = nullptr;    // Search container (owner-drawn static)
    HWND searchLabel  = nullptr;    // Search label (unused, reserved)
    HWND clearButton  = nullptr;    // "×" clear search button

    // ── GDI+ ────────────────────────────────────────────────────────────────
    ULONG_PTR           gdiplusToken = 0;
    GdiplusStartupInput gdiplusStartupInput;

    // ── GDI Brushes (pre-created to avoid per-paint allocation leaks) ───────
    HBRUSH backgroundBrush  = nullptr;  // Main window background
    HBRUSH buttonBrush      = nullptr;  // Tool card surface
    HBRUSH hoverBrush       = nullptr;  // Tool card hover state
    HBRUSH accentBrush      = nullptr;  // Accent highlight
    HBRUSH statusBarBrush   = nullptr;  // Status bar background (returned in WM_CTLCOLORSTATIC)
    HBRUSH searchPanelBrush = nullptr;  // Search panel background (used in WM_DRAWITEM)

    // ── GDI Fonts (pre-created to avoid per-paint allocation leaks) ─────────
    HFONT headerFont   = nullptr;   // 32pt Segoe UI Variable — window header title
    HFONT toolFont     = nullptr;   // 14pt Segoe UI Variable Text — tool card text
    HFONT searchFont   = nullptr;   // 16pt Segoe UI Variable Text — search box
    HFONT modernFont   = nullptr;   // 20pt Times New Roman — search/status controls
    HFONT subtitleFont = nullptr;   // 23pt Times New Roman — header subtitle

    // ── Tool Data ───────────────────────────────────────────────────────────
    std::vector<ToolInfo>              tools;          // All discovered tools
    std::vector<ToolInfo>              filteredTools;  // Currently displayed (after search filter)
    std::unique_ptr<ToolIconManager>   iconManager;    // Generates icons per extension
    std::unique_ptr<ToolScanner>       scanner;        // Scans directory for tool files
    std::unique_ptr<ToolRenderer>      renderer;       // Draws tool cards via GDI+

    // ── Double Buffering (flicker-free rendering) ───────────────────────────
    HDC     hBufferDC     = nullptr;
    HBITMAP hBufferBitmap = nullptr;
    int     lastWidth     = 0;
    int     lastHeight    = 0;

    // ── Interaction State ───────────────────────────────────────────────────
    int      hoveredTool     = -1;                  // Index of tool under cursor (-1 = none, -2 = clear button)
    int      lastHoveredTool = -1;                  // Previous hover index (for change detection)
    int      selectedTool    = -1;                  // Index of tool being clicked (mousedown state)
    bool     isTrackingMouse = false;               // Whether TrackMouseEvent is active
    ViewMode viewMode        = ViewMode::VIEW_GRID; // Current layout mode

    // ── Scroll State ────────────────────────────────────────────────────────
    int  scrollX = 0, scrollY = 0;              // Current scroll offset
    int  maxScrollX = 0, maxScrollY = 0;        // Maximum scroll values
    int  virtualWidth = 0, virtualHeight = 0;   // Total content size
    bool showHScrollBar = false;                // Whether horizontal scrollbar is visible
    bool showVScrollBar = false;                // Whether vertical scrollbar is visible

    // ── Core Methods ────────────────────────────────────────────────────────
    void ScanForTools();                                        // Discover tools via ToolScanner
    void FilterTools(const std::wstring& searchText);           // Filter tools by search query
    void LaunchTool(int index);                                 // Launch tool via ShellExecuteEx
    void CalculateToolPositions();                              // Compute bounding rects for all tools
    int  GetToolAtPoint(POINT pt) const;                        // Hit-test: which tool is at this point?

    // ── Drawing ─────────────────────────────────────────────────────────────
    void OnPaint(HDC hdc);                                              // Main render function (double-buffered)
    void UpdateDoubleBuffer(int width, int height);                     // Recreate offscreen buffer on resize
    void CleanupDoubleBuffer();                                         // Free offscreen buffer resources
    void UpdateStatusText(const std::wstring& message, int toolCount) const; // Update status bar text
    void DrawSearchIcon(HDC hdc, int x, int y) const;                   // Draw magnifying glass icon
    static void ConvertTopropercase(std::wstring& str);                 // Convert "hello_world" → "Hello World"

    // ── Scrolling ───────────────────────────────────────────────────────────
    void UpdateScrollBars();                        // Recalculate scrollbar ranges
    void HandleHorizontalScroll(WPARAM wParam);     // Process WM_HSCROLL messages
    void HandleVerticalScroll(WPARAM wParam);       // Process WM_VSCROLL messages
    void InvalidateToolRegion(int toolId);          // Repaint only one tool card's region
    void CalculateVirtualSize();                    // Compute total content dimensions

    // ── Message Handling ────────────────────────────────────────────────────
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    friend class ToolRenderer;  // ToolRenderer needs access to fonts/brushes/state
};
