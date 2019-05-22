// 3D Programming Project. Group members:
// Emil Svensson & David Larsson 
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "Application.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


#define WIDTH 1024
#define HEIGHT 768

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"CoInitialize Failed!", L"ERROR", MB_OK);
		return 0;
	}

	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance, WIDTH, HEIGHT); //1. Skapa fönster
	Application app = Application(WIDTH, HEIGHT, wndHandle);
	
	if (wndHandle)
	{
		app.initiateApplication();

		ShowWindow(wndHandle, nCmdShow);

		while (WM_QUIT != msg.message && !GetAsyncKeyState(VK_ESCAPE))
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				app.Update();
				app.Render(); //8. Rendera
			}
		}

		DestroyWindow(wndHandle);
	}

	return (int) msg.wParam;
}
