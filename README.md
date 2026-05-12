# Customization Tools

A Windows 11-styled desktop tool launcher built with Win32 API and GDI+. Automatically discovers scripts and executables (`.bat`, `.py`, `.exe`, `.ps1`) in the current directory and displays them as clickable cards in a searchable grid UI.

## Features

- **Auto-discovery** of tools in the working directory
- **Windows 11-styled UI** with gradient header, rounded cards, and hover effects
- **Live search** with instant filtering
- **Keyboard shortcuts**: F5 (refresh), Esc (clear search), Enter (quick launch), Ctrl+F (focus search)
- **Double-buffered rendering** for flicker-free display
- **Dynamic icon generation** based on file type (emoji + colored backgrounds)
- **Scrollable grid** with mouse wheel and scroll bar support

## Building

1. Open `Customization tool win32api.sln` in Visual Studio 2022+
2. Build in Release|x64 (or Debug for development)
3. Place the compiled `.exe` in a folder alongside your tool scripts

## Included Tools

| Tool | Type | Description |
|---|---|---|
| `AccessFolders.bat` | Batch | Quick-access menu for review folder structure |
| `close.bat` | Batch | Force-kill all running processes |
| `copy_usermade.bat` | Batch | Timestamped folder backup utility |
| `CodeLine Counter.py` | Python | LOC/KLOC counter for C++ projects |
| `Content_Replacement_Tool.py` | Python | Bulk find/replace from Excel rules |
| `file_rename.py` | Python | Batch regex-based file renaming from Excel |

## Requirements

- Windows 10/11
- Visual Studio 2022 (v143 toolset) for building
- Python 3.x (for running `.py` tools)
