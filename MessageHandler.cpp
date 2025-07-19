#include "Main.h"

////////////////////////////////////////////////////////////////////////////////////
//
// ToolLauncher::HandleMessage - Windows 11 Style Optimized UI
// This function handles Windows messages for the ToolLauncher window.
// FIXED: Mouse hover flickering issue resolved with proper invalidation
//
////////////////////////////////////////////////////////////////////////////////////

LRESULT ToolLauncher::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        // ═══════════════════════════════════════════════════════════════
        // 1. WINDOW CREATION - Optimized Search & Status Bar
        // ═══════════════════════════════════════════════════════════════
    case WM_CREATE:
    {
        // Initialize common Windows controls
        INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES };
        InitCommonControlsEx(&icex);

        // Get the dimensions of the client area
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // ──────────────────────────────────────────────────────────
        // OPTIMIZED SEARCH CONTAINER - More Efficient Creation
        // ──────────────────────────────────────────────────────────
        const int SEARCH_MAX_WIDTH = 600;
        const int SEARCH_HEIGHT = 48;
        const int SEARCH_MARGIN = 60;

        int searchWidth = min(SEARCH_MAX_WIDTH, clientRect.right - SEARCH_MARGIN);
        int searchX = (clientRect.right - searchWidth) / 2;
        int searchY = HEADER_HEIGHT + 16;

        // Single search container with built-in styling
        searchPanel = CreateWindowEx
        (
            WS_EX_COMPOSITED,
            L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_NOTIFY,
            searchX, searchY, searchWidth, SEARCH_HEIGHT,
            hwnd, (HMENU)1003, GetModuleHandle(NULL), NULL
        );

        // Optimized search box with better positioning
        const int ICON_SPACE = 40;
        const int CLEAR_SPACE = 36;
        const int BOX_HEIGHT = 32;

        searchBox = CreateWindowEx
        (
            WS_EX_CLIENTEDGE,
            L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP |
            ES_LEFT | ES_AUTOHSCROLL,
            searchX + ICON_SPACE,
            searchY + (SEARCH_HEIGHT - BOX_HEIGHT) / 2,
            searchWidth - ICON_SPACE - CLEAR_SPACE,
            BOX_HEIGHT,
            hwnd, (HMENU)1001, GetModuleHandle(NULL), NULL
        );

        // Simplified clear button with Unicode symbol
        clearButton = CreateWindowEx(
            0, L"BUTTON", L"×",
            WS_CHILD | BS_FLAT | WS_TABSTOP | BS_CENTER | BS_VCENTER,
            searchX + searchWidth - 32,
            searchY + (SEARCH_HEIGHT - 28) / 2,
            28, 28,
            hwnd, (HMENU)1005, GetModuleHandle(NULL), NULL);

        // ──────────────────────────────────────────────────────────
        // MODIFIED FONT CREATION - Red Font Color
        // ──────────────────────────────────────────────────────────
        modernFont = CreateFont(
            20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS,
            L"Times New Roman");

        // Apply font efficiently
        if (modernFont) {
            SendMessage(searchBox, WM_SETFONT, (WPARAM)modernFont, TRUE);
            SendMessage(clearButton, WM_SETFONT, (WPARAM)modernFont, TRUE);
        }

        // Set red text color for search box
        HDC hdc = GetDC(searchBox);
        SetTextColor(hdc, RGB(51, 255, 119)); // Red color
        ReleaseDC(searchBox, hdc);

        // Enhanced placeholder with better UX text
        SendMessage(searchBox, EM_SETCUEBANNER, TRUE,
            (LPARAM)L"Search tools...");

        // Optimized text margins
        SendMessage(searchBox, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
            MAKELPARAM(8, 8));

        // ──────────────────────────────────────────────────────────
        // MODIFIED STATUS BAR 
        // ──────────────────────────────────────────────────────────
        statusBar = CreateWindowEx(
            0, STATUSCLASSNAME, NULL,
            WS_CHILD | WS_VISIBLE | CCS_BOTTOM,
            0, 0, 0, 0, hwnd, (HMENU)1002, GetModuleHandle(NULL), NULL);

        if (statusBar) {
            // Simplified status parts - single part for cleaner look
            int statusParts[] = { -1 };
            SendMessage(statusBar, SB_SETPARTS, 1, (LPARAM)statusParts);

            // Apply modern font with red color
            if (modernFont)
            {
                SendMessage(statusBar, WM_SETFONT, (WPARAM)modernFont, TRUE);
            }

            // Set green background for status bar
            SendMessage(statusBar, SB_SETBKCOLOR, 0, RGB(214, 226, 242));
        }

        // Apply modern theming
        SetWindowTheme(searchBox, L"Explorer", NULL);
        SetWindowTheme(statusBar, L"Explorer", NULL);

        // Initially hide clear button
        ShowWindow(clearButton, SW_HIDE);

        // Initialize hover tracking
        hoveredTool = -1;
        lastHoveredTool = -1;
        isTrackingMouse = false;

        // Scan and load all available tools
        ScanForTools();

        // Set initial status
        UpdateStatusText(L"Ready", static_cast<int>(filteredTools.size()));
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 2. OPTIMIZED CUSTOM DRAWING
    // ═══════════════════════════════════════════════════════════════
    case WM_DRAWITEM:
    {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;

        if (dis->CtlID == 1003) // Search panel - simplified drawing
        {
            // Modern Windows 11 acrylic-like background
            HBRUSH bgBrush = CreateSolidBrush(RGB(252, 252, 252));
            FillRect(dis->hDC, &dis->rcItem, bgBrush);

            // Subtle modern border
            HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(225, 225, 225));
            HPEN oldPen = (HPEN)SelectObject(dis->hDC, borderPen);
            SelectObject(dis->hDC, GetStockObject(HOLLOW_BRUSH));

            // Draw rounded rectangle
            RoundRect(dis->hDC, dis->rcItem.left, dis->rcItem.top,
                dis->rcItem.right, dis->rcItem.bottom, 6, 6);

            // Draw search icon directly
            DrawSearchIcon(dis->hDC, dis->rcItem.left + 14,
                dis->rcItem.top + (dis->rcItem.bottom - dis->rcItem.top - 16) / 2);

            SelectObject(dis->hDC, oldPen);
            DeleteObject(borderPen);
            DeleteObject(bgBrush);
        }
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 3. ENHANCED PAINTING WITH WINDOWS 11 AESTHETICS
    // ═══════════════════════════════════════════════════════════════
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Set modern background color
        HBRUSH backgroundBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdc, &ps.rcPaint, backgroundBrush);
        DeleteObject(backgroundBrush);

        OnPaint(hdc);  // Call custom painting function
        EndPaint(hwnd, &ps);
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 4. OPTIMIZED RESIZE HANDLING - Performance Focused
    // ═══════════════════════════════════════════════════════════════
    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        // Batch window updates for better performance
        HDWP hdwp = BeginDeferWindowPos(4);

        if (hdwp && statusBar) {
            hdwp = DeferWindowPos(hdwp, statusBar, NULL, 0, 0, 0, 0,
                SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
            SendMessage(statusBar, WM_SIZE, 0, 0);
        }

        // Recalculate search components
        int searchWidth = min(600, width - 60);
        int searchX = (width - searchWidth) / 2;

        if (hdwp && searchPanel) {
            hdwp = DeferWindowPos(hdwp, searchPanel, NULL,
                searchX, HEADER_HEIGHT + 16, searchWidth, 48,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }

        if (hdwp && searchBox) {
            hdwp = DeferWindowPos(hdwp, searchBox, NULL,
                searchX + 40, HEADER_HEIGHT + 24,
                searchWidth - 76, 32,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }

        if (hdwp && clearButton) {
            hdwp = DeferWindowPos(hdwp, clearButton, NULL,
                searchX + searchWidth - 32, HEADER_HEIGHT + 26,
                28, 28,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }

        if (hdwp) {
            EndDeferWindowPos(hdwp);
        }

        // FIXED: Proper scroll bar update sequence
        CalculateVirtualSize();
        UpdateScrollBars();
        CalculateToolPositions();
        UpdateDoubleBuffer(width, height);
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 5. FIXED MOUSE INTERACTION - NO MORE FLICKERING
    // ═══════════════════════════════════════════════════════════════
    case WM_MOUSEMOVE:
    {
        // Enable mouse tracking if not already enabled
        if (!isTrackingMouse) {
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            isTrackingMouse = true;
        }

        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int newHoveredTool = GetToolAtPoint(pt);

        // Check if hovering over clear button
        RECT clearButtonRect;
        GetWindowRect(clearButton, &clearButtonRect);
        ScreenToClient(hwnd, (POINT*)&clearButtonRect.left);
        ScreenToClient(hwnd, (POINT*)&clearButtonRect.right);

        bool hoveringClearButton = PtInRect(&clearButtonRect, pt);
        int clearButtonHover = hoveringClearButton ? -2 : -1;

        // FIXED: Only update if hover state actually changed
        if (newHoveredTool != lastHoveredTool ||
            (hoveringClearButton && hoveredTool != -2) ||
            (!hoveringClearButton && hoveredTool == -2))
        {
            // Store the previous hover state
            int previousHover = hoveredTool;

            // Update hover state
            hoveredTool = hoveringClearButton ? -2 : newHoveredTool;
            lastHoveredTool = newHoveredTool;

            // FIXED: Only invalidate specific regions instead of entire window
            if (previousHover >= 0 && previousHover < static_cast<int>(filteredTools.size())) {
                InvalidateToolRegion(previousHover);
            }
            if (hoveredTool >= 0 && hoveredTool < static_cast<int>(filteredTools.size())) {
                InvalidateToolRegion(hoveredTool);
            }

            // Update cursor and status
            if (hoveredTool >= 0 && hoveredTool < static_cast<int>(filteredTools.size()))
            {
                SetCursor(LoadCursor(NULL, IDC_HAND));
                if (statusBar)
                {
                    std::wstring displayName = filteredTools[hoveredTool].displayName;
                    std::replace(displayName.begin(), displayName.end(), L'_', L' ');
                    ConvertTopropercase(displayName);
                    std::wstring statusText = L"Click to launch: " + displayName;
                    SendMessage(statusBar, SB_SETTEXT, 0, (LPARAM)statusText.c_str());
                }
            }
            else if (hoveredTool == -2)
            {
                SetCursor(LoadCursor(NULL, IDC_HAND));
                if (statusBar)
                    SendMessage(statusBar, SB_SETTEXT, 0, (LPARAM)L"Clear search");
            }
            else
            {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                UpdateStatusText(L"Ready", static_cast<int>(filteredTools.size()));
            }
        }
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 5a. MOUSE LEAVE HANDLING - Prevent Stuck Hover States
    // ═══════════════════════════════════════════════════════════════
    case WM_MOUSELEAVE:
    {
        // Reset hover state when mouse leaves window
        if (hoveredTool >= 0 && hoveredTool < static_cast<int>(filteredTools.size())) {
            InvalidateToolRegion(hoveredTool);
        }
        hoveredTool = -1;
        lastHoveredTool = -1;
        isTrackingMouse = false;

        SetCursor(LoadCursor(NULL, IDC_ARROW));
        UpdateStatusText(L"Ready", static_cast<int>(filteredTools.size()));
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 6. ENHANCED BUTTON CLICK HANDLING
    // ═══════════════════════════════════════════════════════════════
    case WM_LBUTTONDOWN:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        selectedTool = GetToolAtPoint(pt);

        // Check clear button click
        RECT clearButtonRect;
        GetWindowRect(clearButton, &clearButtonRect);
        ScreenToClient(hwnd, (POINT*)&clearButtonRect.left);
        ScreenToClient(hwnd, (POINT*)&clearButtonRect.right);

        if (PtInRect(&clearButtonRect, pt))
        {
            selectedTool = -2; // Clear button selected
        }

        if (selectedTool >= 0 || selectedTool == -2)
        {
            // FIXED: Only invalidate the specific tool region
            if (selectedTool >= 0 && selectedTool < static_cast<int>(filteredTools.size())) {
                InvalidateToolRegion(selectedTool);
            }

            if (statusBar && selectedTool >= 0)
                SendMessage(statusBar, SB_SETTEXT, 0, (LPARAM)L"Launching...");
        }
        break;
    }

    case WM_LBUTTONUP:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int clickedTool = GetToolAtPoint(pt);

        // Handle clear button click
        RECT clearButtonRect;
        GetWindowRect(clearButton, &clearButtonRect);
        ScreenToClient(hwnd, (POINT*)&clearButtonRect.left);
        ScreenToClient(hwnd, (POINT*)&clearButtonRect.right);

        if (PtInRect(&clearButtonRect, pt) && selectedTool == -2)
        {
            SetWindowText(searchBox, L"");
            SetFocus(searchBox);
            FilterTools(L"");
            ShowWindow(clearButton, SW_HIDE);
            UpdateStatusText(L"Search cleared", static_cast<int>(filteredTools.size()));
        }
        else if (clickedTool >= 0 && clickedTool == selectedTool &&
            clickedTool < static_cast<int>(filteredTools.size()))
        {
            LaunchTool(clickedTool);
            if (statusBar)
            {
                std::wstring displayName = filteredTools[clickedTool].displayName;
                std::replace(displayName.begin(), displayName.end(), L'_', L' ');
                ConvertTopropercase(displayName);
                std::wstring launchMsg = L"Launched: " + displayName;
                SendMessage(statusBar, SB_SETTEXT, 0, (LPARAM)launchMsg.c_str());
                SetTimer(hwnd, 1, 3000, NULL);
            }
        }

        // FIXED: Clear selected state and invalidate only if needed
        if (selectedTool >= 0 && selectedTool < static_cast<int>(filteredTools.size())) {
            InvalidateToolRegion(selectedTool);
        }
        selectedTool = -1;
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 7. MODIFIED COMMAND HANDLING - Red Font Color Support
    // ═══════════════════════════════════════════════════════════════
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case 1001:  // Search box
            if (HIWORD(wParam) == EN_CHANGE)
            {
                // Use static buffer to avoid repeated allocations
                static wchar_t searchBuffer[256];
                GetWindowText(searchBox, searchBuffer, 256);

                bool hasText = searchBuffer[0] != L'\0';
                ShowWindow(clearButton, hasText ? SW_SHOW : SW_HIDE);

                FilterTools(searchBuffer);
                UpdateStatusText(hasText ? L"Search results" : L"Ready",
                    static_cast<int>(filteredTools.size()));

                // FIXED: Update scroll bars after filtering
                CalculateVirtualSize();
                UpdateScrollBars();
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;

        case 1005:  // Clear button
            SetWindowText(searchBox, L"");
            SetFocus(searchBox);
            FilterTools(L"");
            ShowWindow(clearButton, SW_HIDE);
            UpdateStatusText(L"Search cleared", static_cast<int>(filteredTools.size()));
            // FIXED: Update scroll bars after clearing
            CalculateVirtualSize();
            UpdateScrollBars();
            CalculateToolPositions();
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 8. ADDED COLOR CONTROL FOR CONTROLS
    // ═══════════════════════════════════════════════════════════════
    case WM_CTLCOLOREDIT:
    {
        HDC hdc = (HDC)wParam;
        HWND hwndControl = (HWND)lParam;

        if (hwndControl == searchBox)
        {
            SetTextColor(hdc, RGB(51, 51, 1)); // Red text
            SetBkColor(hdc, RGB(255, 255, 255)); // White background

            return (LRESULT)GetStockObject(WHITE_BRUSH);
        }
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        HWND hwndControl = (HWND)lParam;

        if (hwndControl == statusBar)
        {
            SetTextColor(hdc, RGB(64, 64, 64)); // Dark gray text
            SetBkColor(hdc, RGB(214, 226, 242)); // Light blue background
            return (LRESULT)CreateSolidBrush(RGB(214, 226, 242));
        }
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 9. ENHANCED KEYBOARD SHORTCUTS
    // ═══════════════════════════════════════════════════════════════
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_F5:  // Refresh
            ScanForTools();
            CalculateVirtualSize();
            UpdateScrollBars();
            CalculateToolPositions();
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateStatusText(L"Tools refreshed", static_cast<int>(filteredTools.size()));
            break;

        case VK_ESCAPE:  // Clear search
            SetWindowText(searchBox, L"");
            SetFocus(searchBox);
            FilterTools(L"");
            ShowWindow(clearButton, SW_HIDE);
            CalculateVirtualSize();
            UpdateScrollBars();
            CalculateToolPositions();
            UpdateStatusText(L"Search cleared", static_cast<int>(filteredTools.size()));
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case VK_RETURN:  // Quick launch
            if (!filteredTools.empty())
            {
                LaunchTool(0);
                if (statusBar)
                {
                    std::wstring displayName = filteredTools[0].displayName;
                    std::replace(displayName.begin(), displayName.end(), L'_', L' ');
                    ConvertTopropercase(displayName);
                    std::wstring launchMsg = L"Quick launched: " + displayName;
                    SendMessage(statusBar, SB_SETTEXT, 0, (LPARAM)launchMsg.c_str());
                    SetTimer(hwnd, 1, 3000, NULL);
                }
            }
            break;

        case 'F':  // Ctrl+F for search focus
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                SetFocus(searchBox);
                SendMessage(searchBox, EM_SETSEL, 0, -1); // Select all text
            }
            break;

        case VK_HOME:
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                scrollX = scrollY = 0;
                SetScrollPos(hwnd, SB_HORZ, scrollX, TRUE);
                SetScrollPos(hwnd, SB_VERT, scrollY, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;

        case VK_END:
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                scrollX = maxScrollX;
                scrollY = maxScrollY;
                SetScrollPos(hwnd, SB_HORZ, scrollX, TRUE);
                SetScrollPos(hwnd, SB_VERT, scrollY, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;

        case VK_PRIOR: // Page Up
            if (showVScrollBar) {
                scrollY = max(0, scrollY - 200);
                SetScrollPos(hwnd, SB_VERT, scrollY, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;

        case VK_NEXT: // Page Down
            if (showVScrollBar) {
                scrollY = min(maxScrollY, scrollY + 200);
                SetScrollPos(hwnd, SB_VERT, scrollY, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;

        case VK_LEFT:
            if (GetKeyState(VK_CONTROL) & 0x8000 && showHScrollBar) {
                scrollX = max(0, scrollX - 50);
                SetScrollPos(hwnd, SB_HORZ, scrollX, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;

        case VK_RIGHT:
            if (GetKeyState(VK_CONTROL) & 0x8000 && showHScrollBar) {
                scrollX = min(maxScrollX, scrollX + 50);
                SetScrollPos(hwnd, SB_HORZ, scrollX, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;

        case VK_UP:
            if (GetKeyState(VK_CONTROL) & 0x8000 && showVScrollBar) {
                scrollY = max(0, scrollY - 50);
                SetScrollPos(hwnd, SB_VERT, scrollY, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;

        case VK_DOWN:
            if (GetKeyState(VK_CONTROL) & 0x8000 && showVScrollBar) {
                scrollY = min(maxScrollY, scrollY + 50);
                SetScrollPos(hwnd, SB_VERT, scrollY, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
        break;
    }

    // ═══════════════════════════════════════════════════════════════
    // 10. TIMER FOR STATUS MESSAGES
    // ═══════════════════════════════════════════════════════════════
    case WM_TIMER:
        if (wParam == 1)
        {
            KillTimer(hwnd, 1);
            UpdateStatusText(L"Ready", static_cast<int>(filteredTools.size()));
        }
        break;

        // ═══════════════════════════════════════════════════════════════
        // 11. FOCUS MANAGEMENT
        // ═══════════════════════════════════════════════════════════════
    case WM_SETFOCUS:
        if (searchBox)
            SetFocus(searchBox);
        break;

        // ═══════════════════════════════════════════════════════════════
        // 12. PREVENT FLICKERING
        // ═══════════════════════════════════════════════════════════════
    case WM_ERASEBKGND:
        return 1;

        // ═══════════════════════════════════════════════════════════════
        // 13. WINDOW DESTRUCTION WITH PROPER CLEANUP
        // ═══════════════════════════════════════════════════════════════
    case WM_DESTROY:
        if (modernFont) {
            DeleteObject(modernFont);
            modernFont = NULL;
        }
        if (searchFont) {
            DeleteObject(searchFont);
            searchFont = NULL;
        }
        PostQuitMessage(0);
        break;

    case WM_HSCROLL:
        HandleHorizontalScroll(wParam);
        return 0;

    case WM_VSCROLL:
        HandleVerticalScroll(wParam);
        return 0;

    case WM_MOUSEWHEEL:
    {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        int scrollAmount = 60; // Adjust scroll sensitivity

        if (GetKeyState(VK_SHIFT) & 0x8000) {
            // Shift+wheel = horizontal scroll
            if (showHScrollBar) {
                scrollX = max(0, min(maxScrollX, scrollX - (delta > 0 ? scrollAmount : -scrollAmount)));
                SetScrollPos(hwnd, SB_HORZ, scrollX, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        else
        {
            // Normal wheel = vertical scroll
            if (showVScrollBar) {
                scrollY = max(0, min(maxScrollY, scrollY - (delta > 0 ? scrollAmount : -scrollAmount)));
                SetScrollPos(hwnd, SB_VERT, scrollY, TRUE);
                CalculateToolPositions();
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        return 0;
    }

    // ═══════════════════════════════════════════════════════════════
    // 14. DEFAULT MESSAGE HANDLING
    // ═══════════════════════════════════════════════════════════════
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATIC WINDOW PROCEDURE
// ═══════════════════════════════════════════════════════════════════════════════
LRESULT CALLBACK ToolLauncher::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ToolLauncher* launcher = nullptr;

    if (msg == WM_NCCREATE)
    {
        LPCREATESTRUCT pCreate = reinterpret_cast<LPCREATESTRUCT>(lParam);
        launcher = static_cast<ToolLauncher*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(launcher));
        if (launcher)
        {
            launcher->hwnd = hwnd;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    launcher = reinterpret_cast<ToolLauncher*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (launcher)
    {
        return launcher->HandleMessage(msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS - New Optimized Functions
// ═══════════════════════════════════════════════════════════════════════════════

// Optimized status text update
void ToolLauncher::UpdateStatusText(const std::wstring& message, int toolCount)
{
    if (!statusBar) return;

    std::wstring statusText = message + L" • " + std::to_wstring(toolCount) + L" Tools";
    SendMessage(statusBar, SB_SETTEXT, 0, (LPARAM)statusText.c_str());
}

// Optimized search icon drawing
void ToolLauncher::DrawSearchIcon(HDC hdc, int x, int y)
{
    HPEN iconPen = CreatePen(PS_SOLID, 2, RGB(120, 120, 120));
    HPEN oldPen = (HPEN)SelectObject(hdc, iconPen);

    // Draw magnifying glass circle
    SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
    Ellipse(hdc, x, y, x + 12, y + 12);

    // Draw handle
    MoveToEx(hdc, x + 9, y + 9, NULL);
    LineTo(hdc, x + 14, y + 14);

    SelectObject(hdc, oldPen);
    DeleteObject(iconPen);
}

// Convert to proper case function (existing)
void ToolLauncher::ConvertTopropercase(std::wstring& str)
{
    if (str.empty()) return;

    CharLowerBuffW(&str[0], static_cast<DWORD>(str.length()));

    BOOLEAN titlenext = true;
    for (size_t i = 0; i < str.length(); i++) {
        if (iswspace(str[i])) {
            titlenext = true;
        }
        else if (titlenext && iswalpha(str[i])) {
            WCHAR ch[2] = { str[i], L'\0' };
            CharUpperBuffW(ch, 1);
            str[i] = ch[0];
            titlenext = false;
        }
    }
}

// Calculate virtual content size
void ToolLauncher::CalculateVirtualSize()
{
    if (filteredTools.empty()) {
        virtualWidth = 0;
        virtualHeight = 0;
        return;
    }

    int startX = 32;
    int startY = HEADER_HEIGHT + SEARCH_BOX_HEIGHT + 70;

    if (viewMode == ViewMode::VIEW_GRID) {
        int rows = (filteredTools.size() + COLS_PER_ROW - 1) / COLS_PER_ROW;
        virtualWidth = startX + COLS_PER_ROW * (TOOL_BUTTON_SIZE + 16) + 32;
        virtualHeight = startY + rows * (TOOL_BUTTON_SIZE + 60) + 32;
    }
    else {
        virtualWidth = startX + 600 + 32;
        virtualHeight = startY + filteredTools.size() * 60 + 32;
    }
}

// Update scroll bar information
void ToolLauncher::UpdateScrollBars()
{
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    int clientWidth = clientRect.right;
    int clientHeight = clientRect.bottom;

    // Calculate if scroll bars are needed
    showHScrollBar = virtualWidth > clientWidth;
    showVScrollBar = virtualHeight > clientHeight;

    // Adjust for scroll bar thickness
    if (showVScrollBar) clientWidth -= GetSystemMetrics(SM_CXVSCROLL);
    if (showHScrollBar) clientHeight -= GetSystemMetrics(SM_CYHSCROLL);

    // Recalculate after adjustment
    showHScrollBar = virtualWidth > clientWidth;
    showVScrollBar = virtualHeight > clientHeight;

    // Set up horizontal scroll bar
    if (showHScrollBar) {
        maxScrollX = max(0, virtualWidth - clientWidth);
        scrollX = min(scrollX, maxScrollX);

        SCROLLINFO si = {};
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = virtualWidth - 1;
        si.nPage = clientWidth;
        si.nPos = scrollX;
        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
        ShowScrollBar(hwnd, SB_HORZ, TRUE);
    }
    else {
        scrollX = 0;
        maxScrollX = 0;
        ShowScrollBar(hwnd, SB_HORZ, FALSE);
    }

    // Set up vertical scroll bar
    if (showVScrollBar) {
        maxScrollY = max(0, virtualHeight - clientHeight);
        scrollY = min(scrollY, maxScrollY);

        SCROLLINFO si = {};
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = virtualHeight - 1;
        si.nPage = clientHeight;
        si.nPos = scrollY;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        ShowScrollBar(hwnd, SB_VERT, TRUE);
    }
    else {
        scrollY = 0;
        maxScrollY = 0;
        ShowScrollBar(hwnd, SB_VERT, FALSE);
    }
}

// Handle horizontal scroll messages
void ToolLauncher::HandleHorizontalScroll(WPARAM wParam)
{
    int oldScrollX = scrollX;

    switch (LOWORD(wParam)) {
    case SB_LEFT:
        scrollX = 0;
        break;
    case SB_RIGHT:
        scrollX = maxScrollX;
        break;
    case SB_LINELEFT:
        scrollX = max(0, scrollX - 20);
        break;
    case SB_LINERIGHT:
        scrollX = min(maxScrollX, scrollX + 20);
        break;
    case SB_PAGELEFT:
        scrollX = max(0, scrollX - 100);
        break;
    case SB_PAGERIGHT:
        scrollX = min(maxScrollX, scrollX + 100);
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        scrollX = HIWORD(wParam);
        scrollX = max(0, min(maxScrollX, scrollX));
        break;
    }

    if (scrollX != oldScrollX) {
        SetScrollPos(hwnd, SB_HORZ, scrollX, TRUE);
        CalculateToolPositions();
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

// Handle vertical scroll messages
void ToolLauncher::HandleVerticalScroll(WPARAM wParam)
{
    int oldScrollY = scrollY;

    switch (LOWORD(wParam)) {
    case SB_TOP:
        scrollY = 0;
        break;
    case SB_BOTTOM:
        scrollY = maxScrollY;
        break;
    case SB_LINEUP:
        scrollY = max(0, scrollY - 20);
        break;
    case SB_LINEDOWN:
        scrollY = min(maxScrollY, scrollY + 20);
        break;
    case SB_PAGEUP:
        scrollY = max(0, scrollY - 100);
        break;
    case SB_PAGEDOWN:
        scrollY = min(maxScrollY, scrollY + 100);
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        scrollY = HIWORD(wParam);
        scrollY = max(0, min(maxScrollY, scrollY));
        break;
    }

    if (scrollY != oldScrollY) {
        SetScrollPos(hwnd, SB_VERT, scrollY, TRUE);
        CalculateToolPositions();
        InvalidateRect(hwnd, NULL, TRUE);
    }
}
void ToolLauncher::InvalidateToolRegion(int toolId) {
    // Example implementation - adjust based on your tool layout
    RECT toolRect;

    // Calculate the rectangle for the specific tool
    // This depends on your UI layout - you'll need to adjust this
    int toolWidth = 50;  // Example width
    int toolHeight = 50; // Example height
    int spacing = 10;    // Example spacing

    toolRect.left = toolId * (toolWidth + spacing);
    toolRect.top = 0;
    toolRect.right = toolRect.left + toolWidth;
    toolRect.bottom = toolRect.top + toolHeight;

    // Invalidate the specific region
    InvalidateRect(hwnd, &toolRect, TRUE);
}