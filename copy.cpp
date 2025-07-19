#include <windows.h>
#include <shobjidl.h>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#pragma comment(lib, "Ole32.lib")

// Function to show file + folder picker dialog (multi-select)
std::vector<std::wstring> ShowFilesAndFoldersDialog(HWND hwnd)
{
    std::vector<std::wstring> selectedPaths;

    IFileOpenDialog* pFileOpen = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpen))))
    {
        DWORD dwOptions = 0;
        pFileOpen->GetOptions(&dwOptions);
        pFileOpen->SetOptions(dwOptions | FOS_ALLOWMULTISELECT | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

        if (SUCCEEDED(pFileOpen->Show(hwnd)))
        {
            IShellItemArray* pItems = nullptr;
            if (SUCCEEDED(pFileOpen->GetResults(&pItems)))
            {
                DWORD count = 0;
                pItems->GetCount(&count);
                for (DWORD i = 0; i < count; ++i)
                {
                    IShellItem* pItem = nullptr;
                    if (SUCCEEDED(pItems->GetItemAt(i, &pItem)))
                    {
                        PWSTR pszPath = nullptr;
                        if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)))
                        {
                            selectedPaths.push_back(pszPath);
                            CoTaskMemFree(pszPath);
                        }
                        pItem->Release();
                    }
                }
                pItems->Release();
            }
        }
        pFileOpen->Release();
    }
    return selectedPaths;
}

// Function to select a destination folder
std::wstring ShowFolderDialog(HWND hwnd)
{
    std::wstring folderPath = L"";
    IFileDialog* pFileDialog = nullptr;

    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog))))
    {
        DWORD dwOptions = 0;
        pFileDialog->GetOptions(&dwOptions);
        pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

        if (SUCCEEDED(pFileDialog->Show(hwnd)))
        {
            IShellItem* pItem;
            if (SUCCEEDED(pFileDialog->GetResult(&pItem)))
            {
                PWSTR pszFilePath = nullptr;
                if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
                {
                    folderPath = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileDialog->Release();
    }

    return folderPath;
}

int wmain()
{
    CoInitialize(NULL);

    // Step 1: Select multiple files and folders
    std::vector<std::wstring> selectedItems = ShowFilesAndFoldersDialog(NULL);
    if (selectedItems.empty())
    {
        std::wcout << L"No files or folders selected.\n";
        CoUninitialize();
        system("pause");
        return 0;
    }

    // Step 2: Select destination folder
    std::wstring destFolder = ShowFolderDialog(NULL);
    if (destFolder.empty())
    {
        std::wcout << L"No destination folder selected.\n";
        CoUninitialize();
        system("pause");
        return 0;
    }

    // Step 3: Copy files and folders
    std::wcout << L"\nCopying items to: " << destFolder << std::endl;
    for (const auto& src : selectedItems)
    {
        fs::path sourcePath(src);
        fs::path destPath = fs::path(destFolder) / sourcePath.filename();

        try
        {
            if (fs::is_directory(sourcePath))
            {
                fs::copy(sourcePath, destPath,
                    fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            }
            else
            {
                fs::copy_file(sourcePath, destPath, fs::copy_options::overwrite_existing);
            }

            std::wcout << L"✔ Copied: " << sourcePath.filename() << std::endl;
        }
        catch (const std::exception& e)
        {
            std::wcout << L"✖ Failed to copy: " << sourcePath.filename() << L" (" << e.what() << L")\n";
        }
    }

    CoUninitialize();
    system("pause");
    return 0;
}
