; Grok Desktop installer script (Inno Setup 6).
; Compiled by scripts\package.ps1 which passes:
;   /DMyAppVersion=<version>
;   /DSourceDir=<path to build\src\Release>
;   /O<output dir>

#ifndef MyAppVersion
#define MyAppVersion "1.0.0"
#endif
#ifndef SourceDir
#define SourceDir "..\build\src\Release"
#endif

#define MyAppName "Grok Desktop"
#define MyAppPublisher "Grok Desktop Project"
#define MyAppURL "https://grok.com/"
#define MyAppExeName "GrokDesktop.exe"

[Setup]
AppId={{8F3C2A91-4E7B-4D6A-9C15-2B8E7F0A6D41}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputBaseFilename=GrokDesktop-{#MyAppVersion}-win64-setup
Compression=lzma2/max
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0.17763
PrivilegesRequired=admin
SetupIconFile=..\resources\app.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
UninstallDisplayName={#MyAppName}
WizardStyle=modern
CloseApplications=yes
RestartApplications=no

[Languages]
Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Ship the entire Release output directory, minus MSVC import artifacts.
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs; Excludes: "*.exp,*.lib"

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#MyAppName}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
; Do NOT delete the user profile under %LOCALAPPDATA%\GrokDesktop.
; Only remove files the installer created in {app} (handled automatically).
; This section is intentionally left without Type: filesandordirs entries
; that could touch user data.
