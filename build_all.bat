@echo off

:: Locate Visual Studio installation and MSBuild path using vswhere
for /f "tokens=*" %%i in ('"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do set msbuild_path=%%i

:: Check if MSBuild was found
if not defined msbuild_path (
    echo MSBuild.exe not found!
    exit /b 1
)

:: Set your solution file name
set solution=vgframework.sln

:: List of configurations and platforms to build
set configurations=Debug Release Final
set platforms="Win64 DX12" "Win64 Vulkan" "ARM64EC DX12" "ARM64EC Vulkan"

:: Loop through each configuration
for %%c in (%configurations%) do (
    :: Loop through each platform
    for %%p in (%platforms%) do (
        echo Building Configuration: %%c, Platform: %%p
        "%msbuild_path%" %solution% /p:Configuration=%%c /p:Platform=%%p /m
        if errorlevel 1 (
            echo Build failed for Configuration: %%c, Platform: %%p
            exit /b 1
        )
    )
)

echo Build completed for all configurations and platforms.