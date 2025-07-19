#include "ToolScanner.h"
#include "ToolIconManager.h"
#include <unordered_set>

//////////////////////////////////////////////////////////////////////
// Constructor: Initialize with icon manager
//////////////////////////////////////////////////////////////////////
ToolScanner::ToolScanner(ToolIconManager* iconMgr) : iconManager(iconMgr) {}

//////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////
ToolScanner::~ToolScanner() {}

//////////////////////////////////////////////////////////////////////
// ScanForTools: Scans all files in current folder, filters by extension
//////////////////////////////////////////////////////////////////////
std::vector<ToolInfo> ToolScanner::ScanForTools() {
    std::vector<ToolInfo> foundTools;
    foundTools.reserve(64); // Preallocate memory for better performance

    // File types we care about
    static const std::unordered_set<std::wstring> supportedExtensions = {
        L".bat", L".py", L".exe", L".ps1"
    };

    // Search all files in the current directory
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(L"*.*", &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Skip folders
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                std::wstring filename = findData.cFileName;

                // Find file extension
                size_t dotPos = filename.find_last_of(L'.');
                if (dotPos != std::wstring::npos) {
                    std::wstring ext = filename.substr(dotPos);
                    if (supportedExtensions.count(ext)) {
                        foundTools.emplace_back(CreateToolInfo(filename, ext));
                    }
                }
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }

    return foundTools;
}

//////////////////////////////////////////////////////////////////////
// CreateToolInfo: Generates ToolInfo from file name and extension
//////////////////////////////////////////////////////////////////////
ToolInfo ToolScanner::CreateToolInfo(const std::wstring& filename, const std::wstring& extension) {
    ToolInfo tool;
    tool.filename = filename;
    tool.extension = extension;

    // Extract display name (remove extension)
    size_t dotPos = filename.find_last_of(L'.');
    tool.displayName = (dotPos != std::wstring::npos) ? filename.substr(0, dotPos) : filename;

    // Create icon for tool
    if (iconManager) {
        tool.icon = iconManager->CreateToolIcon(extension, tool.displayName);
    }

    return tool;
}

//////////////////////////////////////////////////////////////////////
// FilterTools: Case-insensitive filter by displayName
//////////////////////////////////////////////////////////////////////
std::vector<ToolInfo> ToolScanner::FilterTools(const std::vector<ToolInfo>& allTools, const std::wstring& searchText) {
    if (searchText.empty())
        return allTools;

    std::vector<ToolInfo> filtered;
    filtered.reserve(allTools.size());

    std::wstring lowerSearch = ToLower(searchText);

    for (const auto& tool : allTools) {
        std::wstring name = ToLower(tool.displayName);
        if (name.find(lowerSearch) != std::wstring::npos) {
            filtered.push_back(tool);
        }
    }

    return filtered;
}

//////////////////////////////////////////////////////////////////////
// ToLower: Converts a string to lowercase
//////////////////////////////////////////////////////////////////////
std::wstring ToolScanner::ToLower(const std::wstring& str) {
    std::wstring result;
    result.resize(str.size());

    // Faster than transform with lambda for wide strings
    for (size_t i = 0; i < str.size(); ++i)
        result[i] = towlower(str[i]);

    return result;
}
