#include "ToolRenderer.h"
#include <gdiplus.h>
using namespace Gdiplus;

// Constructor - stores the pointer to the ToolLauncher
ToolRenderer::ToolRenderer(ToolLauncher* launcher) : toolLauncher(launcher) {}

// Destructor
ToolRenderer::~ToolRenderer() {}

//////////////////////////////////////////////////////////////////////
// Function : DrawHeader
// Purpose  : Draws the window header with gradient background, title, and tool count
//////////////////////////////////////////////////////////////////////
void ToolRenderer::DrawHeader(HDC hdc, const RECT& clientRect, int toolCount) {
    Graphics graphics(hdc);  // GDI+ graphics for advanced rendering

    // Draw gradient background in header area
    LinearGradientBrush gradientBrush(
        Point(0, 0), Point(0, HEADER_HEIGHT),
        Color(255, 255, 255, 255), Color(250, 249, 248, 255)
    );
    graphics.FillRectangle(&gradientBrush, 0, 0, clientRect.right, HEADER_HEIGHT);

    // Set transparent background for text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, win11_text);

    // Create and select header font
    HFONT headerFont = CreateFont(32, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable");
    HFONT oldFont = (HFONT)SelectObject(hdc, headerFont);

    // Draw title text
    std::wstring headerText = L"Tool Launcher";
    RECT headerTextRect = { 30, 15, clientRect.right - 30, 50 };
    DrawText(hdc, headerText.c_str(), -1, &headerTextRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Set secondary text color and font
    SetTextColor(hdc, win11_text_secondary);
    HFONT subtitleFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");
    SelectObject(hdc, subtitleFont);

    // Draw subtitle (tool count)
    std::wstring subtitleText = std::to_wstring(toolCount) + L" Tools available";
    RECT subtitleRect = { 30, 50, clientRect.right - 30, HEADER_HEIGHT - 5 };
    DrawText(hdc, subtitleText.c_str(), -1, &subtitleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Restore and delete fonts
    SelectObject(hdc, oldFont);
    DeleteObject(headerFont);
    DeleteObject(subtitleFont);
}

//////////////////////////////////////////////////////////////////////
// Function : DrawTool
// Purpose  : Draws a tool card with icon and name
//////////////////////////////////////////////////////////////////////
void ToolRenderer::DrawTool(HDC hdc, const ToolInfo& tool, int index, bool isHovered) {
    RECT rect = tool.rect;  // Get tool position

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

    // Color setup based on hover state
    Color fillColor = isHovered ? Color(246, 246, 246, 255) : Color(255, 255, 255, 255);
    Color borderColor = isHovered ? Color(25, 102, 255) : Color(225, 223, 221, 255);

    // Draw tool card shadow
    SolidBrush shadowBrush(Color(20, 0, 0, 0));
    FillRoundedRectangle(&graphics, &shadowBrush, rect.left + 2, rect.top + 2,
        TOOL_BUTTON_SIZE, TOOL_BUTTON_SIZE, 8);

    // Draw tool card fill and border
    SolidBrush fillBrush(fillColor);
    Pen borderPen(borderColor, 2.0f);
    FillRoundedRectangle(&graphics, &fillBrush, rect.left, rect.top,
        TOOL_BUTTON_SIZE, TOOL_BUTTON_SIZE, 8);
    DrawRoundedRectangle(&graphics, &borderPen, rect.left, rect.top,
        TOOL_BUTTON_SIZE - 1, TOOL_BUTTON_SIZE - 1, 8);

    // Draw icon and name
    DrawToolIcon(hdc, tool, rect);
    DrawToolName(hdc, tool, rect);
}

//////////////////////////////////////////////////////////////////////
// Function : DrawToolIcon
// Purpose  : Draws icon or red placeholder if missing
//////////////////////////////////////////////////////////////////////
void ToolRenderer::DrawToolIcon(HDC hdc, const ToolInfo& tool, const RECT& rect) {
    if (tool.icon) {
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, tool.icon);
        int iconX = rect.left + (TOOL_BUTTON_SIZE - 64) / 2;
        int iconY = rect.top + 15;
        BitBlt(hdc, iconX, iconY, 64, 64, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBitmap);
        DeleteDC(memDC);
    }
    else {
        // Red box placeholder with label
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
        RECT iconRect = { rect.left + (TOOL_BUTTON_SIZE - 64) / 2, rect.top + 15,
                          rect.left + (TOOL_BUTTON_SIZE - 64) / 2 + 64, rect.top + 79 };
        FillRect(hdc, &iconRect, redBrush);
        DeleteObject(redBrush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        DrawText(hdc, L"NO ICON", -1, &iconRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

//////////////////////////////////////////////////////////////////////
// Function : ConvertTopropercase
// Purpose  : Changes string to Title Case
//////////////////////////////////////////////////////////////////////
void ToolRenderer::ConvertTopropercase(std::wstring& str) {
    CharLowerBuffW(&str[0], str.length());
    bool nextCap = true;
    for (wchar_t& ch : str) {
        if (iswspace(ch)) {
            nextCap = true;
        }
        else if (nextCap && iswalpha(ch)) {
            WCHAR temp[2] = { ch, L'\0' };
            CharUpperBuffW(temp, 1);
            ch = temp[0];
            nextCap = false;
        }
    }
}

//////////////////////////////////////////////////////////////////////
// Function : DrawToolName
// Purpose  : Draws formatted tool name below icon
//////////////////////////////////////////////////////////////////////
void ToolRenderer::DrawToolName(HDC hdc, const ToolInfo& tool, const RECT& rect) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(102, 102, 153));

    std::wstring name = tool.displayName;
    std::replace(name.begin(), name.end(), L'_', L' ');
    ConvertTopropercase(name);

    HFONT nameFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, nameFont);

    RECT textRect = { rect.left + 10, rect.top + 85, rect.right - 10, rect.bottom - 15 };
    DrawText(hdc, name.c_str(), -1, &textRect,
        DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL);

    SelectObject(hdc, oldFont);
    DeleteObject(nameFont);
}

//////////////////////////////////////////////////////////////////////
// Function : FillRoundedRectangle
// Purpose  : Fills rounded rectangle using GDI+ path
//////////////////////////////////////////////////////////////////////
void ToolRenderer::FillRoundedRectangle(Graphics* graphics, Brush* brush, INT x, INT y, INT w, INT h, INT r) {
    GraphicsPath path;
    path.AddArc(x, y, r * 2, r * 2, 180, 90);
    path.AddArc(x + w - r * 2, y, r * 2, r * 2, 270, 90);
    path.AddArc(x + w - r * 2, y + h - r * 2, r * 2, r * 2, 0, 90);
    path.AddArc(x, y + h - r * 2, r * 2, r * 2, 90, 90);
    path.CloseFigure();
    graphics->FillPath(brush, &path);
}

//////////////////////////////////////////////////////////////////////
// Function : DrawRoundedRectangle
// Purpose  : Draws border of rounded rectangle using GDI+
//////////////////////////////////////////////////////////////////////
void ToolRenderer::DrawRoundedRectangle(Graphics* graphics, Pen* pen, INT x, INT y, INT w, INT h, INT r) {
    GraphicsPath path;
    path.AddArc(x, y, r * 2, r * 2, 180, 90);
    path.AddArc(x + w - r * 2, y, r * 2, r * 2, 270, 90);
    path.AddArc(x + w - r * 2, y + h - r * 2, r * 2, r * 2, 0, 90);
    path.AddArc(x, y + h - r * 2, r * 2, r * 2, 90, 90);
    path.CloseFigure();
    graphics->DrawPath(pen, &path);
}
