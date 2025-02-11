#include "Precomp.h"

#include "core/Plugin/Plugin.h"
#include "core/CmdLine/CmdLine.h"
#include "core/Logger/Logger.h"
#include "core/Kernel.h"
#include "core/File/File.h"
#include "core/Object/AutoRegisterClass.h"

#include "engine/IEngine.h"
#include "engine/IEngineOptions.h"
#include "renderer/IRenderer.h"
#include "gfx/IDevice.h"

#include "resource.h"
#include "application.h"
#include "version.h"
#include "commit.h"

HINSTANCE hInst;
HWND g_hWnd;

using namespace vg;
engine::IEngine * g_engine = nullptr;
renderer::IRenderer * g_renderer = nullptr;

#define VG_TEST 1
#ifdef VG_TEST
#define VG_TEST_CLZ_CTZ 1
#ifdef VG_TEST_CLZ_CTZ
#include "core/Math/Math.h"
#endif
#endif

//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (g_engine && g_engine->WndProc(hWnd, message, wParam, lParam))
        return true;

	int wmId, wmEvent;

	switch (message)
	{
		case WM_SIZE:
		{
			RECT win;
			GetClientRect(g_hWnd, &win);
		}
		break;

		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SYSKEYDOWN:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
				{
					RECT win;
					GetClientRect(g_hWnd, &win);
				}
				break;
			}
			break;

		case WM_KEYDOWN:
			switch (LOWORD(wParam))
			{
				// Ctrl-Shift-Q
				case 'Q':
				{
					if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))
						SendMessage(g_hWnd, WM_DESTROY, 0, 0);
				}
				break;
			}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//--------------------------------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"Game";
	wcex.hIconSm = nullptr;

	return RegisterClassEx(&wcex);
}

//--------------------------------------------------------------------------------------
bool processSystemMessage()
{
	bool quit = false;
	MSG msg = { 0 };
	BOOL message = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	while (message)
	{
		if (msg.message == WM_QUIT)
		{
			if (g_engine)
				g_engine->Quit();
			quit = true;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		message = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	}
	return quit;
}

//--------------------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, core::uint _width, core::uint _height)
{
	hInst = hInstance;

	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;

	int flags = WS_OVERLAPPEDWINDOW;

	RECT rc = { 0, 0, (LONG)_width, (LONG)_height };
	AdjustWindowRect(&rc, flags, FALSE);

	g_hWnd = CreateWindow(L"Game", L"VG Framework", flags, x, y, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!g_hWnd)
		return FALSE;

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

//--------------------------------------------------------------------------------------
bool CreateGameWindow(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, core::uint _width, core::uint _height)
{
	if (!MyRegisterClass(hInstance))
		return false;

	if (!InitInstance(hInstance, nCmdShow, _width, _height))
		return false;

	return true;
}

#if VG_TEST
void runTests()
{
#define VG_EXPECT_EQ(expr, expected) { bool vgtest = (expr == expected); VG_ASSERT(vgtest, "%s\n\nReturned '%u' instead of expected value '%u'.", #expr, expr, expected); }

	using namespace vg::core;

	#ifdef VG_TEST_CLZ_CTZ
    // ctz for u32
    VG_EXPECT_EQ(ctz(u32(0x00000001)), 0);
    VG_EXPECT_EQ(ctz(u32(0x00000002)), 1);
    VG_EXPECT_EQ(ctz(u32(0x00000004)), 2);
    VG_EXPECT_EQ(ctz(u32(0x00000008)), 3);
    VG_EXPECT_EQ(ctz(u32(0x00000010)), 4);
    VG_EXPECT_EQ(ctz(u32(0x00000000)), 32);  // Zero case
    VG_EXPECT_EQ(ctz(u32(0x80000000)), 31);  // High bit set

    // ctz for u64
    VG_EXPECT_EQ(ctz(u64(0x0000000000000001)), 0);
    VG_EXPECT_EQ(ctz(u64(0x0000000000000002)), 1);
    VG_EXPECT_EQ(ctz(u64(0x0000000000000004)), 2);
    VG_EXPECT_EQ(ctz(u64(0x0000000000000008)), 3);
    VG_EXPECT_EQ(ctz(u64(0x0000000000000010)), 4);
    VG_EXPECT_EQ(ctz(u64(0x0000000000000000)), 64);  // Zero case
    VG_EXPECT_EQ(ctz(u64(0x8000000000000000)), 63);  // High bit set

    // clz for u32
    VG_EXPECT_EQ(clz(u32(0x00000001)), 31);
    VG_EXPECT_EQ(clz(u32(0x00000002)), 30);
    VG_EXPECT_EQ(clz(u32(0x00000004)), 29);
    VG_EXPECT_EQ(clz(u32(0x00000008)), 28);
    VG_EXPECT_EQ(clz(u32(0x00000010)), 27);
    VG_EXPECT_EQ(clz(u32(0x80000000)), 0);  // High bit set
    VG_EXPECT_EQ(clz(u32(0x00000000)), 32);  // Zero case

    // clz for u64
    VG_EXPECT_EQ(clz(u64(0x0000000000000001)), 63);
    VG_EXPECT_EQ(clz(u64(0x0000000000000002)), 62);
    VG_EXPECT_EQ(clz(u64(0x0000000000000004)), 61);
    VG_EXPECT_EQ(clz(u64(0x0000000000000008)), 60);
    VG_EXPECT_EQ(clz(u64(0x0000000000000010)), 59);
    VG_EXPECT_EQ(clz(u64(0x8000000000000000)), 0);  // High bit set
    VG_EXPECT_EQ(clz(u64(0x0000000000000000)), 64);  // Zero case
	#endif
}
#endif

//--------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	#ifdef _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 0;
	#endif

	// Save Startup root directory
	core::io::initRootDirectory();

	// First thing we do is create and set the logger instance
	auto logger = new core::Logger();
	core::Kernel::setLogger(logger);

	#if VG_TEST
	runTests();
	#endif

	core::uint width = 1920, height = 1080;

	if (!CreateGameWindow(hInstance, lpCmdLine, nCmdShow, width, height))
		return 1;

	ShowWindow(g_hWnd, SW_MAXIMIZE);
	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	width = clientRect.right - clientRect.left;
	height = clientRect.bottom - clientRect.top;

	core::CmdLine cmdLine(lpCmdLine);

    g_engine = core::Plugin::create<engine::IEngine>("engine");
	if (nullptr == g_engine)
		return -1;

    engine::EngineCreationParams engineParams;
								 engineParams.renderer.device.resolution = core::uint2(width, height);
								 engineParams.logger = logger;

	#ifdef VG_DX12
	engineParams.renderer.device.api = gfx::API::DirectX12;
	#elif VG_VULKAN
	engineParams.renderer.device.api = gfx::API::Vulkan;
	#else 
	#error No GFX API defined
	#endif

    #ifdef VG_DEBUG
    engineParams.renderer.device.debugDevice = true;
    engineParams.renderer.device.breakOnWarnings = true;
	engineParams.renderer.device.breakOnErrors = true;
    #endif

	bool debugDevice;
	if (cmdLine.getBool("debugDevice", debugDevice))
		engineParams.renderer.device.debugDevice = debugDevice;

    bool breakOnErrors;
    if (cmdLine.getBool("breakOnErrors", breakOnErrors))
        engineParams.renderer.device.breakOnErrors = breakOnErrors;

    bool breakOnWarnings;
    if (cmdLine.getBool("breakOnWarnings", breakOnWarnings))
        engineParams.renderer.device.breakOnWarnings = breakOnWarnings;

    engineParams.renderer.device.window = g_hWnd;
    engineParams.renderer.device.instance = hInstance;

	engineParams.physics.engine = g_engine;

    // MessageBox at application start before engine init
    bool attachDebugger;
    if (cmdLine.getBool("attachDebugger", attachDebugger) && attachDebugger)
        vg::core::messageBox(vg::core::MessageBoxIcon::Info, vg::core::MessageBoxType::OK, "attachDebugger=true", "You can attach debugger now");

	core::string title = fmt::sprintf("VGFramework %s %s build | %s - %s", core::Plugin::getCompiler(), core::Plugin::getConfiguration(), core::Plugin::getPlatform(), core::asString(engineParams.renderer.device.api));
    if (engineParams.renderer.device.debugDevice)
        title += " (debug device)";

	title += fmt::sprintf(" - Version %u.%u.%u", VG_FRAMEWORK_VERSION_MAJOR, VG_FRAMEWORK_VERSION_MINOR, VG_FRAMEWORK_VERSION_PATCH);
	
	#ifdef GIT_REVISION
	title += fmt::sprintf(" (commit %s)", GIT_REVISION);
	#endif

	SetWindowTextA(g_hWnd, title.c_str());

	// 'Debug' and 'Release' targets default with Editor enabled, but 'Final' doesn't.
	// Override command-line with "editor=false" for Debug/Release standalone version or use "editor=true" for a FINAL editor version
	#if VG_FINAL
	bool editor = false;
	#else
	bool editor = true;
	#endif
	cmdLine.getBool("editor", editor);
	engineParams.renderer.editor = editor;

    // Start profiling as soon as possible
    int profileStartFrameCount = -1;
	if (cmdLine.getInt("profileStart", profileStartFrameCount))
		engineParams.renderer.profileStart = true;

    core::Singletons singletons;
    g_engine->Init(engineParams, singletons);
	g_renderer = g_engine->GetRenderer();

    Application * app = new Application(*g_engine);
	auto * profiler = g_renderer->GetProfiler();

    // Start in play mode? In Editor mode the default is not to start in 'Play' mode but in standalone we default to 'Play' mode start
	bool play = !editor;
	cmdLine.getBool("play", play);
    g_engine->StartInPlayMode(play);

    // Start maximized? In Editor mode the default is not to maximize game view but in standalone we default to maximize game view
	bool fullscreen = !editor;
    cmdLine.getBool("fullscreen", fullscreen),
	g_engine->GetRenderer()->SetFullscreen(fullscreen);

	// Command-line override or world name from config
	core::string world;
	if (!cmdLine.getString("world", world))
		world = g_engine->GetOptions()->GetStartWorld();
    if (!world.empty())
        g_engine->LoadWorld(world);

	while (!processSystemMessage() && !g_engine->IsQuitting())
	{
		app->Update();
	
		if (profileStartFrameCount >= 0)
		{
			if (!profileStartFrameCount--)
				profiler->Stop();
		}
	}

    VG_SAFE_DELETE(app);
    g_engine->Deinit();
    VG_SAFE_RELEASE(g_engine);

    core::AutoRegisterClassInfo::unregisterClasses();

	return 0;
}