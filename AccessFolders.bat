@echo off
setlocal enabledelayedexpansion

set "base_path=S:\Review comments\COMMENTS FILES"

:: Define an array of folder names
set "folders=Review-CheckList Testcase Excel CPP Image PDF Block_extension_dll Error_Message omake Software XML Coverity_Report Document"

:: Check if base path exists, if not create it
if not exist "%base_path%" (
    mkdir "%base_path%"
    echo Created base path: %base_path%
)

:: Create folders if they do not exist
for %%F in (%folders%) do (
    set "folder_path=%base_path%\%%F"
    if not exist "!folder_path!" (
        mkdir "!folder_path!"
        echo Created folder: "!folder_path!"
    )
)

:menu
echo.
echo 1. Open Review folder
echo 2. Open Testcase folder
echo 3. Open Excel folder
echo 4. Open CPP folder
echo 5. Open Image folder
echo 6. Open PDF folder
echo 7. Open Block extension dll files folder
echo 8. Open Error Message folder
echo 8. Open omake folder
echo 9. Open Software folder
echo 10.Open XML folder
echo 11.Open Coverity_Report folder
echo 12.Open Document folder
echo a. Exit
echo.
set /p choice="Enter your choice: "

:: Open the selected folder based on user input
set "index=1"

for %%F in (%folders%) do (
    if "%choice%" == "!index!" (
        start "" "%base_path%\%%F"
        goto end
    )
    set /a index+=1
)

if "%choice%" == "a" (
    exit
) else (
    echo Invalid choice. Please try again.
    goto menu
)

:end
timeout /t 2 /nobreak > nul
