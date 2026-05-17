#include "ToolIconManager.h"
#include <algorithm>
#include "Main.h"

///////////////////////////////////////////////////////////////////////////
// Constructor: ToolIconManager
// Purpose    : Initializes the icon manager (currently no setup needed)
///////////////////////////////////////////////////////////////////////////
ToolIconManager::ToolIconManager() {}

///////////////////////////////////////////////////////////////////////////
// Destructor: ~ToolIconManager
// Purpose    : Cleans up icon resources (done in ToolInfo, so nothing here)
///////////////////////////////////////////////////////////////////////////
ToolIconManager::~ToolIconManager() {}

///////////////////////////////////////////////////////////////////////////
// Function   : CreateToolIcon
// Purpose    : Creates a modern 64x64 icon bitmap with gradient background,
//              rounded appearance, and bold extension text label.
// Returns    : HBITMAP handle to the created icon image.
///////////////////////////////////////////////////////////////////////////
HBITMAP ToolIconManager::CreateToolIcon(const std::wstring& extension, const std::wstring& toolName) {
    // Get screen device context (DC) for bitmap compatibility
    HDC hdc = GetDC(NULL);

    // Create a memory device context (offscreen drawing)
    HDC memDC = CreateCompatibleDC(hdc);

    // Create a 64x64 bitmap compatible with screen DC
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, TOOL_ICON_SIZE, TOOL_ICON_SIZE);

    // Select the bitmap into the memory DC
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    //-----------------------------------------------
    // Step 1: Draw gradient background
    //-----------------------------------------------
    DrawIconBackground(memDC, extension);

    //-----------------------------------------------
    // Step 2: Draw extension label text on icon
    //-----------------------------------------------
    DrawIconText(memDC, extension);

    //-----------------------------------------------
    // Step 3: Draw subtle inner highlight for depth
    //-----------------------------------------------
    DrawIconHighlight(memDC);

    //-----------------------------------------------
    // Step 4: Final cleanup and return
    //-----------------------------------------------
    SelectObject(memDC, oldBitmap);                      // Restore previous bitmap
    DeleteDC(memDC);                                     // Free memory DC
    ReleaseDC(NULL, hdc);                                // Release screen DC

    return hBitmap;                                      // Return the created icon bitmap
}

///////////////////////////////////////////////////////////////////////////
// Function   : DrawIconBackground
// Purpose    : Fills the icon with a modern gradient background using
//              GDI+ for smooth color transitions per file extension.
///////////////////////////////////////////////////////////////////////////
void ToolIconManager::DrawIconBackground(HDC memDC, const std::wstring& extension) {
    Graphics graphics(memDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    // Get extension-specific gradient colors
    Color topColor, bottomColor;
    GetGradientColors(extension, topColor, bottomColor);

    // Draw rounded rectangle background with gradient
    LinearGradientBrush gradientBrush(
        Point(0, 0), Point(0, TOOL_ICON_SIZE),
        topColor, bottomColor
    );

    // Fill with rounded corners using GraphicsPath
    GraphicsPath path;
    int r = 10; // Corner radius
    int w = TOOL_ICON_SIZE;
    int h = TOOL_ICON_SIZE;
    path.AddArc(0, 0, r * 2, r * 2, 180, 90);
    path.AddArc(w - r * 2, 0, r * 2, r * 2, 270, 90);
    path.AddArc(w - r * 2, h - r * 2, r * 2, r * 2, 0, 90);
    path.AddArc(0, h - r * 2, r * 2, r * 2, 90, 90);
    path.CloseFigure();

    graphics.FillPath(&gradientBrush, &path);
}

///////////////////////////////////////////////////////////////////////////
// Function   : DrawIconHighlight
// Purpose    : Adds a subtle semi-transparent highlight at the top of the
//              icon for a glossy 3D effect.
///////////////////////////////////////////////////////////////////////////
void ToolIconManager::DrawIconHighlight(HDC memDC) {
    Graphics graphics(memDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    // Semi-transparent white highlight on top half
    LinearGradientBrush highlightBrush(
        Point(0, 0), Point(0, TOOL_ICON_SIZE / 2),
        Color(45, 255, 255, 255),   // Subtle white at top
        Color(0, 255, 255, 255)     // Fully transparent at middle
    );

    // Highlight only the top portion with rounded corners
    GraphicsPath highlightPath;
    int r = 10;
    int w = TOOL_ICON_SIZE;
    int h = TOOL_ICON_SIZE / 2;
    highlightPath.AddArc(1, 1, r * 2, r * 2, 180, 90);
    highlightPath.AddArc(w - r * 2 - 1, 1, r * 2, r * 2, 270, 90);
    highlightPath.AddLine(w - 1, h, 1, h);
    highlightPath.CloseFigure();

    graphics.FillPath(&highlightBrush, &highlightPath);
}

///////////////////////////////////////////////////////////////////////////
// Function   : GetGradientColors
// Purpose    : Returns modern gradient color pairs for each file extension.
//              Uses rich, saturated colors for a premium look.
///////////////////////////////////////////////////////////////////////////
void ToolIconManager::GetGradientColors(const std::wstring& extension, Color& topColor, Color& bottomColor) {
    if (extension == L".py") {
        // Python — rich blue to deep navy
        topColor = Color(255, 55, 118, 210);
        bottomColor = Color(255, 25, 60, 140);
    }
    else if (extension == L".bat") {
        // Batch — charcoal to near-black
        topColor = Color(255, 75, 80, 90);
        bottomColor = Color(255, 40, 42, 48);
    }
    else if (extension == L".exe") {
        // Executable — vibrant teal to deep teal
        topColor = Color(255, 0, 150, 180);
        bottomColor = Color(255, 0, 95, 130);
    }
    else if (extension == L".ps1") {
        // PowerShell — royal blue to deep indigo
        topColor = Color(255, 60, 80, 180);
        bottomColor = Color(255, 30, 40, 120);
    }
    else {
        // Default — neutral dark gray gradient
        topColor = Color(255, 100, 100, 110);
        bottomColor = Color(255, 60, 60, 68);
    }
}

///////////////////////////////////////////////////////////////////////////
// Function   : GetIconBrush
// Purpose    : Returns a solid color brush based on the file extension.
//              Kept for backwards compatibility but gradient is preferred.
///////////////////////////////////////////////////////////////////////////
HBRUSH ToolIconManager::GetIconBrush(const std::wstring& extension) {
    if (extension == L".py") {
        return CreateSolidBrush(RGB(55, 118, 210));    // Python = rich blue
    }
    else if (extension == L".bat") {
        return CreateSolidBrush(RGB(75, 80, 90));      // BAT = charcoal
    }
    else if (extension == L".exe") {
        return CreateSolidBrush(RGB(0, 150, 180));     // EXE = teal
    }
    else if (extension == L".ps1") {
        return CreateSolidBrush(RGB(60, 80, 180));     // PowerShell = royal blue
    }
    else {
        return CreateSolidBrush(RGB(100, 100, 110));   // Default = neutral gray
    }
}

///////////////////////////////////////////////////////////////////////////
// Function   : DrawIconText
// Purpose    : Draws a clean, bold uppercase extension label centered in
//              the icon. Uses "Segoe UI Variable" for crisp rendering.
///////////////////////////////////////////////////////////////////////////
void ToolIconManager::DrawIconText(HDC memDC, const std::wstring& extension) {
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));            // White text for contrast

    // Create a bold, clean font for the extension label
    HFONT iconFont = CreateFont(
        22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI Variable"
    );

    // Select the font
    HFONT oldFont = (HFONT)SelectObject(memDC, iconFont);

    // Get the label text (uppercase extension name)
    std::wstring displayText = GetIconSymbol(extension);

    // Center the text in the 64x64 bitmap
    RECT textRect = { 0, 0, TOOL_ICON_SIZE, TOOL_ICON_SIZE };
    DrawText(memDC, displayText.c_str(), -1, &textRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Clean up font
    SelectObject(memDC, oldFont);
    DeleteObject(iconFont);
}

///////////////////////////////////////////////////////////////////////////
// Function   : IsEmojiSymbol
// Purpose    : Returns true if the given text is an emoji symbol.
//              No longer used since we switched to text labels, kept for
//              backwards compatibility.
///////////////////////////////////////////////////////////////////////////
bool ToolIconManager::IsEmojiSymbol(const std::wstring& text) {
    return false;  // No emojis used in current design
}

///////////////////////////////////////////////////////////////////////////
// Function   : GetIconSymbol
// Purpose    : Returns a clean uppercase extension label for the icon.
//              E.g., ".py" → "PY", ".bat" → "BAT"
///////////////////////////////////////////////////////////////////////////
std::wstring ToolIconManager::GetIconSymbol(const std::wstring& extension) {
    // Remove the dot and uppercase the extension
    if (extension.length() > 1 && extension[0] == L'.') {
        std::wstring result = extension.substr(1);
        std::transform(result.begin(), result.end(), result.begin(), ::towupper);
        return result;
    }
    return extension;
}
