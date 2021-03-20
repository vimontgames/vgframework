# vgframework
 VG framework is using DirectX12/Vulkan
 
# external
D3D12 Memory Allocator\
https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator

Dear ImGui\
https://github.com/ocornut/imgui

Dirent\
https://github.com/tronkko/dirent

hlslpp\
https://github.com/redorav/hlslpp

ImGui-Addons\
https://github.com/gallickgunner/ImGui-Addons

magic_enum\
https://github.com/Neargye/magic_enum

px_sched\
https://github.com/pplux/px

stb\
https://github.com/nothings/stb

Vulkan Memory Allocator\
https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator

WinPixEventRuntime 1.0.200127001\
https://www.nuget.org/packages/WinPixEventRuntime

# sdk
FBX SDK 2020.0.1\
https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0  
**$(FBX_SDK)** should point to the FBX SDK installation dir *(i.e. "C:\SDK\FBX\2020.0.1")*

Vulkan SDK 1.2.148.0 (VK)\
https://vulkan.lunarg.com/sdk/home  
**$(VK_SDK_PATH)** should point the Vulkan SDK installation dir *(i.e. "C:\SDK\Vulkan\1.2.148.0")*

Win10 SDK 10.0.17763.0\
https://developer.microsoft.com/fr-fr/windows/downloads/sdk-archive/

# ide
VS Studio 2017 Community\
https://visualstudio.microsoft.com/fr/vs/older-downloads/

# optional
"Solution Configuration Name" plugin for Visual Studio 2017
https://marketplace.visualstudio.com/items?itemName=ceztko.SolutionConfigurationName

If you are using the "Solution Configuration Name" plugin you can use SolutionPlatform="$(SolutionPlatform)" in Visual Studio's command line to be able to use the DirectX12 or the Vulkan renderer by directly changing the solution platform.\
Also don't forget to set the *working directory* to **$(SolutionDir)**.

![Screenshot](doc/img/SolutionPlatformName1.png)
![Screenshot](doc/img/SolutionPlatformName2.png)
