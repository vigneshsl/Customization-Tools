#pragma once

#include <windows.h>     // Windows API for GDI drawing
#include <string>        // For using std::wstring (Unicode text)

//------------------------------------------------------------------------------
// Class: ToolIconManager
// Purpose: Responsible for generating simple bitmap icons dynamically
//          based on file extensions (.exe, .bat, etc.)
//------------------------------------------------------------------------------
class ToolIconManager {
public:
    // Constructor - sets up any default configurations (if needed)
    ToolIconManager();

    // Destructor - currently nothing to clean, but safe practice
    ~ToolIconManager();

    //-------------------------------------------------------------------------
    // Function: CreateToolIcon
    // Purpose : Creates a small bitmap icon with background color + text/symbol
    // Params  : extension - file type (e.g., ".exe")
    //           toolName - name of the tool (used for emoji fallback logic)
    // Returns : HBITMAP - handle to icon bitmap for display
    //-------------------------------------------------------------------------
    HBITMAP CreateToolIcon(const std::wstring& extension, const std::wstring& toolName);

private:
    //-------------------------------------------------------------------------
    // Function: GetIconBrush
    // Purpose : Returns a colored brush based on file extension
    //           Used to paint the icon background
    //-------------------------------------------------------------------------
    HBRUSH GetIconBrush(const std::wstring& extension);

    //-------------------------------------------------------------------------
    // Function: DrawIconText
    // Purpose : Renders text (like "BAT", "PY", emoji) on the icon bitmap
    // Params  : memDC - memory DC where text is drawn
    //           extension - extension that determines what to write
    //-------------------------------------------------------------------------
    void DrawIconText(HDC memDC, const std::wstring& extension);

    //-------------------------------------------------------------------------
    // Function: IsEmojiSymbol
    // Purpose : Checks if the given text is an emoji (optional use)
    //-------------------------------------------------------------------------
    bool IsEmojiSymbol(const std::wstring& text);

    //-------------------------------------------------------------------------
    // Function: GetIconSymbol
    // Purpose : Returns text or emoji to be shown inside the icon
    //           For example: "⚙" for .exe, "🐍" for .py
    //-------------------------------------------------------------------------
    std::wstring GetIconSymbol(const std::wstring& extension);
};
