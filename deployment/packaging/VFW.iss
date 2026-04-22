; --- VFW Player Installer (System-wide, Program Files) ---
#define MyAppName "VFW"
#define MyAppVersion "1.1.8"
#define MyAppPublisher "Nomix17"
#define MyAppURL "https://github.com/Nomix17/VFW"
#define MyAppExeName "VFW.exe"

[Setup]
AppId={{5C486A7E-0177-474C-B128-E89E1F2F7071}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=dialog
OutputDir=C:\Users\PC\Desktop\VFW_Project
OutputBaseFilename=VFW
SetupIconFile=C:\Users\PC\Desktop\VFW_Project\VFW-v1.1.8\assets\icons\VFW-Squircle-Dark.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
SolidCompression=yes
WizardStyle=modern dynamic
ChangesAssociations=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Main executable and build files (Program Files)
Source: "C:\Users\PC\Desktop\VFW_Project\VFW-v1.1.8\build\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\PC\Desktop\VFW_Project\VFW-v1.1.8\build\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

; User-writable configs & assets
Source: "C:\Users\PC\Desktop\VFW_Project\VFW-v1.1.8\configs\*"; DestDir: "{userappdata}\VFW\configs"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\PC\Desktop\VFW_Project\VFW-v1.1.8\assets\*"; DestDir: "{userappdata}\VFW\assets"; Flags: ignoreversion recursesubdirs createallsubdirs

[Registry]
; System-wide file associations (HKCR)
Root: HKCR; Subkey: "Applications\VFW.exe"; ValueType: string; ValueName: ""; ValueData: "VFW Player"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Applications\VFW.exe"; ValueType: string; ValueName: "FriendlyAppName"; ValueData: "VFW Player"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "VFWFile"; ValueType: string; ValueName: ""; ValueData: "Video & Audio file"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "VFWFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\VFW.exe,0"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "VFWFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\VFW.exe"" ""%1"""; Flags: uninsdeletevalue

; Video extensions
Root: HKCR; Subkey: ".mp4"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".avi"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".mkv"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".mov"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".wmv"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".flv"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".webm"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue

; Audio extensions
Root: HKCR; Subkey: ".mp3"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".wav"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".flac"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".aac"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".ogg"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".wma"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".m4a"; ValueType: string; ValueName: ""; ValueData: "VFWFile"; Flags: uninsdeletevalue

; MIME types
Root: HKCR; Subkey: ".mp4\Content Type"; ValueType: string; ValueName: ""; ValueData: "video/mp4"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".mkv\Content Type"; ValueType: string; ValueName: ""; ValueData: "video/x-matroska"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".avi\Content Type"; ValueType: string; ValueName: ""; ValueData: "video/x-msvideo"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".mov\Content Type"; ValueType: string; ValueName: ""; ValueData: "video/quicktime"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".wmv\Content Type"; ValueType: string; ValueName: ""; ValueData: "video/x-ms-wmv"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".flv\Content Type"; ValueType: string; ValueName: ""; ValueData: "video/x-flv"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".webm\Content Type"; ValueType: string; ValueName: ""; ValueData: "video/webm"; Flags: uninsdeletevalue

Root: HKCR; Subkey: ".mp3\Content Type"; ValueType: string; ValueName: ""; ValueData: "audio/mpeg"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".wav\Content Type"; ValueType: string; ValueName: ""; ValueData: "audio/wav"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".flac\Content Type"; ValueType: string; ValueName: ""; ValueData: "audio/flac"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".aac\Content Type"; ValueType: string; ValueName: ""; ValueData: "audio/aac"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".ogg\Content Type"; ValueType: string; ValueName: ""; ValueData: "audio/ogg"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".wma\Content Type"; ValueType: string; ValueName: ""; ValueData: "audio/x-ms-wma"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".m4a\Content Type"; ValueType: string; ValueName: ""; ValueData: "audio/mp4"; Flags: uninsdeletevalue

[Icons]
Name: "{userprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{userdesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
; Launch the app after installation
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#MyAppName}}"; Flags: nowait postinstall skipifsilent
; Refresh Explorer so file associations appear immediately
Filename: "rundll32.exe"; Parameters: "shell32.dll,SHChangeNotify 0x8000000,0"; Flags: runhidden
