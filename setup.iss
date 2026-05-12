; ============================================================
; Customization Tools - Inno Setup Installer Script
; ============================================================
; To build the installer:
;   1. Install Inno Setup from https://jrsoftware.org/isinfo.php
;   2. Open this file in Inno Setup Compiler
;   3. Click Build > Compile (or press Ctrl+F9)
;   4. The installer will be created in the "Output" directory
; ============================================================

#define MyAppName "Customization Tools"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Vignesh SL"
#define MyAppExeName "Customization tool win32api.exe"
#define MyAppURL "https://github.com/vigneshsl/Customization-Tools"

[Setup]
; Application identity
AppId={{B7A3F8D2-4E5C-4A1B-9D6E-8F2C3A7B5D4E}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}

; Installation directory
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}

; Output settings
OutputDir=installer_output
OutputBaseFilename=CustomizationTools_Setup_v{#MyAppVersion}
SetupIconFile=UI.ico

; Compression
Compression=lzma2/ultra64
SolidCompression=yes

; Minimum Windows version (Windows 10)
MinVersion=10.0

; Privileges
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog

; UI settings
WizardStyle=modern
WizardSizePercent=120

; Uninstaller
UninstallDisplayIcon={app}\{#MyAppExeName}
UninstallDisplayName={#MyAppName}

; Allow user to create desktop icon
AllowNoIcons=yes

; Architecture
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "Create a &Quick Launch icon"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; ── Main Application ──
; NOTE: Update the Source path below to point to your actual Release build output directory
Source: "x64\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion

; ── Application Icon ──
Source: "UI.ico"; DestDir: "{app}"; Flags: ignoreversion

; ── Batch Script Tools ──
Source: "AccessFolders.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "close.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "copy_usermade.bat"; DestDir: "{app}"; Flags: ignoreversion

; ── Python Script Tools ──
Source: "CodeLine Counter.py"; DestDir: "{app}"; Flags: ignoreversion
Source: "Content_Replacement_Tool.py"; DestDir: "{app}"; Flags: ignoreversion
Source: "file_rename.py"; DestDir: "{app}"; Flags: ignoreversion

; ── README ──
Source: "README.md"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; Start Menu shortcuts
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

; Desktop shortcut (optional)
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

; Quick Launch shortcut (optional)
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
; Option to run the app after installation
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Registry]
; Register the application in Windows Apps list
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\{#MyAppPublisher}\{#MyAppName}"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"; Flags: uninsdeletekey

[Code]
// ── Custom code to check for Python installation ──
function IsPythonInstalled: Boolean;
var
  ResultCode: Integer;
begin
  Result := Exec('python', '--version', '', SW_HIDE, ewWaitUntilTerminated, ResultCode)
            and (ResultCode = 0);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    if not IsPythonInstalled then
    begin
      MsgBox('Python was not detected on this system.' + #13#10 +
             'Some tools (.py files) require Python 3.x to run.' + #13#10#13#10 +
             'Please install Python from https://www.python.org/downloads/',
             mbInformation, MB_OK);
    end;
  end;
end;
