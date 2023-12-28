
; Author    KMS - Martin Dubois, P. Eng.
; Copyright (C) 2023 KMS
; License   http://www.apache.org/licenses/LICENSE-2.0
; Product   KMS-Tools
; File      Product_x64.iss

[Setup]
AppName=KMS-Tools
AppPublisher=KMS
AppPublisherURL=https://www.kms-quebec.com
AppSupportURL=https://www.kms-quebec.com
AppVersion=0.0.0-dev
DefaultDirName={pf}\KMS-Tools
OutputBaseFilename=KMS-Tools_0.0.0-dev_x64
OutputDir=Installer

[Files]
Source: "_DocUser\Documentation.html"                         ; DestDir: "{app}"
Source: "_DocUser\KMS-Tools.ReadMe.txt"                               ; DestDir: "{app}"; Flags: isreadme
; TODO Source: "KMS-ComTool\_DocUser\KMS.KMS-ComTool.ReadMe.txt"       ; DestDir: "{app}"
Source: "ModbusSim\_DocUser\KMS-Tool.ModbusSim.ReadMe.txt"    ; DestDir: "{app}"
Source: "ModbusTool\_DocUser\KMS-Tools.ModbusTool.ReadMe.txt" ; DestDir: "{app}"
; TODO Source: "KMS-Sync\_DocUser\KMS.KMS-Sync.ReadMe.txt"             ; DestDir: "{app}"
; TODO Source: "KMS-WOP\_DocUser\KMS.KMS-WOP.ReadMe.txt"               ; DestDir: "{app}"
; TODO Source: "x64\Release_Static\KMS-ComTool.exe"                    ; DestDir: "{app}"
Source: "x64\Release_Static\ModbusSim.exe"                    ; DestDir: "{app}"
Source: "x64\Release_Static\ModbusTool.exe"                   ; DestDir: "{app}"
; TODO Source: "x64\Release_Static\KMS-Sync.exe"                       ; DestDir: "{app}"
; TODO Source: "x64\Release_Static\KMS-WOP.exe"                        ; DestDir: "{app}"
