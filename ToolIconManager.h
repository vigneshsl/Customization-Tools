#pragma once

#include <windows.h>     // Windows API for GDI drawing
#include <string>        // For using std::wstring (Unicode text)
#include <gdiplus.h>     // GDI+ for gradient and anti-aliased drawing
using namespace Gdiplus;

//------------------------------------------------------------------------------
// Class: ToolIconManager
// Purpose: Generates modern 64x64 bitmap icons dynamically with gradient
//          backgrounds, clean text labels, and glossy effects per extension.
//------------------------------------------------------------------------------
class ToolIconManager {
public:
    // Constructor - sets up any default configurations (if needed)
    ToolIconManager();

    // Destructor - currently nothing to clean, but safe practice
    ~ToolIconManager();

    //-------------------------------------------------------------------------
    // Function: CreateToolIcon
    // Purpose : Creates a modern bitmap icon with gradient background + text
    // Params  : extension - file type (e.g., ".exe")
    //           toolName - name of the tool (for future customization)
    // Returns : HBITMAP - handle to icon bitmap for display
    //-------------------------------------------------------------------------
    HBITMAP CreateToolIcon(const std::wstring& extension, const std::wstring& toolName);

private:
    //-------------------------------------------------------------------------
    // Function: DrawIconBackground
    // Purpose : Fills icon with a gradient background using GDI+
    //-------------------------------------------------------------------------
    void DrawIconBackground(HDC memDC, const std::wstring& extension);

    //-------------------------------------------------------------------------
    // Function: DrawIconHighlight
    // Purpose : Adds a subtle glossy highlight effect on top of the icon
    //-------------------------------------------------------------------------
    void DrawIconHighlight(HDC memDC);

    //-------------------------------------------------------------------------
    // Function: GetGradientColors
    // Purpose : Returns top and bottom gradient colors for each extension
    //-------------------------------------------------------------------------
    void GetGradientColors(const std::wstring& extension, Color& topColor, Color& bottomColor);

    //-------------------------------------------------------------------------
    // Function: GetIconBrush
    // Purpose : Returns a colored brush based on file extension (legacy)
    //-------------------------------------------------------------------------
    HBRUSH GetIconBrush(const std::wstring& extension);

    //-------------------------------------------------------------------------
    // Function: DrawIconText
    // Purpose : Renders clean bold text label (e.g., "PY", "BAT") on the icon
    //-------------------------------------------------------------------------
    void DrawIconText(HDC memDC, const std::wstring& extension);

    //-------------------------------------------------------------------------
    // Function: IsEmojiSymbol
    // Purpose : Legacy check — always returns false (emojis removed)
    //-------------------------------------------------------------------------
    bool IsEmojiSymbol(const std::wstring& text);

    //-------------------------------------------------------------------------
    // Function: GetIconSymbol
    // Purpose : Returns uppercase extension label (e.g., ".py" → "PY")
    //-------------------------------------------------------------------------
    std::wstring GetIconSymbol(const std::wstring& extension);
};
