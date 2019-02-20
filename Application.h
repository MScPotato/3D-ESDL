#pragma once
#include "Camera.h"
#include "ModelHandler.h"
#include "LightHandler.h"

const int NROF_PASSES = 4;

class Application
{
private:
	float width, height;
	HWND hwnd;
	HRESULT hr;


	Constantbuffer constBuffData;
	
	Camera* camera;
	ModelHandler* ObjHandler;
	LightHandler* lightHandler;

	Timer timer;
	double dt = 0.f;

	// ImGui
	int ModelsCounter = 0;
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

	ID3D11Buffer* gQuadBuffer;

	// resources that represent shaders
	ID3D11VertexShader* gVertexShader;
	//ID3D11GeometryShader* gGeometryShader;
	ID3D11PixelShader* gPixelShader;


	//---------------------------------------
	//--------- Deferred rendering ----------
	//---------------------------------------

	ID3D11Texture2D* gDefTex[NROF_PASSES];
	ID3D11RenderTargetView* gDefRTV[NROF_PASSES];
	ID3D11ShaderResourceView* gDefSRV[NROF_PASSES];
	ID3D11InputLayout* gDefVLayout;

	ID3D11VertexShader* gDefVS;
	ID3D11GeometryShader* gDefGS;
	ID3D11PixelShader* gDefPS;

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
	void CreateDefShaders_Color();
	void CreateGBuffers(); // Deferred Tex, RTV, SRV
	void CreateShaders();
	HRESULT CreateDirect3DContext(HWND wndHandle);
	HRESULT CreateConstantbufferDescription();
	HRESULT UpdateConstBuffer();
	HRESULT UpdateCamBuffer();
	void initiateApplication();
	void Update();
	void SetViewport();
	void Render();
	void ImGuiRender();
	void drawAll();
	void DepthStencil();
	void SetSampleState();
	void SetupImGui();

	void CreateQuadBuffer();
};

