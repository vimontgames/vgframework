copy /y "VGFramework_x64_Release_DX12.exe" "Editor.exe"
xcopy build\bin\x64\Release bin\x64\Release /E /H /C /I /Y
xcopy build\bin\x64\Release_DX12 bin\x64\Release_DX12 /E /H /C /I /Y

copy /y "VGFramework_x64_Final_DX12.exe" "Game.exe"
xcopy build\bin\x64\Release bin\x64\Final /E /H /C /I /Y
xcopy build\bin\x64\Release_DX12 bin\x64\Final_DX12 /E /H /C /I /Y
