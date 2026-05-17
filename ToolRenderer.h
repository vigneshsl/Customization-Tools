#pragma once

// Include core definitions (ToolInfo, constants, etc.)
#include "Main.h"

// Include necessary Windows and GDI+ headers
#include <windows.h>
#include <gdiplus.h>  // GDI+ for advanced graphics (gradients, anti-aliasing)
using namespace Gdiplus;

// Forward declaration of the ToolLauncher class to avoid circular dependency
class ToolLauncher;

///////////////////////////////////////////////////////////////////////////////
// Class: ToolRenderer
// Purpose: Responsible for drawing premium tool launcher UI elements using
//          GDI+. Renders modern card layouts with shadows, rounded corners,
//          hover elevation, accent borders, and formatted text.
///////////////////////////////////////////////////////////////////////////////
class ToolRenderer
{
public:
    // Constructor: Requires a reference to ToolLauncher (for layout, state, etc.)
    ToolRenderer(ToolLauncher* launcher);

    // Destructor: Cleans up cached GDI font objects
    ~ToolRenderer();

    ///////////////////////////////////////////////////////////////////////////
    // Public Drawing Methods
    ///////////////////////////////////////////////////////////////////////////

    // Draws a single tool button (icon, name, hover effect with elevation)
    void DrawTool(HDC hdc, const ToolInfo& tool, int index, bool isHovered);

private:
    ToolLauncher* toolLauncher;  // Reference to parent window/controller
    HFONT nameFont = nullptr;    // Cached font for tool name drawing

    ///////////////////////////////////////////////////////////////////////////
    // Private Helper Methods (Internally used by DrawTool)
    ///////////////////////////////////////////////////////////////////////////

    // Draws the tool icon (bitmap) or fallback if missing
    void DrawToolIcon(HDC hdc, const ToolInfo& tool, const RECT& rect);

    // Draws the tool name below the icon (handles wrapping and formatting)
    void DrawToolName(HDC hdc, const ToolInfo& tool, const RECT& rect);

    ///////////////////////////////////////////////////////////////////////////
    // Rounded Rectangle Drawing Helpers (GDI+ based)
    ///////////////////////////////////////////////////////////////////////////

    // Draws and fills a rectangle with rounded corners
    void FillRoundedRectangle(Graphics* graphics, Brush* brush,
        INT x, INT y, INT width, INT height, INT radius);

    // Draws a border around a rounded rectangle
    void DrawRoundedRectangle(Graphics* graphics, Pen* pen,
        INT x, INT y, INT width, INT height, INT radius);
};
