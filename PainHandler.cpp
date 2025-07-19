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
    RECT headerRect = { 0, 0, clientRect.right, HEADER_HEIGHT };

    // Create a GDI+ graphics object for advanced drawing (gradients, antialiasing)
    Graphics graphics(hdcMem);

    // Create a vertical gradient brush from white to light gray
    LinearGradientBrush gradientBrush(
        Point(0, 0), Point(0, HEADER_HEIGHT),
        Color(39, 245, 91, 204),     // Top gradient color (custom shade)
        Color(250, 249, 248, 255)    // Bottom gradient color (off-white)
    );

    // Fill the header rectangle with the gradient
    graphics.FillRectangle(&gradientBrush, 0, 0, clientRect.right, HEADER_HEIGHT);

    // Set background mode for text to transparent (no solid background)
    SetBkMode(hdcMem, TRANSPARENT);

    // Set main header text color (predefined Win11 color)
    SetTextColor(hdcMem, win11_text);

    // Use previously created header font (e.g., Segoe UI 32pt)
    HFONT oldFont = (HFONT)SelectObject(hdcMem, headerFont);

    //----------------------------------------------
    // 2. DRAW SUBTITLE TEXT (e.g., "Customization Tools")
    //----------------------------------------------

    // Create a subtitle font (23pt Times New Roman)
    HFONT subtitleFont = CreateFont(
        23, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman"
    );

    // Change text color to predefined subtitle color
    SetTextColor(hdcMem, TOOLS_AVAILABLE_COLOR);

    // Switch to subtitle font
    SelectObject(hdcMem, subtitleFont);

    // Define rectangle area to draw the subtitle string
    RECT subtitleRect = { 30, 50, clientRect.right - 30, HEADER_HEIGHT - 5 };

    // Draw the subtitle text
    std::wstring subtitleText = L"Customization Tools";
    DrawText(hdcMem, subtitleText.c_str(), -1, &subtitleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Restore old font and delete temporary subtitle font
    SelectObject(hdcMem, oldFont);
    DeleteObject(subtitleFont);

    //----------------------------------------------
    // 3. DRAW ALL TOOLS (icons and labels)
    //----------------------------------------------
    for (size_t i = 0; i < filteredTools.size(); ++i)
    {
        // Check if mouse is hovering over this tool
        bool isHovered = (hoveredTool == static_cast<int>(i));

        // Delegate the tool drawing to ToolRenderer
        renderer->DrawTool(hdcMem, filteredTools[i], static_cast<int>(i), isHovered);
    }

    //----------------------------------------------
    // 4. COPY TO SCREEN (BitBlt for smooth final paint)
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
