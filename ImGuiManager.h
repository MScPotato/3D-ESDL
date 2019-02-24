#pragma once
//#include "imgui.h"
//#include "imgui_impl_win32.h"
//#include "imgui_impl_dx11.h"
//#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

class ImGuiManager
{
private:
	 ID3D11Device*            guiDevice;
	 ID3D11DeviceContext*     guiDeviceContext;
	 IDXGISwapChain*          guiSwapChain;
	 ID3D11RenderTargetView*  guiMainRenderTargetView;
	
	HINSTANCE hInstance;
	HWND hwnd;

public:
	ImGuiManager(HINSTANCE hInstance);
	~ImGuiManager();

	//void Initialize();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	HRESULT CreateDevice(HWND hWnd);
	void CleanupDevice();

	LRESULT WINAPI imWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int main(/*int, char***/);
};