@echo off
setlocal
echo User continues using the path. 
echo Z:\ctluif\protocee\FBMot1 
echo Z:\ctluif\protocee\FBMot2 
echo Z:\cee\CF\PMD\src 
echo C:\Users\H553536\Music\Testing Block Automation s\new update vba folder deep code
echo C:\Users\H553536\3D Objects\changes in review comments
echo.E:\WIN32API\CWRD
echo.

REM Set source folder and destination folder
set /p source_folder=Give copy file path :
set "destination_folder=c:\Customization tool\backup folder"
set /p Frontname=Optional Filename or default name (Y/N): 

if /i "%Frontname%"=="Y" (
    set /p Front_name=Enter the filename : 
) 

 
if not exist "%destination_folder%" (
    mkdir "%destination_folder%"
)

REM Get the current date and time in YYYYMMDD_HHMMSS_MMM format
for /f "delims=" %%a in ('wmic OS Get localdatetime ^| find "."') do set datetime=%%a
set "year=%datetime:~0,4%"
set "month=%datetime:~4,2%"
set "day=%datetime:~6,2%"
set "hour=%datetime:~8,2%"
set "minute=%datetime:~10,2%"
set "second=%datetime:~12,2%"
set "millisecond=%datetime:~15,3%"
set "file_name=%Front_name%_%day%_%month%_%year%_%hour%_%minute%_%second%_%millisecond%"
set "backup_folder=%destination_folder%\%file_name%"
for %%I in ("%source_folder%") do set "lastWord=%%~nxI"
set "FolderName=%lastWord%"
if not exist "%FolderName%" (
    mkdir "%FolderName%"
)
set "FBMot1=%backup_folder%\%FolderName%"
if not exist "%backup_folder%" (
    mkdir "%backup_folder%"
)

xcopy "%source_folder%\*" "%FBMot1%\" /E /I /Y
rem xcopy "%CMN_folder%\*" "%CMN%\" /E /I /Y
if %errorlevel% neq 0 (
    echo An error occurred during the copy process.
) else (
    echo Files copied successfully.
)

timeout /t 2 /nobreak > nul
start "" "%backup_folder%"
 