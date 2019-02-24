#include "pch.h"
#include "ImGuiManager.h"


ImGuiManager::ImGuiManager(HINSTANCE hInstance)
{
	this->hInstance = hInstance;
	guiDevice = nullptr;
	guiDeviceContext = nullptr;
	guiSwapChain = nullptr;
	guiMainRenderTargetView = nullptr;
	
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = false;
	//io.ConfigDockingTabBarOnSingleWindows = true;
	//io.ConfigDockingTransparentPayload = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(guiDevice, guiDeviceContext);
}


ImGuiManager::~ImGuiManager()
{
}

//void ImGuiManager::Initialize()
//{	
//
//	//// Setup Dear ImGui context
//	//IMGUI_CHECKVERSION();
//	//ImGui::CreateContext();
//	//ImGuiIO& io = ImGui::GetIO(); (void)io;
//	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
//	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
//	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
//	////io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
//	////io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
//	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
//	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI
//	////io.ConfigViewportsNoAutoMerge = true;
//	////io.ConfigViewportsNoTaskBarIcon = false;
//	////io.ConfigDockingTabBarOnSingleWindows = true;
//	////io.ConfigDockingTransparentPayload = true;
//
//	//// Setup Dear ImGui style
//	//ImGui::StyleColorsDark();
//	////ImGui::StyleColorsClassic();
//
//	//// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
//	//ImGuiStyle& style = ImGui::GetStyle();
//	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//	//{
//	//	style.WindowRounding = 0.0f;
//	//	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//	//}
//
//	//// Setup Platform/Renderer bindings
//	//ImGui_ImplWin32_Init(hwnd);
//	//ImGui_ImplDX11_Init(guiDevice, guiDeviceContext);
//}

void ImGuiManager::CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	guiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	guiDevice->CreateRenderTargetView(pBackBuffer, NULL, &guiMainRenderTargetView);
	pBackBuffer->Release();
}

void ImGuiManager::CleanupRenderTarget()
{
	if (guiMainRenderTargetView) 
	{ 
		guiMainRenderTargetView->Release(); 
		guiMainRenderTargetView = NULL;
	}
}

HRESULT ImGuiManager::CreateDevice(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 
		createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, 
		&sd, &guiSwapChain, &guiDevice, &featureLevel, &guiDeviceContext) != S_OK)
		return E_FAIL;

	CreateRenderTarget();

	return S_OK;
}

void ImGuiManager::CleanupDevice()
{
	CleanupRenderTarget();
	if (guiSwapChain)
	{
		guiSwapChain->Release();
		guiSwapChain = NULL;
	}
	if (guiDeviceContext)
	{
		guiDeviceContext->Release();
		guiDeviceContext = NULL;
	}
	if (guiDevice)
	{
		guiDevice->Release();
		guiDevice = NULL;
	}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI ImGuiManager::imWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (guiDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			guiSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int ImGuiManager::main(/*int, char***/)
{
	ImGui_ImplWin32_EnableDpiAwareness();

	// Create application window
	//WNDCLASSEX wc = { 
	//	sizeof(WNDCLASSEX), 
	//	CS_CLASSDC, 
	//	WndProc,
	//	0L, 0L, 
	//	GetModuleHandle(NULL), 
	//	NULL, NULL, NULL, NULL, 
	//	_T("ImGui Example"), NULL };

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = this->hInstance;
	wc.lpszClassName = L"ImGui Example";	
	
	RegisterClassEx(&wc);

	hwnd = CreateWindow(
		wc.lpszClassName,
		L"Dear ImGui DirectX11 Example",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		1280,
		800,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

	//InitWindow(hInstance, 1280, 800);

	//HWND hwnd = CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	
	// Initialize Direct3D
	CreateDevice(hwnd);
	//if (CreateDevice(hwnd) < 0)
	//{
	//	CleanupDevice();
	//	UnregisterClass(wc.lpszClassName, wc.hInstance);
	//	return 1;
	//}

	// Show the window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		guiDeviceContext->OMSetRenderTargets(1, &guiMainRenderTargetView, NULL);
		guiDeviceContext->ClearRenderTargetView(guiMainRenderTargetView, (float*)&clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//{
		//	ImGui::UpdatePlatformWindows();
		//	ImGui::RenderPlatformWindowsDefault();
		//}

		guiSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDevice();
	DestroyWindow(hwnd);
	//UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}


//void ImGuiManager::Begin() {
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplGlfw_NewFrame();
//	ImGui::NewFrame();
//}
//void ImGuiManager::End() {
//	ImGui::Render();
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//	GLFWwindow* backup = glfwGetCurrentContext();
//	ImGui::UpdatePlatformWindows();
//	ImGui::RenderPlatformWindowsDefault();
//	glfwMakeContextCurrent(backup);
//}