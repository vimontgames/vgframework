# changelog

## 0.32
### Build
Update to Visual Studio 2022 17.11.2\
HDR output (WIP)\
FPS and CPU/GPU frame timings displayed in HUD and editor
Fix UI in FINAL fullscreen mode

## 0.31

Fix Float4 color edit without alpha\
Optimize selection \
Better UI for DynamicProperty debug\
Merge static colliders at init\
Fix RigidCharacter initial matrix\
Control views using Camera component\
Viewport stats\
Split-screen\
Per-view UI\
Fix wireframe\
Add **Final** target

## 0.30 

Split "Rigid Body" component into separate "Body" and "Shape" components\
Physics shapes now support rotation\
Added 2 new shapes: TaperedCapsule and Cylinder\
AttachToNode component to link object matrix to animation node\
Reset animation state when engine stops\
MaxDT and moved timing settings to engine options\
Expose CullMode in materials

## 0.29 
Override Prefab properties\
Fix editor crash closing Prefab view\
Components can reference other GameObjects using UID\
UICanvasComponent, UITextComponent, andUIImageComponent\
Update JoltPhysics to 5.0.0\
Update Visual Studio 17.10.3

## 0.28 
Reverse Z-Buffer\
Fix rounding issues\
Custom Snap component\
Fix physics debug scale\
Editor fixes related to Prefabs\
Planar projection

## 0.27 
Can now create **Prefabs** to instanciate in Levels\
Animation blend helper functions\
Use ImGuiFileDialog instead of ImGuiFileBrowser\
3D View Titlebar menu

## 0.26 
Editor Camera V0\
Improve window resize + fix rare crash when maximizing window\
Forbid deleting or adding siblings to root node\
Better leak tracking with __FILE__ and __LINE__ + Fix rare memory leak in animation loading\
Update WinPixEventRuntime 1.0.231030001

## 0.25 
Fix an issue with **CountLeadingZeroes** that could return a different result depending on CPU used\
Jolt no more uses **AVX2** to allow running on older hardware\
VGFramework requires **AVX1** available on any AMD/Intel CPU since 2011 (Bulldozer/Sandy Bridge)\
Patch shader files in memory to support GPU DebugInfo/Warning/Error/Assert\
Fix non-triangulated mesh import

## 0.24 
New darker ImGui style\
Read instance color and texture from GPUInstanceData and GPUMaterialData\
Improve BLAS management\
Transparent pass\
Lock-free multithreaded debug display

## 0.23
Directional and spot lights with Raytraced (hard) shadows

## 0.22
Skinned BLAS updates for RT\
Add icons for component types\
Depth/Stencil Shader Resource Views\
Fix Vulkan RenderDoc capture crash because of RayTracing used in bindless root signature

## 0.21
Add support for inline RayTracing with BLAS & TLAS for DirectX12/Vulkan\
Duplicate/Instanciate GameObjects hierarchies UI\
Log shader and PSO compilation timings\
Optimize physics broadphase when game starts\
Use float for dt instead of double\
Optimized DirectX12 clear color\
Fix multithread issue in Resource window when hot-reloading\
Variable map/unmap size to speed up skinning\
Update Vulkan Memory Allocator to 3.0.1\
Update hlslpp to 3.4

## 0.20
Translate/Move/Rotate objects using ImGuizmo\
Character Controller component\
Start in Game mode, not editor

## 0.17
New add/remove component buttons and improved components popup windows
Add support for themes

## 0.16
JoltPhysics integration V0

## 0.15
**Removed dependency to the FBX SDK**

## 0.14
Skeletal animations with Compute Shader Skinning\
Modified properties are restored when exiting Game mode\
Application now has icon :p

## 0.13
Loaded resources are displayed per type and their owners with paths

## 0.12
Mesh (FBX), Material & Texture hot-reload\
GPU Picking

## 0.11
Add Info/Warning/Error console + clean & fixes

## 0.1
Separate DLLs for Engine, Renderer, Editor and Game.