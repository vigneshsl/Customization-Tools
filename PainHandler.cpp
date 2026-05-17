#include "Main.h"
#include "ToolRenderer.h"
using namespace Gdiplus;

///////////////////////////////////////////////////////////////////////////
// Function   : ToolLauncher::OnPaint
// Purpose    : Handles the WM_PAINT message by rendering the window UI.
//              Uses double buffering and GDI+ for smooth graphics.
///////////////////////////////////////////////////////////////////////////
void ToolLauncher::OnPaint(HDC hdc)
{
    // Get the size of the client area (inside the window, excluding title bar)
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    // If window size changed, update the memory buffer to match new dimensions
    if (clientRect.right != lastWidth || clientRect.bottom != lastHeight) {
        UpdateDoubleBuffer(clientRect.right, clientRect.bottom);
    }

    // Use memory device context for flicker-free offscreen drawing
    HDC hdcMem = hBufferDC;

    // Fill entire background using a pre-selected brush (solid or pattern)
    FillRect(hdcMem, &clientRect, backgroundBrush);

    //----------------------------------------------
    // 1. DRAW THE HEADER (TOP SECTION)
    //----------------------------------------------
    Graphics graphics(hdcMem);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

    // Modern gradient header — soft blue-purple to white fade
    LinearGradientBrush gradientBrush(
        Point(0, 0), Point(clientRect.right, HEADER_HEIGHT),
        Color(255, 230, 238, 255),    // Soft lavender-blue (left)
        Color(255, 248, 240, 252)     // Soft pink-white (right)
    );
    graphics.FillRectangle(&gradientBrush, 0, 0, clientRect.right, HEADER_HEIGHT);

    // Draw a subtle separator line below header
    Pen separatorPen(Color(30, 100, 100, 180), 1.0f);
    graphics.DrawLine(&separatorPen, 0, HEADER_HEIGHT - 1, clientRect.right, HEADER_HEIGHT - 1);

    // Set background mode for text to transparent (no solid background)
    SetBkMode(hdcMem, TRANSPARENT);

    //----------------------------------------------
    // 2. DRAW HEADER TITLE ("Customization Tools")
    //----------------------------------------------
    // Use pre-created header font (Segoe UI 32pt SemiBold)
    HFONT oldFont = (HFONT)SelectObject(hdcMem, headerFont);

    // Draw main title
    SetTextColor(hdcMem, RGB(35, 40, 60));  // Dark navy for title
    RECT titleRect = { 30, 12, clientRect.right - 30, 50 };
    std::wstring titleText = L"Customization Tools";
    DrawText(hdcMem, titleText.c_str(), -1, &titleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    //----------------------------------------------
    // 3. DRAW SUBTITLE (tool count)
    //----------------------------------------------
    SetTextColor(hdcMem, TOOLS_AVAILABLE_COLOR);
    SelectObject(hdcMem, subtitleFont);

    RECT subtitleRect = { 30, 48, clientRect.right - 30, HEADER_HEIGHT - 5 };
    std::wstring subtitleText = std::to_wstring(filteredTools.size()) + L" Tools Available";
    DrawText(hdcMem, subtitleText.c_str(), -1, &subtitleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Restore old font
    SelectObject(hdcMem, oldFont);

    //----------------------------------------------
    // 4. DRAW ALL TOOLS (icons and labels)
    //----------------------------------------------
    for (size_t i = 0; i < filteredTools.size(); ++i)
    {
        // Check if mouse is hovering over this tool
        bool isHovered = (hoveredTool == static_cast<int>(i));

        // Delegate the tool drawing to ToolRenderer
        renderer->DrawTool(hdcMem, filteredTools[i], static_cast<int>(i), isHovered);
    }

    //----------------------------------------------
    // 5. COPY TO SCREEN (BitBlt for smooth final paint)
    //----------------------------------------------
    BitBlt(
        hdc,                  // Target: actual screen device context
        0, 0,                 // Destination X, Y
        clientRect.right,     // Width
        clientRect.bottom,    // Height
        hdcMem,               // Source: our memory DC
        0, 0,                 // Source X, Y
        SRCCOPY               // Copy operation
    );
}

///////////////////////////////////////////////////////////////////////////
// Function   : ToolLauncher::UpdateDoubleBuffer
// Purpose    : Recreates the memory buffer used for offscreen drawing.
//              Called when window size changes.
///////////////////////////////////////////////////////////////////////////
void ToolLauncher::UpdateDoubleBuffer(int width, int height)
{
    // Free any existing buffer to avoid memory leaks
    CleanupDoubleBuffer();

    // Get a handle to the device context of the screen
    HDC hdc = GetDC(hwnd);

    // Create a memory-compatible device context (offscreen drawing)
    hBufferDC = CreateCompatibleDC(hdc);

    // Create a bitmap compatible with the screen
    hBufferBitmap = CreateCompatibleBitmap(hdc, width, height);

    // Attach the bitmap to the memory device context
    SelectObject(hBufferDC, hBufferBitmap);

    // Store the new dimensions for future resize checks
    lastWidth = width;
    lastHeight = height;

    // Release the screen device context
    ReleaseDC(hwnd, hdc);
}

///////////////////////////////////////////////////////////////////////////
// Function   : ToolLauncher::CleanupDoubleBuffer
// Purpose    : Frees the memory buffer and associated bitmap.
//              Should be called before resizing or on destruction.
///////////////////////////////////////////////////////////////////////////
void ToolLauncher::CleanupDoubleBuffer()
{
    // Delete the offscreen bitmap
    if (hBufferBitmap)
    {
        DeleteObject(hBufferBitmap);
        hBufferBitmap = nullptr;
    }

    // Delete the memory device context
    if (hBufferDC)
    {
        DeleteDC(hBufferDC);
        hBufferDC = nullptr;
    }
}
