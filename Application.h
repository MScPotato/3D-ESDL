#pragma once
#include "Camera.h"
//#include "ModelHandler.h"
#include "Light_Dir.h"
#include "LightHandler.h"
#include "Terrain.h"
#include "Quadtree.h"
//#include "myGui.h"
const int NROF_PASSES = 4;

const float lightX = 0.f;
const float lightY = 35.f;
const float lightZ = 0.f;

class Application
{
private:
	ID3D11Debug* pDebug;
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
	Quadtree* quadTree;
	Frustum* camFrustum;

	std::vector<Particle> particles;

	// time
	Timer timer;
	double dt = 0.f;

	myGui gui;
	bool topView = false;
private:

	// objects to instruct the API what to do.
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	
	IDXGISwapChain* gSwapChain;
	ID3D11RenderTargetView* gBackbufferRTV; // A "view" of a particular resource (the color buffer)

	
	// Resources to store Vertices in the GPU
	ID3D11InputLayout* gDefVLayout;
	ID3D11InputLayout* gVertexLayout; // used by deferred and quad rendering

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

	ID3D11VertexShader* gDefVS;
	ID3D11GeometryShader* gDefGS;
	ID3D11PixelShader* gDefPS;

	//---------------------------------------
	//---------- Shadow Rendering -----------
	//---------------------------------------
	
	ID3D11VertexShader* SMVertexShader;
	ID3D11PixelShader* SMPixelShader;

	//---------------------------------------
	//---------- Terrain Rendering -----------
	//---------------------------------------

	//ID3D11VertexShader* terrainVertexShader; // deferred vertexsh
	ID3D11GeometryShader* terrainGeometryShader;
	ID3D11PixelShader* terrainPixelShader;

	//---------------------------------------
	//---------- Particles Rendering -----------
	//---------------------------------------

	ID3D11VertexShader* particleVertex;
	ID3D11GeometryShader* particleGeometry;
	ID3D11PixelShader* particlePixel;
	ID3D11ComputeShader* particlesCompute;
	ID3D11InputLayout* particleInputLayout;
	ID3D11Buffer* particlesBuffer;
	ID3D11ShaderResourceView* SRVparticles;
	ID3D11ShaderResourceView* TextureParticle;
	ID3D11UnorderedAccessView * UAVParticles;

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
	void initiateParticles();
	//HRESULT CreateDeferredShaders();
	void CreateShadowShader();
	void CreateTerrainShaders();
	void CreateDefShaders();
	void CreateParticleShaders();
	void CreateParticleComputeShader();
	void CreateParticleBuffer();
	void updateParticles();
	void CreateGBuffers(); // Deferred Tex, RTV, SRV
	void CreateShaders();
	HRESULT CreateDirect3DContext(HWND wndHandle);
	HRESULT CreateConstantbufferDescription();
	HRESULT UpdateCameraView();
	//HRESULT UpdateLightView();
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

