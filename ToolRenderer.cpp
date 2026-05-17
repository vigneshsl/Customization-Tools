#include "ToolRenderer.h"
#include <gdiplus.h>
using namespace Gdiplus;

///////////////////////////////////////////////////////////////////////////////
// Constructor — stores parent pointer, creates cached fonts for tool names
///////////////////////////////////////////////////////////////////////////////
ToolRenderer::ToolRenderer(ToolLauncher* launcher) : toolLauncher(launcher) {
    nameFont = CreateFont(14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");
}

///////////////////////////////////////////////////////////////////////////////
// Destructor — clean up cached font
///////////////////////////////////////////////////////////////////////////////
ToolRenderer::~ToolRenderer() {
    if (nameFont) {
        DeleteObject(nameFont);
        nameFont = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////
// Function : DrawTool
// Purpose  : Draws a clean tool card — minimal draw calls for
//            smooth performance. No card movement on hover.
//////////////////////////////////////////////////////////////////////
void ToolRenderer::DrawTool(HDC hdc, const ToolInfo& tool, int index, bool isHovered) {
    RECT rect = tool.rect;

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    int cornerRadius = 10;

    // ── Single shadow (lightweight) ─────────────────────────────────
    SolidBrush shadowBrush(Color(isHovered ? 25 : 15, 0, 0, 0));
    FillRoundedRectangle(&graphics, &shadowBrush,
        rect.left + 2, rect.top + 2,
        TOOL_BUTTON_SIZE, TOOL_BUTTON_SIZE, cornerRadius);

    // ── Card background ─────────────────────────────────────────────
    Color fillColor = isHovered
        ? Color(255, 240, 244, 255)   // Very subtle blue tint on hover
        : Color(255, 255, 255, 255);  // Pure white
    SolidBrush fillBrush(fillColor);
    FillRoundedRectangle(&graphics, &fillBrush,
        rect.left, rect.top,
        TOOL_BUTTON_SIZE, TOOL_BUTTON_SIZE, cornerRadius);

    // ── Card border ─────────────────────────────────────────────────
    Color borderColor = isHovered
        ? Color(255, 80, 130, 220)    // Blue on hover
        : Color(255, 225, 225, 230);  // Light gray
    Pen borderPen(borderColor, isHovered ? 1.5f : 1.0f);
    DrawRoundedRectangle(&graphics, &borderPen,
        rect.left, rect.top,
        TOOL_BUTTON_SIZE - 1, TOOL_BUTTON_SIZE - 1, cornerRadius);

    // ── Draw icon and name ──────────────────────────────────────────
    DrawToolIcon(hdc, tool, rect);
    DrawToolName(hdc, tool, rect);
}

//////////////////////////////////////////////////////////////////////
// Function : DrawToolIcon
// Purpose  : Draws tool icon bitmap centered in card, or a styled
//            placeholder if the icon is missing.
//////////////////////////////////////////////////////////////////////
void ToolRenderer::DrawToolIcon(HDC hdc, const ToolInfo& tool, const RECT& rect) {
    int iconX = rect.left + (TOOL_BUTTON_SIZE - TOOL_ICON_SIZE) / 2;
    int iconY = rect.top + 18;

    if (tool.icon) {
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, tool.icon);
        BitBlt(hdc, iconX, iconY, TOOL_ICON_SIZE, TOOL_ICON_SIZE, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBitmap);
        DeleteDC(memDC);
    }
    else {
        // Simple gray placeholder
        HBRUSH phBrush = CreateSolidBrush(RGB(210, 210, 218));
        RECT iconRect = { iconX, iconY, iconX + TOOL_ICON_SIZE, iconY + TOOL_ICON_SIZE };
        FillRect(hdc, &iconRect, phBrush);
        DeleteObject(phBrush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(130, 130, 140));
        DrawText(hdc, L"?", -1, &iconRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

//////////////////////////////////////////////////////////////////////
// Function : DrawToolName
// Purpose  : Draws formatted tool name below icon.
//////////////////////////////////////////////////////////////////////
void ToolRenderer::DrawToolName(HDC hdc, const ToolInfo& tool, const RECT& rect) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(50, 52, 60));

    std::wstring name = tool.displayName;
    std::replace(name.begin(), name.end(), L'_', L' ');
    ToolLauncher::ConvertTopropercase(name);

    HFONT oldFont = (HFONT)SelectObject(hdc, nameFont);

    RECT textRect = { rect.left + 6, rect.top + 88, rect.right - 6, rect.bottom - 5 };
    DrawText(hdc, name.c_str(), -1, &textRect,
        DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL);

    SelectObject(hdc, oldFont);
}

//////////////////////////////////////////////////////////////////////
// FillRoundedRectangle / DrawRoundedRectangle — GDI+ helpers
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

void ToolRenderer::DrawRoundedRectangle(Graphics* graphics, Pen* pen, INT x, INT y, INT w, INT h, INT r) {
    GraphicsPath path;
    path.AddArc(x, y, r * 2, r * 2, 180, 90);
    path.AddArc(x + w - r * 2, y, r * 2, r * 2, 270, 90);
    path.AddArc(x + w - r * 2, y + h - r * 2, r * 2, r * 2, 0, 90);
    path.AddArc(x, y + h - r * 2, r * 2, r * 2, 90, 90);
    path.CloseFigure();
    graphics->DrawPath(pen, &path);
}
