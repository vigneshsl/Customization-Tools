#pragma once

#include "Main.h"              // Includes definitions like ToolInfo struct/class
#include <vector>              // Used for storing list of tools
#include <string>              // For using std::wstring (wide string support)
#include <unordered_set>       // Faster extension matching than multiple scan calls

// Forward declaration to avoid including full header
class ToolIconManager;

////////////////////////////////////////////////////////////////////////
// Class: ToolScanner
// Purpose: Responsible for scanning current directory for tools
//          like .exe, .bat, .py files and preparing display info.
////////////////////////////////////////////////////////////////////////
class ToolScanner {
public:
    // Constructor - needs icon manager to assign icons to tools
    ToolScanner(ToolIconManager* iconMgr);

    // Destructor - no dynamic memory to clean here, safe default
    ~ToolScanner();

    // Main function to scan directory for tool files (.exe, .bat, etc.)
    std::vector<ToolInfo> ScanForTools();

    // Filters tool list based on search text (e.g., user typing in search box)
    std::vector<ToolInfo> FilterTools(const std::vector<ToolInfo>& allTools, const std::wstring& searchText);

private:
    ToolIconManager* iconManager;  // Used to assign icons to discovered tools

    // Old method: scans a single file type using pattern like *.exe
    // (Not used in optimized version, but can be retained if fallback is needed)
    void ScanForFileType(std::vector<ToolInfo>& tools, const std::wstring& pattern, const std::wstring& extension);

    // Converts a file into ToolInfo (display name, extension, icon, etc.)
    ToolInfo CreateToolInfo(const std::wstring& filename, const std::wstring& extension);

    // Converts wide string to all lowercase (used for search matching)
    std::wstring ToLower(const std::wstring& str);
};
