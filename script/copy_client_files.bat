copy /y "vgframework_win64_msvc_dx12_release.exe" "Editor.exe"
xcopy "build\bin\Win64\release MSVC" "bin\Win64\release MSVC" /E /H /C /I /Y
xcopy "build\bin\Win64\release MSVC DX12" "bin\Win64\release MSVC DX12" /E /H /C /I /Y

copy /y "vgframework_win64_msvc_dx12_final.exe" "Game.exe"
xcopy "build\bin\Win64\final MSVC" "bin\Win64\final MSVC" /E /H /C /I /Y
xcopy "build\bin\Win64\final MSVC DX12" "bin\Win64\final MSVC DX12" /E /H /C /I /Y
