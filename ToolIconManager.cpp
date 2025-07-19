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
// Purpose    : Creates a 64x64 custom icon bitmap for the tool based on its
//              file extension or name.
// Returns    : HBITMAP handle to the created icon image.
///////////////////////////////////////////////////////////////////////////
HBITMAP ToolIconManager::CreateToolIcon(const std::wstring& extension, const std::wstring& toolName) {
    // Get screen device context (DC) for bitmap compatibility
    HDC hdc = GetDC(NULL);

    // Create a memory device context (offscreen drawing)
    HDC memDC = CreateCompatibleDC(hdc);

    // Create a 64x64 bitmap compatible with screen DC
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, 64, 64);

    // Select the bitmap into the memory DC
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    //-----------------------------------------------
    // Step 1: Fill icon background
    //-----------------------------------------------
    HBRUSH brush = CreateSolidBrush(win11_background);   // Create background brush (Win11 theme color)
    HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, brush);
    PatBlt(memDC, 0, 0, 64, 64, PATCOPY);                // Fill the whole 64x64 area
    SelectObject(memDC, oldBrush);                       // Restore previous brush
    DeleteObject(brush);                                 // Clean up brush

    //-----------------------------------------------
    // Step 2: Draw emoji/symbol text on icon
    //-----------------------------------------------
    DrawIconText(memDC, extension);

    //-----------------------------------------------
    // Step 3: Final cleanup and return
    //-----------------------------------------------
    SelectObject(memDC, oldBitmap);                      // Restore previous bitmap
    DeleteDC(memDC);                                     // Free memory DC
    ReleaseDC(NULL, hdc);                                // Release screen DC

    return hBitmap;                                      // Return the created icon bitmap
}

///////////////////////////////////////////////////////////////////////////
// Function   : GetIconBrush
// Purpose    : Returns a solid color brush based on the file extension.
//              Used for category-based background coloring.
///////////////////////////////////////////////////////////////////////////
HBRUSH ToolIconManager::GetIconBrush(const std::wstring& extension) {
    if (extension == L".py") {
        return CreateSolidBrush(RGB(52, 144, 220));    // Python = blue
    }
    else if (extension == L".bat") {
        return CreateSolidBrush(RGB(72, 72, 72));      // BAT = gray
    }
    else if (extension == L".exe") {
        return CreateSolidBrush(RGB(0, 120, 215));     // EXE = blue
    }
    else if (extension == L".ps1") {
        return CreateSolidBrush(RGB(1, 36, 86));       // PowerShell = dark blue
    }
    else {
        return CreateSolidBrush(RGB(96, 94, 92));      // Default = neutral gray
    }
}

///////////////////////////////////////////////////////////////////////////
// Function   : DrawIconText
// Purpose    : Draws a symbol or emoji on the icon using "Segoe UI Emoji" font
//              centered in the 64x64 space.
///////////////////////////////////////////////////////////////////////////
void ToolIconManager::DrawIconText(HDC memDC, const std::wstring& extension) {
    SetBkMode(memDC, TRANSPARENT);                     // No background behind text
    SetTextColor(memDC, RGB(0, 153, 51));              // Green color text

    // Create emoji-capable font (Segoe UI Emoji)
    HFONT iconFont = CreateFont(
        44, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI Emoji"
    );

    // Select the emoji font
    HFONT oldFont = (HFONT)SelectObject(memDC, iconFont);

    // Get the character to draw (emoji or extension)
    std::wstring displayText = GetIconSymbol(extension);

    // Center the emoji in the 64x64 bitmap
    RECT textRect = { 0, 12, 64, 52 };
    DrawText(memDC, displayText.c_str(), -1, &textRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Clean up font
    SelectObject(memDC, oldFont);
    DeleteObject(iconFont);
}

///////////////////////////////////////////////////////////////////////////
// Function   : IsEmojiSymbol
// Purpose    : Returns true if the given text is a known emoji symbol.
// Note       : Only checks known ones used in this project.
///////////////////////////////////////////////////////////////////////////
bool ToolIconManager::IsEmojiSymbol(const std::wstring& text) {
    return (text == L"👽" || text == L"⚡");
}

///////////////////////////////////////////////////////////////////////////
// Function   : GetIconSymbol
// Purpose    : Returns an emoji or fallback text to be used for a tool icon.
//              Uses emojis for known extensions, or shows text for others.
///////////////////////////////////////////////////////////////////////////
std::wstring ToolIconManager::GetIconSymbol(const std::wstring& extension) {
    if (extension == L".py") {
        return L"👽";       // Python = alien emoji
    }
    else if (extension == L".bat") {
        return L"⚡";       // BAT = lightning bolt emoji
    }
    else {
        // For other extensions, show uppercase text like "TXT" or "DLL"
        if (extension.length() > 1 && extension[0] == L'.') {
            std::wstring result = extension.substr(1); // Remove the dot
            std::transform(result.begin(), result.end(), result.begin(), ::towupper);
            return result;
        }
        return extension;
    }
}
