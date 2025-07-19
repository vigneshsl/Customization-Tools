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
// Purpose: Responsible for drawing tool launcher UI elements using GDI+
//          This includes headers, tool buttons, icons, and labels.
///////////////////////////////////////////////////////////////////////////////
class ToolRenderer
{
public:
    // Constructor: Requires a reference to ToolLauncher (for layout, state, etc.)
    ToolRenderer(ToolLauncher* launcher);

    // Destructor: Currently no dynamic cleanup needed
    ~ToolRenderer();

    ///////////////////////////////////////////////////////////////////////////
    // Public Drawing Methods
    ///////////////////////////////////////////////////////////////////////////

    // Draws the top header (title + subtitle with tool count)
    void DrawHeader(HDC hdc, const RECT& clientRect, int toolCount);

    // Draws a single tool button (icon, name, and hover effect)
    void DrawTool(HDC hdc, const ToolInfo& tool, int index, bool isHovered);

private:
    ToolLauncher* toolLauncher;  // Reference to parent window/controller

    ///////////////////////////////////////////////////////////////////////////
    // Private Helper Methods (Internally used by DrawTool and DrawHeader)
    ///////////////////////////////////////////////////////////////////////////

    // Draws the tool icon (bitmap) or fallback if missing
    void DrawToolIcon(HDC hdc, const ToolInfo& tool, const RECT& rect);

    // Converts a string to Proper Case (title format)
    void ConvertTopropercase(std::wstring& str);

    // Draws the tool name below the icon (handles wrapping and formatting)
    void DrawToolName(HDC hdc, const ToolInfo& tool, const RECT& rect);

    // [Optional] Draws a badge showing file extension like ".EXE" in corner
    void DrawExtensionBadge(HDC hdc, const ToolInfo& tool, const RECT& rect);

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
