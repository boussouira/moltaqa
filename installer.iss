[Setup]
AppId={{15FA1AB7-9C81-43E8-9882-C7145422FE7E}
AppName=مكتبة الملتقى
AppVerName=مكتبة الملتقى 0.9a3
DefaultDirName={pf}\moltaqa-lib
DefaultGroupName=مكتبة الملتقى
AllowNoIcons=yes
OutputDir=installer
OutputBaseFilename=moltaqa-lib-0.9-alpha1
SetupIconFile=moltaqa-lib.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
;Name: "arabic"; MessagesFile: "compiler:Languages\Arabic.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
; Application
Source: "bin\moltaqa-lib.exe"; DestDir: "{app}\bin"; Flags: ignoreversion

; CLucene
Source: "bin\clucene-contribs-lib.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\clucene-core.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\clucene-shared.dll"; DestDir: "{app}\bin"; Flags: ignoreversion

; ZLib
Source: "bin\zlib1.dll"; DestDir: "{app}\bin"; Flags: ignoreversion

; Visual visual c++ stuff
Source: "bin\msvcm90.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\msvcp90.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\msvcr90.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\Microsoft.VC90.CRT.manifest"; DestDir: "{app}\bin"; Flags: ignoreversion

; User manual
;Source: "manual.doc"; DestDir: "{app}"; Flags: ignoreversion

; Qt dll
Source: "bin\phonon4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\QtCore4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\QtGui4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\QtNetwork4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\QtSql4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\QtWebKit4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\QtXmlPatterns4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "bin\QtXml4.dll"; DestDir: "{app}\bin"; Flags: ignoreversion

; Other stuff
Source: "share\moltaqa-lib\*"; DestDir: "{app}\share\moltaqa-lib"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "bin\sqldrivers\*"; DestDir: "{app}\bin\sqldrivers"; Flags: ignoreversion recursesubdirs createallsubdirs

; Fonts
Source: "share\fonts\moltaqa-lib\*"; DestDir: "{fonts}"; Flags: ignoreversion

[Icons]
Name: "{group}\مكتبة الملتقى"; Filename: "{app}\bin\moltaqa-lib.exe"
;Name: "{group}\دليل المستخدم"; Filename: "{app}\manual.doc"
Name: "{group}\{cm:UninstallProgram,مكتبة الملتقى}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\مكتبة الملتقى"; Filename: "{app}\bin\moltaqa-lib.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\bin\moltaqa-lib.exe"; Description: "{cm:LaunchProgram,مكتبة الملتقى}"; Flags: nowait postinstall skipifsilent

[InstallDelete]
