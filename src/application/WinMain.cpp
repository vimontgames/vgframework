#include "Precomp.h"

#include "core/Plugin/Plugin.h"
#include "core/CmdLine/CmdLine.h"
#include "core/Kernel.h"

#include "engine/IEngine.h"
#include "graphics/renderer/IRenderer.h"
#include "graphics/driver/IDevice.h"

#include "application.h"

HINSTANCE hInst;
HWND g_hWnd;

using namespace vg;
engine::IEngine * g_engine = nullptr;

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
				RECT win;
				GetClientRect(g_hWnd, &win);
				break;
			}
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:
				SendMessage(g_hWnd, WM_DESTROY, 0, 0);
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
	wcex.hIcon = nullptr;
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
	{
		return FALSE;
	}

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

//--------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	#ifdef _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 337;
	#endif

	core::uint width = 1920, height = 1080;

	if (!CreateGameWindow(hInstance, lpCmdLine, nCmdShow, width, height))
		return 1;

	core::CmdLine cmdLine(lpCmdLine);

    g_engine = core::Plugin::create<engine::IEngine>("engine");

    engine::EngineParams engineParams;
                         engineParams.renderer.device.resolution = core::uint2(width, height);

	const core::string * solutionPlatform = cmdLine.find("SolutionPlatform");

	#ifdef VG_DX12
	engineParams.renderer.device.api = graphics::driver::API::DirectX12;
	#elif VG_VULKAN
	engineParams.renderer.device.api = graphics::driver::API::Vulkan;
	#elif 
	#error No GFX API defined
	#endif

    #ifdef VG_DEBUG
    engineParams.renderer.device.debugDevice = false;
    engineParams.renderer.device.breakOnErrors = true;
    engineParams.renderer.device.breakOnWarnings = true;
    #endif

    engineParams.renderer.device.window = g_hWnd;
    engineParams.renderer.device.instance = hInstance;

    core::Singletons singletons;
    g_engine->init(engineParams, singletons);

    Application * game = new Application(*g_engine);

    const auto engineVersion = g_engine->getVersion();
	const auto rendererVersion = g_engine->GetRenderer()->getVersion();
	core::string title = "VG Framework " + core::Plugin::getConfiguration() + " - Engine " + core::to_string(engineVersion.major) + "." + core::to_string(engineVersion.minor) + " - " + core::asString(engineParams.renderer.device.api) + " Renderer " + core::to_string(rendererVersion.major) + "." + core::to_string(rendererVersion.minor);
    if (engineParams.renderer.device.debugDevice)
        title += " (debug device)";

	SetWindowTextA(g_hWnd, title.c_str());

    //core::u64 frame = 0;
	while (!processSystemMessage())
	{
        //SetWindowTextA(g_hWnd, (title + " - Frame " + std::to_string(frame)).c_str());
        //frame++;

        game->update();
	}

    VG_SAFE_DELETE(game);
    g_engine->deinit();
    VG_SAFE_RELEASE(g_engine);

	return 0;
}