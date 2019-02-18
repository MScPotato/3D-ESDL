#pragma once
#include "Camera.h"
#include "ModelHandler.h"
#include "LightHandler.h"

class Application
{
private:
	float width, height;
	float nrOfIndicies;
	HWND hwnd;
	HRESULT hr;


	Constantbuffer constBuffData;
	
	Camera* camera;
	ModelHandler* ObjHandler;
	LightHandler* lightHandler;

	Timer timer;
	double dt = 0.f;

	int boxCounter = 0;
	bool imguiInit = false;
	float size = 1;
	float fx = 0.f;
	float fy = 0.f;
	float fz = 0.f;
	float ax = 0.f;
	float ay = 0.f;
	float az = 0.f;

	void CalcFPS(double dt);
	int fpsCounter = 0;
	float fpsTimer = 0.f;
	int fps = 0.f;

private:
	IDXGISwapChain* gSwapChain;

	// objects to instruct the API what to do.
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;

	// A "view" of a particular resource (the color buffer)
	ID3D11RenderTargetView* gBackbufferRTV;

	// a resource to store Vertices in the GPU
	//ID3D11Buffer* gVertexBuffer;
	ID3D11Buffer* gIndexBuffer;
	ID3D11InputLayout* gVertexLayout;
	ID3D11Buffer* gConstantBuffer;

	// resources that represent shaders
	ID3D11VertexShader* gVertexShader;
	ID3D11GeometryShader* gGeometryShader;
	ID3D11PixelShader* gPixelShader;


	//---------------------------------------
	//--------- Deferred rendering ----------
	//---------------------------------------

	// Deferred Color
	ID3D11InputLayout* gDefVS_Layout;

	ID3D11VertexShader* gDefVS_Color;
	//ID3D11GeometryShader* gDefGS_Color;
	ID3D11PixelShader* gDefPS_Color;

	// Deferred Normal
	//ID3D11VertexShader* gDefVS_Normal;
	//ID3D11GeometryShader* gDefGS_Normal;
	//ID3D11PixelShader* gDefPS_Normal;

	//// Deferred Position
	//ID3D11VertexShader* gDefVS_Position;
	//ID3D11GeometryShader* gDefGS_Position;
	//ID3D11PixelShader* gDefPS_Position;
	//---------------------------------------

	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11SamplerState* gSampleStateWrap;

	ID3D11ShaderResourceView* gTextureSRV;

	ID3D11Buffer* camBuffer;
	void CreateCameraBuffer();
public:
	Application(float width, float height, HWND wndHandle);
	~Application();

	bool initModels();
	//HRESULT CreateDeferredShaders();
	HRESULT CreateDefShaders_Color();
	HRESULT CreateShaders();
	HRESULT CreateDirect3DContext(HWND wndHandle);
	HRESULT CreateConstantbufferDescription();
	HRESULT UpdateConstBuffer();
	HRESULT UpdateCamBuffer();
	HRESULT initiateApplication();
	void Update();
	void SetViewport();
	void Render();
	void ImGuiRender();
	void drawAll();
	void DepthStencil();
	void SetSampleState();
	void SetupImGui();
};

