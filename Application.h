#pragma once
#include "Camera.h"
#include "ModelHandler.h"
#include "Light_Dir.h"
#include "LightHandler.h"
#include "Terrain.h"
//#include "myGui.h"
const int NROF_PASSES = 4;

class Application
{
private:
	//ImGuiManager* ImGuiManager;
	// foundation
	float width, height;
	HWND hwnd;
	HRESULT hr;
	Constantbuffer constBuffData;
	
	// class objs
	Camera* camera;
	Terrain* wTerrain;
	ModelHandler* ObjHandler;
	Light_Dir* sunLight;
	LightHandler* lightHandler;

	// time
	Timer timer;
	double dt = 0.f;

	myGui gui;

private:

	// objects to instruct the API what to do.
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	
	IDXGISwapChain* gSwapChain;
	ID3D11RenderTargetView* gBackbufferRTV; // A "view" of a particular resource (the color buffer)

	// a resource to store Vertices in the GPU
	ID3D11InputLayout* gVertexLayout;
	ID3D11Buffer* gConstantBuffer;
	ID3D11Buffer* gQuadBuffer;

	//---------------------------------------
	//--------- Default rendering ----------
	//---------------------------------------

	ID3D11VertexShader* gVertexShader;
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
	//---------- Shadow Rendering -----------
	//---------------------------------------
	
	ID3D11VertexShader* SMVertexShader;
	ID3D11PixelShader* SMPixelShader;

	//---------------------------------------

	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11SamplerState* gSampleStateWrap;
	ID3D11SamplerState* gSampleStateClamp;

	ID3D11ShaderResourceView* gTextureSRV;

	// Camera
	ID3D11Buffer* camBuffer;
	ID3D11ShaderResourceView* camDepth_SRV;
	// Terrain/Camera 
	float terrainY = 0.f;
public:
	Application(float width, float height, HWND wndHandle);
	~Application();

	void initTerrain();
	bool initModels();
	//HRESULT CreateDeferredShaders();
	void CreateShadowShader();
	void CreateDefShaders();
	void CreateGBuffers(); // Deferred Tex, RTV, SRV
	void CreateShaders();
	HRESULT CreateDirect3DContext(HWND wndHandle);
	HRESULT CreateConstantbufferDescription();
	HRESULT UpdateConstBuffer(XMFLOAT4X4 view);
	void initiateApplication();
	void Update();
	void TerrainWalk();
	void Render();
	void RenderImGui();
	void drawScene(bool shadow = false);
	void SetSampleState();
	void SetupImGui();

	void CreateQuadBuffer(); // Finished frame quad
	void DepthStencil();
	HRESULT UpdateCamBuffer();
	void CreateCameraBuffer();
	void SetViewport();

};

