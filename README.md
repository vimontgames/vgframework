# vgframework
Yes, yet another work-in-progress game and graphic engine project :)

[news](#news)\
[build](#build)\
[description](#description)\
[changelog](#changelog)\
[getting started](#getting-started)\
[extern libs](#extern-libs)\
[disclaimer](#disclaimer)

![Screenshot](doc/img/v32.gif)

# news
<small>07/17/24</small>
Experimental ARM64EC support by [Onduril](https://github.com/vimontgames/vgframework/tree/arm64EC)

# build
| Platform | API    | Debug                                                                                                                    | Release                                                                                                                      | Final 												
| -------- | ------ | ------------------------------------------------------------------------------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------- | -----------------------------------------------------------------------------------------------------------------------
| ARM64EC  | DX12   | ![Debug_ARM64EC_DX12.yml](https://github.com/vimontgames/vgframework/actions/workflows/Debug_ARM64EC_DX12.yml/badge.svg) | ![Release_ARM64EC_DX12.yml](https://github.com/vimontgames/vgframework/actions/workflows/Release_ARM64EC_DX12.yml/badge.svg) | ![Final_ARM64EC_DirectX12.yml](https://github.com/vimontgames/vgframework/actions/workflows/Final_ARM64EC_DX12.yml/badge.svg)
| ARM64EC  | Vulkan | *N/A*                                                                                                                    | *N/A*                                                                                                                        | *N/A*
| Win64    | DX12   | ![Debug_Win64_DX12.yml](https://github.com/vimontgames/vgframework/actions/workflows/Debug_Win64_DX12.yml/badge.svg)     | ![Release_Win64_DX12.yml](https://github.com/vimontgames/vgframework/actions/workflows/Release_Win64_DX12.yml/badge.svg)     | ![Final_Win64_DirectX12.yml](https://github.com/vimontgames/vgframework/actions/workflows/Final_Win64_DX12.yml/badge.svg)
| Win64    | Vulkan | ![Debug_Win64_Vulkan.yml](https://github.com/vimontgames/vgframework/actions/workflows/Debug_Win64_Vulkan.yml/badge.svg) | ![Release_Win64_Vulkan.yml](https://github.com/vimontgames/vgframework/actions/workflows/Release_Win64_Vulkan.yml/badge.svg) | ![Final_Win64_Vulkan.yml](https://github.com/vimontgames/vgframework/actions/workflows/Final_Win64_Vulkan.yml/badge.svg)

# description

Its renderer only uses modern graphics APIs like DirectX12 and Vulkan (I would also like to support Metal the day I got and Apple device) so it can be 100% bindless from the very beginning and do strong choices in that direction:  
* instanciating and writing into GPU tables is not even exposed: You have to do all the rendering stuff the modern, bindless way. 
* All the interface is made to encourage using only push constants and bindless buffers/textures, not updating tables constantly as if we were still emulating DX9.
* You can't access the command lists without using the framegraph that is part of the graphic driver layer. Go framegraph or do not render anything.
* Apart from the number of root constants, all shaders are sharing the same root signatures.

# changelog

Moved [changelog](CHANGELOG.md) to a separate file.

# getting started

- Sync the depot.
- Install the required SDKs and configure the environment variables.
- Open the "vgframework.sln" solution in Visual Studio 2022 Community IDE.
- Build & Run

**Debug** and **Release** versions will start with the editor:
Press F5 to enter game mode and F11 to maximize game view and press ESC to go back Editor mode.

**Final** versions are compiled without the editor, they will start directly in game mode and maximized.

First run might take a few seconds because files are cooking.
 
## command-line args

| Argument		  | Type | Default	 | Description													
| --------------  | ---- | --------- | ------------------------------------------------------------- 
| attachDebugger  | bool | false	 | MessageBox at application start to let attach a debugger
| breakOnErrors   | bool | true	     | Break on graphics API errors (with **debugDevice**)
| breakOnWarnings | bool | false	 | Break on graphics API warnings (with **debugDevice**)
| debugDevice	  | bool | false (*) | Enable the graphics API debug layer	
| fullscreen	  | bool | false	 | Start application with game view maximized instead of editor	
| play			  | bool | false	 | Start application with play mode running	

(*) **debugDevice** is enabled by default on **debug** builds

## keyboard shortcuts

| Key			   | Description	 												
| ---------------- | ------------------------------------------------------------- 
| A                | Toggle snap
| F                | Focus Selection
| H                | Toggle HDR modes (None, HDR10, HDR16)
| V                | Toggle VSync mode (None, 1 VBL, 2 VBL, 3 VBL, 4 VBL)
| UP               | Move editor camera forward
| LEFT             | Move editor camere left
| DOWN             | Move editor camera backward
| RIGHT            | Move editor camere right
| F1               | Start/stop profiler capture	
| F2               | Rename selected Object
| F6               | Hot reload shaders
| F7               | Hot reload resources				
| F5               | Enter Play mode
| Shift-F5         | Restart Play mode
| PAUSE            | Pause/resume Play mode
| F11              | Maximize Game view (fullscreen)
| ESCAPE           | Exit Play mode and go back to Editor mode
| DELETE           | Delete selected objects
| Ctrl+Mouse Wheel | Toggle Translation/Rotation/Scale gizmo
| Ctrl+D           | Duplicate selected objects (*)
| Ctrl+S           | Save
| Ctrl+U           | Save Prefab & Update
| Ctrl+Shift-Q     | Quit application

(*) You can also keep shift pressed while moving objects to duplicate

## ide
VS Studio 2022 Community\
https://visualstudio.microsoft.com/fr/vs/community/

To compile the solution you will need the following components if they are not already installed:
- MSVC v143 - VS 2022 C++ x64/x86 build tools (latest)
- C++ ATL for latest v143 build tools (x86 & x64)

You will also need if you want to build the ARM64EC versions:
- MSVC v143 - VS 2022 C++ ARM64/ARM64EC build tools (latest)
- C++ ATL for latest v143 build tools (ARM64/ARM64EC)

![Screenshot](doc/img/vsbuild.png)

You can change the Solution Platform in Visual Studio's command line to toggle between the DirectX12 and the Vulkan renderers.

Also don't forget to set the *working directory* to **$(SolutionDir)**.

![Screenshot](doc/img/SolutionPlatformName2.png)

## SDKs

The SDKs are not included with the sources. You need to install them.

| SDK			 | Version       | URL												
| -------------- | ------------- | ------------------------------------------------------------- 
| Win10 SDK		 | 10.0.22621.0  | https://developer.microsoft.com/fr-fr/windows/downloads/sdk-archive/	
| Vulkan SDK (*) | SDK 1.3.261.1 | https://vulkan.lunarg.com/sdk/home 

(*) Only required to build Vulkan version. **$(VULKAN_SDK)** should point the Vulkan SDK installation dir (i.e. **"C:\SDK\Vulkan\1.3.261.1"**)

# extern libs

The external libraries are included with the sources for convenience.

| Name                       | Version       | URL                                                                       
| -------------------------- | ------------- | ------------------------------------------------------------------------- 
| D3D12 Memory Allocator     |               | https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator    
| D3D12 HDR Sample           |               | https://github.com/microsoft/directx-graphics-samples/tree/master/Samples/Desktop/D3D12HDR
| Dear ImGui - Docking       | 1.91.2        | https://github.com/ocornut/imgui/tree/docking                          
| DirectXShaderCompiler      | August 2023   | https://github.com/microsoft/DirectXShaderCompiler/releases/tag/v1.7.2308 
| Dirent                     | 1.23.2        | https://github.com/tronkko/dirent                                                            
| fmt                        | 10.1.1        | https://github.com/fmtlib/fmt/releases/tag/10.1.1                         
| hlslpp                     | 3.5.2         | https://github.com/redorav/hlslpp/releases/tag/3.5.2                       
| IconFontCppHeaders         |               | https://github.com/juliettef/IconFontCppHeaders                                                     
| ImGuiFileDialog            | 0.6.7         | https://github.com/aiekick/ImGuiFileDialog
| ImGuizmo                   | 1.83          | https://github.com/CedricGuillemet/ImGuizmo/releases/tag/1.83             
| JoltPhysics                | 5.0.0         | https://github.com/jrouwe/JoltPhysics/releases/tag/v5.0.0                   
| meta_enum_lite             |               | https://github.com/vimontgames/meta_enum_lite         
| optick (*)                 | 1.3.1         | https://github.com/bombomby/optick                                        
| px_sched                   |               | https://github.com/pplux/px   
| SoLoud                     | August 2024   | https://github.com/jarikomppa/soloud   
| stb                        |               | https://github.com/nothings/stb                                           
| tinyXML2                   | 10.0.0        | https://github.com/leethomason/tinyxml2/releases/tag/10.0.0               
| UFBX                       |               | https://github.com/ufbx/ufbx                                              
| Vulkan Memory Allocator    | 3.0.1         | https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator         
| WinPixEventRuntime         | 1.0.231030001 | https://www.nuget.org/packages/WinPixEventRuntime                         

(*) *You can download the binaries for optick 1.3.1 from https://github.com/bombomby/optick/releases/tag/1.3.1.0*

# data

## fonts

| Font						 | Author        | URL												
| -------------------------- | ------------- | ------------------------------------------------------------- 
| Font-Awesome               | 6.x           | https://github.com/FortAwesome/Font-Awesome/tree/6.x   
| Rowdies                    |               | https://github.com/magictype/rowdy 
| RubikMonoOne               |               | https://fonts.google.com/specimen/Rubik+Mono+One
| UbuntuMono                 | 1.0           | https://fonts.google.com/specimen/Ubuntu+Mono

## models

| Model						    | Author            | URL												
| ----------------------------- | ----------------- | ------------------------------------------------------------- 
| 3D Foot Ball Free low-poly 3D | prabath-weerapana | https://www.cgtrader.com/free-3d-models/sports/game/3d-foot-ball
| Luigi						    | Anthony Yanez     | https://www.turbosquid.com/3d-models/luigi-super-model-1308288 
| Traffic Cone Free 3D model	| hinndia           | https://www.cgtrader.com/free-3d-models/exterior/street-exterior/traffic-cone-5849a434-2331-4a90-9b87-087ccc16cd0c
| Soccergoal                    | NorbertVarga      | https://www.turbosquid.com/3d-models/soccergoal-3d-model-1840894

## sound

| Sound		 | Author               | URL												
| ---------- | -------------------- | ------------------------------------------------------------- 
| Tally Dub  | Brotheration Records | https://pixabay.com/fr/music/reggae-tally-dub-brotheration-records-2016-140287/  

# misc

Used https://github.com/Nadrin/PBR as reference for PBR lighting equations (MIT licence)

# disclaimer
Everything is heavily "Work-in-Progress" and 0% in an usable state, and I would not recommend anyone to use it yet.
