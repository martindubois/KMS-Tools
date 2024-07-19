
; Author    KMS - Martin Dubois, P. Eng.
; Copyright (C) 2023-2024 KMS
; License   http://www.apache.org/licenses/LICENSE-2.0
; Product   KMS-Tools
; File      Product_x64.iss

[Setup]
AppName=KMS-Tools
AppPublisher=KMS
AppPublisherURL=https://www.kms-quebec.com
AppSupportURL=https://www.kms-quebec.com
AppVersion=0.0.2-dev
DefaultDirName={pf}\KMS-Tools
OutputBaseFilename=KMS-Tools_0.0.2-dev_x64
OutputDir=Installer

[Files]
Source: "_DocUser\Documentation.html"                         ; DestDir: "{app}"; Flags: isreadme
Source: "_DocUser\KMS-Tools.ReadMe.txt"                       ; DestDir: "{app}"
Source: "ComTool\_DocUser\KMS-Tools.ComTool.ReadMe.txt"       ; DestDir: "{app}"
Source: "ComTool\ComTool_GUI.cmd"                             ; DestDir: "{app}"
Source: "Import\front-end\*"                                  ; DestDir: "{app}\front-end"; Flags: recursesubdirs
Source: "LabCtrl\_DocUser\KMS-Tools.LabCtrl.ReadMe.txt"       ; DestDir: "{app}"
Source: "Launcher\_DocUser\KMS-Tools.Launcher.ReadMe.txt"     ; DestDir: "{app}"
Source: "Launcher\Launcher_GUI.cmd"                           ; DestDir: "{app}"
Source: "ModbusSim\_DocUser\KMS-Tools.ModbusSim.ReadMe.txt"   ; DestDir: "{app}"
Source: "ModbusSim\ModbusSim_GUI.cmd"                         ; DestDir: "{app}"
Source: "ModbusTool\_DocUser\KMS-Tools.ModbusTool.ReadMe.txt" ; DestDir: "{app}"
Source: "ModbusTool\ModbusTool_GUI.cmd"                       ; DestDir: "{app}"
Source: "PGeo\_DocUser\KMS-Tools.PGeo.ReadMe.txt"             ; DestDir: "{app}"
Source: "PGeo\PGeo_GUI.cmd"                                   ; DestDir: "{app}"
Source: "WOP-Tool\_DocUser\KMS-Tools.WOP-Tool.ReadMe.txt"     ; DestDir: "{app}"
Source: "WOP-Tool\WOP-Tool_GUI.cmd"                           ; DestDir: "{app}"
Source: "x64\Release_Static\ComTool.exe"                      ; DestDir: "{app}"
Source: "x64\Release_Static\LabCtrl.exe"                      ; DestDir: "{app}"
Source: "x64\Release_Static\Launcher.exe"                     ; DestDir: "{app}"
Source: "x64\Release_Static\ModbusSim.exe"                    ; DestDir: "{app}"
Source: "x64\Release_Static\ModbusTool.exe"                   ; DestDir: "{app}"
Source: "x64\Release_Static\PGeo.exe"                         ; DestDir: "{app}"
Source: "x64\Release_Static\WOP-Tool.exe"                     ; DestDir: "{app}"
