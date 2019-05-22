#include "pch.h"
#include "Application.h"

Application::Application(float width, float height, HWND wndHandle)
{
	

	timer.Start();
	this->width = width;
	this->height = height;
	this->hwnd = wndHandle;

	gSwapChain = nullptr;
	gDevice = nullptr;
	gDeviceContext = nullptr;
	gBackbufferRTV = nullptr;
	//gIndexBuffer = nullptr;
	gVertexLayout = nullptr;
	gVertexShader = nullptr;
	//gGeometryShader = nullptr;
	gPixelShader = nullptr;
	gSampleStateWrap = nullptr;
	gTextureSRV = nullptr;
	gQuadBuffer = nullptr;

	gConstantBuffer = nullptr;
	gDefVS = nullptr;
	gDefGS = nullptr;
	gDefPS = nullptr;

	depthStencilView = nullptr;
	depthStencilBuffer = nullptr;

	particlesBuffer = nullptr;
	SRVparticles = nullptr;
	TextureParticle = nullptr;
	UAVParticles = nullptr;

	camera = new Camera(width, height, wndHandle);
	XMStoreFloat4x4(&constBuffData.world, XMMatrixIdentity());
	constBuffData.view = camera->getView();
	XMStoreFloat4x4(&constBuffData.projection, XMMatrixPerspectiveFovLH(XM_PI*0.45, (this->width / this->height), 0.1, 75));
	//XMStoreFloat4x4(&constBuffData.projection, XMMatrixOrthographicLH(this->width, this->height, 1.f, 2.5f));
	
	ObjHandler = new ModelHandler();
	lightHandler = new LightHandler();
	sunLight = new Light_Dir(XMFLOAT3(lightX, lightY, lightZ));// , XMFLOAT3(0.f, 4.9f, -1.f), width, height);
	wTerrain = new Terrain();
	quadTree = new Quadtree();
	camFrustum = new Frustum();

	
}

Application::~Application() // REMEMBER TO RELEASE ALL COM OBJs
{
	//SRVparticles->Release();
	//UAVParticles->Release();
	//particlesBuffer->Release();

	delete ObjHandler;
	delete lightHandler;
	delete sunLight;
	delete wTerrain;
	delete quadTree;
	delete camFrustum;

	//---------------------------------------
	//---------- Particles Rendering -----------
	//---------------------------------------

	UAVParticles->Release();
	TextureParticle->Release();
	SRVparticles->Release();
	particlesBuffer->Release();
	particleInputLayout->Release();
	particlesCompute->Release();
	particlePixel->Release();
	particleGeometry->Release();
	particleVertex->Release();

	//---------------------------------------
	//--------- Deferred rendering ----------
	//---------------------------------------

	gDefVLayout->Release();
	
	for (int i = 0; i < NROF_PASSES; i++) {
		gDefTex[i]->Release();
		gDefRTV[i]->Release();
		gDefSRV[i]->Release();
	}

	gDefPS->Release();
	gDefGS->Release();
	gDefVS->Release();
	//---------- Terrain Rendering ----------
	terrainPixelShader->Release();
	terrainGeometryShader->Release();
	//---------- Shadow Rendering -----------
	SMPixelShader->Release();
	SMVertexShader->Release();

	gQuadBuffer->Release();
	gVertexLayout->Release();
	gConstantBuffer->Release();
	//--------- Default rendering ----------
	gPixelShader->Release();
	gVertexShader->Release();
	//---------------------------------------	
	gSampleStateClamp->Release();
	gSampleStateWrap->Release();
	depthStencilBuffer->Release();
	depthStencilView->Release();
	
	gBackbufferRTV->Release();
	gSwapChain->Release();
	gDeviceContext->Release();
	gDevice->Release();

	//gTextureSRV->Release();
	if (pDebug != nullptr)
	{
		pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		//pDebug->ReportLiveObjects(D3D11_RLDO_DETAIL);
		pDebug = nullptr;
	}
}

void Application::initiateApplication()
{
	hr = CreateDirect3DContext(hwnd); //2. Skapa och koppla SwapChain, Device och Device Context
	if (FAILED(hr))
		MessageBox(NULL, L"CRITICAL ERROR: Could not create 3DContext", L"ERROR", MB_OK);
	
	CreateConstantbufferDescription();
	initiateParticles();
	CreateParticleBuffer();
	CreateCameraBuffer();
	CreateQuadBuffer();
	SetViewport(); //3. S�tt viewport
	SetSampleState();
	SetupImGui();

	CreateGBuffers();
	CreateShaders(); //4. Skapa vertex- och pixel-shaders
	CreateTerrainShaders();
	CreateDefShaders();
	CreateShadowShader();
	CreateParticleShaders();
	
	initTerrain();
	initModels();
	
}

void Application::initTerrain()
{
	wTerrain->initTerrain(gDevice, gDeviceContext);
	wTerrain->loadHeightmap();
	wTerrain->BuildQuadPatchVB();
	wTerrain->BuildQuadPatchIB();
}

bool Application::initModels()
{
	ObjHandler->Init(gDevice, gDeviceContext, gTextureSRV, gConstantBuffer);
	lightHandler->Init(gDevice, gDeviceContext);
	sunLight->InitDirLight(gDevice, gDeviceContext);

	//ObjHandler->addModel(0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, L"candysun.obj");
	//ObjHandler->addModel(-1.5, -3.0, -5.5, 1.0, 0.0, 0.0, 0.0, L"candyblue.obj");

	//lightHandler->addLight(XMFLOAT3(7, 27.5, -5), XMFLOAT4(1, 1, 1, 1));
	//lightHandler->addLight(XMFLOAT3(-5, 4.9, 3), XMFLOAT4(1, 0, 0, 1));
	//lightHandler->addLight(XMFLOAT3(0, -3, 0), XMFLOAT4(0, 1, 0, 3));
	//lightHandler->addLight(XMFLOAT3(4, 5, -5), XMFLOAT4(0, 0, 1, 1));
	//lightHandler->addLight(XMFLOAT3(0, 0, 3), XMFLOAT4(1, 0, 1, 3));
	//lightHandler->addLight(XMFLOAT3(3, 0, 0), XMFLOAT4(1, 1, 0, 3));
	//lightHandler->addLight(XMFLOAT3(-3, 0, 0), XMFLOAT4(0, 1, 1, 3));

	//lightHandler->CreateLightPosBuffer();
	//lightHandler->CreateLightRGBABuffer();

	//test f�r olika texture
	//ObjHandler->addSphere(1.f, 4.9f, 1.f);
	//ObjHandler->addCube(1.f, 4.9f, 15.f);
	//ObjHandler->addCube(1.f, 4.9f, 7.f);
	//ObjHandler->addCube(-5.f, 4.9f, 1.f);	
	//ObjHandler->addCube(-1.f, 4.9f, -7.f);

	//level 1
	ObjHandler->addSphere(-15.f, 4.9f, -15.f);

	//level 2
	ObjHandler->addCube(7.f, 4.9f, -23.f);
	ObjHandler->addCube(7.f, 4.9f, -7.f);

	ObjHandler->addCube(-7.f, 4.9f, 7.f);
	ObjHandler->addCube(-7.f, 4.9f, 23.f);
	
	//level 3
	ObjHandler->addCube(11.f, 4.9f, 11.f);
	ObjHandler->addCube(11.f, 4.9f, 3.f);
	ObjHandler->addCube(3.f, 4.9f, 3.f);
	//ObjHandler->addCube(4.f, 4.9f, 12.f);

	//level 4
	ObjHandler->addCube(3.f, 4.9f, 11.f);
	ObjHandler->addCube(5.f, 4.9f, 11.f);


	
	//ObjHandler->addSphere(lightX, lightY, lightZ, 0.1);
	//ObjHandler->addCube();
	//3x trains
	//ObjHandler->addModel(-1, 0, 0, 0.5, L"steyerdorf.obj");
	//ObjHandler->addModel(0, 0, 0, 1, 0, 0, 0, L"steyerdorf.obj");
	//ObjHandler->addModel(constBuffData.world, 5, 0, -1, 1, 0, -90, 0, L"steyerdorf.obj");

	quadTree->InitTree(gDeviceContext, gTextureSRV, gConstantBuffer, ObjHandler, 64);

	return true;
}

void Application::initiateParticles()
{
	CHECK_HR(CreateWICTextureFromFile(gDevice, L"Textures\\snowflake_white.png", nullptr, &TextureParticle));
	if (FAILED(hr))
	{
		CHECK_HR(CreateWICTextureFromFile(gDevice, L"Textures\\Default.jpg", nullptr, &TextureParticle));
		MessageBox(NULL, L"Loading texture Failed!", L"ERROR", MB_OK);
	}

	//---------------------------------------------
	
	Particle temp;

	for (int i = 0; i < 1000; i++)
	{
		temp.pos = XMFLOAT3(Math::RandomInt(-32, 32), Math::RandomInt(3, 20), Math::RandomInt(-32, 32));
		particles.push_back(temp);
	}	
}

void Application::Update()
{
	if (GetAsyncKeyState('3'))
		topView = true;
	if (GetAsyncKeyState('4'))
		topView = false;

	dt = timer.GetMillisecondsElapse();
	timer.Restart();

	ObjHandler->update();
	TerrainWalk();
	camera->UpdateCamera(dt);

	//UpdateConstBuffer();
	UpdateCamBuffer();
	camFrustum->ConstructFrustum(75, constBuffData.projection, camera->getView());
	sunLight->Update();
	//updateParticles();

	gui.CalcFPS(dt);
}

void Application::TerrainWalk()
{
	if (!camera->getFlying())
	{
		
		terrainY = wTerrain->getY(camera->getPosition().x, camera->getPosition().z);
		if (terrainY > 0)
		{
			camera->Walk(terrainY);
		}
		else
		{
			camera->respawn();
		}
	}
}
void Application::CreateShadowShader()
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	/** SM Vertex Shader **/

	CHECK_HR(D3DCompileFromFile(
		L"SMVertex.hlsl",		// filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"VS_main",			// entry point
		"vs_5_0",			// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,					// IGNORE...DEPRECATED.
		&pVS,				// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	));

	CHECK_HR(gDevice->CreateVertexShader(
		pVS->GetBufferPointer(),
		pVS->GetBufferSize(),
		nullptr,
		&SMVertexShader
	));

	/** SM Pixel Shader **/
	ID3DBlob* pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;
	CHECK_HR(D3DCompileFromFile(
		L"SMPixel.hlsl",		// filename
		nullptr,				// optional macros
		nullptr,				// optional include files
		"PS_main",				// entry point
		"ps_5_0",				// shader model (target)
		D3DCOMPILE_DEBUG,		// shader compile options
		0,						// effect compile options
		&pPS,					// double pointer to ID3DBlob		
		&errorBlob				// pointer for Error Blob messages.
	));

	CHECK_HR(gDevice->CreatePixelShader(
		pPS->GetBufferPointer(),
		pPS->GetBufferSize(),
		nullptr, &SMPixelShader));
	pPS->Release();
}


void Application::CreateTerrainShaders()
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	ID3DBlob* errorBlob = nullptr;

	// Uses deferred vertex shader
	
	/** Geometry Shader **/
	ID3DBlob* pGS = nullptr;

	CHECK_HR(D3DCompileFromFile(
		L"TerrainGeometry.hlsl",
		nullptr,
		nullptr,
		"GS_main",
		"gs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&pGS,
		&errorBlob
	));

	CHECK_HR(gDevice->CreateGeometryShader(
		pGS->GetBufferPointer(),
		pGS->GetBufferSize(),
		nullptr,
		&terrainGeometryShader));

	/** Pixel Shader **/
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;
	CHECK_HR(D3DCompileFromFile(
		L"TerrainPixel.hlsl",		// filename
		nullptr,				// optional macros
		nullptr,				// optional include files
		"PS_main",				// entry point
		"ps_5_0",				// shader model (target)
		D3DCOMPILE_DEBUG,		// shader compile options
		0,						// effect compile options
		&pPS,					// double pointer to ID3DBlob		
		&errorBlob				// pointer for Error Blob messages.
	));

	CHECK_HR(gDevice->CreatePixelShader(
		pPS->GetBufferPointer(),
		pPS->GetBufferSize(),
		nullptr, &terrainPixelShader));
	pPS->Release();
}

void Application::CreateDefShaders()
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	/** Vertex Shader **/

	CHECK_HR(D3DCompileFromFile(
		L"DefVertex.hlsl",		// filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"VS_main",			// entry point
		"vs_5_0",			// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,					// IGNORE...DEPRECATED.
		&pVS,				// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	));

	// compilation failed?
	//if (FAILED(hr))
	//{
	//	if (errorBlob)
	//	{
	//		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	//		errorBlob->Release();
	//	}
	//	if (pVS)
	//		pVS->Release();
	//	return hr;
	//}

	CHECK_HR(gDevice->CreateVertexShader(
		pVS->GetBufferPointer(),
		pVS->GetBufferSize(),
		nullptr,
		&gDefVS
	));

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{
			"POSITION",					 // "semantic" name in shader
			0,							 // "semantic" index (not used)
			DXGI_FORMAT_R32G32B32_FLOAT, // size of ONE element (3 floats)
			0,							 // input slot
			0,							 // offset of first element
			D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
			0							 // used for INSTANCING (ignore)
		},
		{
			"TEXCOORD",
			0,							 // same slot as previous (same vertexBuffer)
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,							 // offset of FIRST element (after POSITION)
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,							 // same slot as previous (same vertexBuffer)
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,							 // offset of FIRST element (after POSITION)
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	CHECK_HR(gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gDefVLayout));
	pVS->Release();

	/** Geometry Shader **/
	ID3DBlob* pGS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	CHECK_HR(D3DCompileFromFile(
		L"DefGeometry.hlsl",		// filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"GS_main",			// entry point
		"gs_5_0",			// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,					// IGNORE...DEPRECATED.
		&pGS,				// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	));

	CHECK_HR(gDevice->CreateGeometryShader(
		pGS->GetBufferPointer(),
		pGS->GetBufferSize(),
		nullptr,
		&gDefGS));

	/** Pixel Shader **/
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;
	CHECK_HR(D3DCompileFromFile(
		L"DefPixel.hlsl",		// filename
		nullptr,				// optional macros
		nullptr,				// optional include files
		"PS_main",				// entry point
		"ps_5_0",				// shader model (target)
		D3DCOMPILE_DEBUG,		// shader compile options
		0,						// effect compile options
		&pPS,					// double pointer to ID3DBlob		
		&errorBlob				// pointer for Error Blob messages.
	));

	CHECK_HR(gDevice->CreatePixelShader(
		pPS->GetBufferPointer(),
		pPS->GetBufferSize(),
		nullptr, &gDefPS));
	pPS->Release();	
}


void Application::CreateParticleShaders()
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	/** Vertex Shader **/

	CHECK_HR(D3DCompileFromFile(
		L"ParticleVertex.hlsl",		// filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"VS_main",			// entry point
		"vs_5_0",			// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,					// IGNORE...DEPRECATED.
		&pVS,				// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	));

	CHECK_HR(gDevice->CreateVertexShader(
		pVS->GetBufferPointer(),
		pVS->GetBufferSize(),
		nullptr,
		&particleVertex
	));

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
	{
		"SV_VertexID",					// "semantic" name in shader
		0,							 // "semantic" index (not used)
		DXGI_FORMAT_R32_UINT,		 // size of ONE element (1 uint)
		0,							 // input slot
		0,							 // offset of first element
		D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
		0							 // used for INSTANCING (ignore)
	}
	};

	//D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
	//	{
	//		"POSITION",					// "semantic" name in shader
	//		0,							 // "semantic" index (not used)
	//		DXGI_FORMAT_R32G32B32_FLOAT,		 // size of ONE element (1 uint)
	//		0,							 // input slot
	//		0,							 // offset of first element
	//		D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
	//		0							 // used for INSTANCING (ignore)
	//	}
	//};

	CHECK_HR(gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &particleInputLayout));
	pVS->Release();

	/** Geometry Shader **/
	ID3DBlob* pGS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	CHECK_HR(D3DCompileFromFile(
		L"ParticleGeometry.hlsl",		// filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"GS_main",			// entry point
		"gs_5_0",			// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,					// IGNORE...DEPRECATED.
		&pGS,				// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	));

	CHECK_HR(gDevice->CreateGeometryShader(
		pGS->GetBufferPointer(),
		pGS->GetBufferSize(),
		nullptr,
		&particleGeometry));

	/** Pixel Shader **/
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;
	CHECK_HR(D3DCompileFromFile(
		L"ParticlePixel.hlsl",		// filename
		nullptr,				// optional macros
		nullptr,				// optional include files
		"PS_main",				// entry point
		"ps_5_0",				// shader model (target)
		D3DCOMPILE_DEBUG,		// shader compile options
		0,						// effect compile options
		&pPS,					// double pointer to ID3DBlob		
		&errorBlob				// pointer for Error Blob messages.
	));

	CHECK_HR(gDevice->CreatePixelShader(
		pPS->GetBufferPointer(),
		pPS->GetBufferSize(),
		nullptr, &particlePixel));
	pPS->Release();

	CreateParticleComputeShader();
}

void Application::CreateParticleComputeShader()
{
	ID3DBlob* pCS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	CHECK_HR(D3DCompileFromFile(
		L"ParticleCompute.hlsl",
		nullptr, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"CS_main", 
		"cs_5_0",
		D3DCOMPILE_DEBUG,
		0, 
		&pCS,
		&errorBlob));

	CHECK_HR(gDevice->CreateComputeShader(
		pCS->GetBufferPointer(),
		pCS->GetBufferSize(),
		nullptr, &particlesCompute));
}

void Application::CreateShaders()
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	/** Vertex Shader **/

	CHECK_HR(D3DCompileFromFile(
		L"Vertex.hlsl",		// filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"VS_main",			// entry point
		"vs_5_0",			// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,					// IGNORE...DEPRECATED.
		&pVS,				// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	));

	// compilation failed?
	//if (FAILED(hr))
	//{
	//	if (errorBlob)
	//	{
	//		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	//		errorBlob->Release();
	//	}
	//	if (pVS)
	//		pVS->Release();
	//	return hr;
	//}

	CHECK_HR(gDevice->CreateVertexShader(
		pVS->GetBufferPointer(),
		pVS->GetBufferSize(),
		nullptr,
		&gVertexShader
	));

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{
			"POSITION",					 // "semantic" name in shader
			0,							 // "semantic" index (not used)
			DXGI_FORMAT_R32G32B32_FLOAT, // size of ONE element (3 floats)
			0,							 // input slot
			0,							 // offset of first element
			D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
			0							 // used for INSTANCING (ignore)
		},
		{
			"TEXCOORD",
			0,							 // same slot as previous (same vertexBuffer)
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,							 // offset of FIRST element (after POSITION)
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	CHECK_HR(gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout));
	//if (FAILED(hr))
	//	MessageBox(NULL, L"CRITICAL ERROR: Creating inputlayout", L"ERROR", MB_OK);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();



	/** Geometry Shader **/
	//ID3DBlob* pGS = nullptr;
	//if (errorBlob) errorBlob->Release();
	//errorBlob = nullptr;

	//hr = D3DCompileFromFile(
	//	L"Geometry.hlsl",		// filename
	//	nullptr,			// optional macros
	//	nullptr,			// optional include files
	//	"GS_main",			// entry point
	//	"gs_5_0",			// shader model (target)
	//	D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
	//	0,					// IGNORE...DEPRECATED.
	//	&pGS,				// double pointer to ID3DBlob		
	//	&errorBlob			// pointer for Error Blob messages.
	//);

	//// compilation failed?
	//if (FAILED(hr))
	//{
	//	if (errorBlob)
	//	{
	//		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	//		errorBlob->Release();
	//	}
	//	if (pGS)
	//		pGS->Release();
	//	return hr;
	//}

	//gDevice->CreateGeometryShader(
	//	pGS->GetBufferPointer(),
	//	pGS->GetBufferSize(),
	//	nullptr,
	//	&gGeometryShader);

	/** Pixel Shader **/
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;
	CHECK_HR(D3DCompileFromFile(
		L"Pixel.hlsl",		// filename
		nullptr,				// optional macros
		nullptr,				// optional include files
		"PS_main",				// entry point
		"ps_5_0",				// shader model (target)
		D3DCOMPILE_DEBUG,		// shader compile options
		0,						// effect compile options
		&pPS,					// double pointer to ID3DBlob		
		&errorBlob				// pointer for Error Blob messages.
	));

	// compilation failed?
	//if (FAILED(hr))
	//{
	//	if (errorBlob)
	//	{
	//		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	//		errorBlob->Release();
	//	}
	//	if (pPS)
	//		pPS->Release();
	//	return hr;
	//}

	CHECK_HR(gDevice->CreatePixelShader(
		pPS->GetBufferPointer(),
		pPS->GetBufferSize(),
		nullptr, &gPixelShader));
	pPS->Release();

	//DepthStencil();

	//return hr;
}

void Application::CreateParticleBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(Particle) * particles.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(Particle);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = particles.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	CHECK_HR(gDevice->CreateBuffer(&desc, &data, &particlesBuffer));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc;
	SRVdesc.Format = DXGI_FORMAT_UNKNOWN;

	SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVdesc.Buffer.ElementOffset = 0;
	SRVdesc.Buffer.ElementWidth = 1000; // sizeof(Particle);

	//ID3D11ShaderResourceView* pView = 0;
	CHECK_HR(gDevice->CreateShaderResourceView(particlesBuffer, &SRVdesc, &SRVparticles));

	//gDeviceContext->VSSetShaderResources(0, 1, &SRVparticles);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
	UAVdesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVdesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	UAVdesc.Buffer.FirstElement = 0;
	UAVdesc.Buffer.NumElements = 1000;
	CHECK_HR(gDevice->CreateUnorderedAccessView(particlesBuffer, &UAVdesc, &UAVParticles));

	return;
}

//Gammal och om vi kan f� ^ att funka s� �r den on�dig
//void Application::CreateParticleBuffer()
//{
//	D3D11_BUFFER_DESC desc;
//	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
//	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	desc.Usage = D3D11_USAGE_IMMUTABLE;
//	desc.ByteWidth = sizeof(Particle) * particles.size();
//	desc.CPUAccessFlags = 0;
//	desc.MiscFlags = 0;
//	//desc.StructureByteStride = sizeof(Particle);
//
//	D3D11_SUBRESOURCE_DATA data;
//	data.pSysMem = particles.data();
//	data.SysMemPitch = 0;
//	data.SysMemSlicePitch = 0;
//	
//	CHECK_HR(gDevice->CreateBuffer(&desc, &data, &particlesBuffer));
//}

void Application::updateParticles()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = gDeviceContext->Map(particlesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &particles, sizeof(Particle));
	gDeviceContext->Unmap(particlesBuffer, 0);
	//gDeviceContext->VSSet(0, 1, &particlesBuffer);
}

HRESULT Application::CreateConstantbufferDescription()
{
	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(Constantbuffer);
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.MiscFlags = 0;
	cbufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &constBuffData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&cbufferDesc, &data, &gConstantBuffer);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBuffer);
	return hr;
}

void Application::SetSampleState()
{
	// samplestate once, directx
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	hr = gDevice->CreateSamplerState(&sampDesc, &gSampleStateWrap);
	gDeviceContext->PSSetSamplers(0, 1, &gSampleStateWrap);

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hr = gDevice->CreateSamplerState(&sampDesc, &gSampleStateClamp);
	gDeviceContext->PSSetSamplers(1, 1, &gSampleStateClamp);
}

HRESULT Application::UpdateCameraView()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = gDeviceContext->Map(gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (topView)
		constBuffData.view = sunLight->getView();
	else
		constBuffData.view = camera->getView();
	//Constantbuffer* dataPtr = (Constantbuffer*)mappedResource.pData;
	memcpy(mappedResource.pData, &constBuffData, sizeof(Constantbuffer));
	gDeviceContext->Unmap(gConstantBuffer, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBuffer);
	return hr;
}



void Application::Render()
{
	// ------------------------------------------------------------------------
	// Clear colors and depth buffers

	// clear the back buffer to a deep blue
	float clearColor[] = { 0.2, 0.3, 0.3, 1 };

	for (int i = 0; i < 4; i++)
	{
		gDeviceContext->ClearRenderTargetView(gDefRTV[i], clearColor);
	}
	gDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	gDeviceContext->ClearDepthStencilView(sunLight->getShadowDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ------------------------------------------------------------------------
	// null all shaders

	// specifying NULL or nullptr we are disabling that stage
	// in the pipeline
	gDeviceContext->VSSetShader(nullptr, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(nullptr, nullptr, 0);

	// ------------------------------------------------------------------------
	//** Shadowmapping Rendering **//

	gDeviceContext->IASetInputLayout(gDefVLayout);
	gDeviceContext->VSSetShader(SMVertexShader, nullptr, 0);
	gDeviceContext->PSSetShader(SMPixelShader, nullptr, 0);
	gDeviceContext->OMSetRenderTargets(0, nullptr, sunLight->getShadowDSV());
	sunLight->UpdateLightData();
	drawScene();	

	// ------------------------------------------------------------------------
	//** Terrain Rendering **//

	gDeviceContext->VSSetShader(gDefVS, nullptr, 0);
	gDeviceContext->GSSetShader(terrainGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(terrainPixelShader, nullptr, 0);
	gDeviceContext->IASetInputLayout(gDefVLayout);
	
	gDeviceContext->OMSetRenderTargets(NROF_PASSES, gDefRTV, depthStencilView);

	wTerrain->setTerrainMTL();
	wTerrain->draw(constBuffData, gConstantBuffer, false);

	// ------------------------------------------------------------------------
	//** Deferred Rendering **//	

	gDeviceContext->VSSetShader(gDefVS, nullptr, 0);
	gDeviceContext->GSSetShader(gDefGS, nullptr, 0);
	gDeviceContext->PSSetShader(gDefPS, nullptr, 0);

	// specify the IA Layout (how is data passed)
	gDeviceContext->IASetInputLayout(gDefVLayout);
	//SetViewport();
	UpdateCameraView();
	drawScene();

	// ------------------------------------------------------------------------
	//** Quad Rendering **//
	UINT quadSize = sizeof(float) * 5;
	UINT offset = 0;
	
	ID3D11RenderTargetView* pNullRTV[] = { NULL };
	ID3D11DepthStencilView* pNullDSV = nullptr;
	gDeviceContext->OMSetRenderTargets(1, pNullRTV, pNullDSV);

	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, nullptr);
	gDeviceContext->IASetVertexBuffers(0, 1, &gQuadBuffer, &quadSize, &offset);

	float errorColor[] = { 1.0, 0.0, 0.0, 1 };
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, errorColor);
	
	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	//gDeviceContext->HSSetShader(nullptr, nullptr, 0); <-not changed
	//gDeviceContext->DSSetShader(nullptr, nullptr, 0); <-not changed
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	for (int i = 0; i < NROF_PASSES; i++) {
		gDeviceContext->PSSetShaderResources(i, 1, &gDefSRV[i]);
	}

	ID3D11ShaderResourceView* pSRV = sunLight->getShadowSRV();
	gDeviceContext->PSSetShaderResources(NROF_PASSES, 1, &camDepth_SRV);
	gDeviceContext->PSSetShaderResources(NROF_PASSES + 1, 1, &pSRV);

	gDeviceContext->IASetInputLayout(gVertexLayout);
	gDeviceContext->Draw(6, 0);
	RenderImGui();

	//gDeviceContext->IASetVertexBuffers(0, 0, nullptr, 0, 0); //test

	// ------------------------------------------------------------------------
	//** Particles Rendering **//
	//quadSize = sizeof(Particle);
	//offset = 0;
	//gDeviceContext->IASetVertexBuffers(0, 1, &particlesBuffer, &quadSize, &offset);

	//ID3D11RenderTargetView* pNullRTV[] = { NULL };
	//gDeviceContext->OMSetRenderTargets(NROF_PASSES, pNullRTV, nullptr);

	ID3D11Buffer* buffers[] = { nullptr };
	UINT strides[] = { NULL };
	UINT offsets[] = { NULL };
	gDeviceContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);

	ID3D11ShaderResourceView* pNullSRV[] = { nullptr };
	for (int i = 0; i < NROF_PASSES + 2; i++)
		gDeviceContext->PSSetShaderResources(i, 1, pNullSRV);

	//for (int i = 0; i < NROF_PASSES+2; i++) {
	//	gDeviceContext->PSSetShaderResources(i, 1, nullptr);
	//}


	gDeviceContext->IASetInputLayout(nullptr);
	gDeviceContext->CSSetShader(particlesCompute, nullptr, 0);
	gDeviceContext->CSSetUnorderedAccessViews(0, 1, &UAVParticles, nullptr);
	gDeviceContext->Dispatch(10, 1, 1); // 10 * 100

	ID3D11UnorderedAccessView* UAVViewNULL[1] = { nullptr };
	gDeviceContext->CSSetUnorderedAccessViews(0, 1, UAVViewNULL, nullptr);

	gDeviceContext->VSSetShader(particleVertex, nullptr, 0);
	gDeviceContext->GSSetShader(particleGeometry, nullptr, 0);
	gDeviceContext->PSSetShader(particlePixel, nullptr, 0);
	gDeviceContext->IASetInputLayout(particleInputLayout);
	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, depthStencilView);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// resource after shader settings
	gDeviceContext->VSSetShaderResources(0, 1, &SRVparticles);
	gDeviceContext->PSSetShaderResources(0, 1, &TextureParticle);

	gDeviceContext->Draw(1000, 0);
	gDeviceContext->VSSetShaderResources(0, 1, pNullSRV);


	////ComputeShader.SetUnorderedAccessView(TRIANGLE_SLOT, null); // too clean?
	//gDeviceContext->CSSetShader(nullptr, nullptr, 0);

	//gDeviceContext->DrawInstancedIndirect(particlesBuffer, 0);
	//gDeviceContext->Draw(particles.size(), 0);

	// ------------------------------------------------------------------------
	//RenderImGui();

	// Present the backbuffer / present the finished image quad
	gSwapChain->Present(0, 0); //9. V�xla front- och back-buffer

}

void Application::RenderImGui()
{
	gui.Update(ObjHandler, camera->getPosition(), gDefSRV, camDepth_SRV, sunLight->getShadowSRV(), SRVparticles);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Application::drawScene(bool shadow)
{
		quadTree->render(camFrustum, constBuffData, shadow);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		gDeviceContext->Map(gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		XMStoreFloat4x4(&constBuffData.world, XMMatrixIdentity());
		memcpy(mappedResource.pData, &constBuffData, sizeof(Constantbuffer));
		gDeviceContext->Unmap(gConstantBuffer, 0);
		gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBuffer);
}

HRESULT Application::CreateDirect3DContext(HWND wndHandle)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1,
	};

	// create a device, device context and swap chain using the information in the scd struct
	hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		levels,
		1,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();

		// set the render target as the back buffer
		DepthStencil();
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, depthStencilView);

	}
	CHECK_HR(gDevice->QueryInterface(IID_PPV_ARGS(&pDebug)));	

	return hr;
}

void Application::CreateGBuffers() // Deferred Shading
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = 0;
	texDesc.CPUAccessFlags = 0;

	for (int i = 0; i < NROF_PASSES; i++)
	{
		gDevice->CreateTexture2D(&texDesc, NULL, &gDefTex[i]);
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < NROF_PASSES; i++)
	{
		CHECK_HR(gDevice->CreateRenderTargetView(gDefTex[i], &rtvDesc, &gDefRTV[i]));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	for (int i = 0; i < NROF_PASSES; i++)
	{
		CHECK_HR(gDevice->CreateShaderResourceView(gDefTex[i], &srvDesc, &gDefSRV[i]));
	}

}

void Application::SetupImGui()
{
	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(gDevice, gDeviceContext);
	ImGui::StyleColorsDark();
}

void Application::CreateQuadBuffer()
{
	struct VertexData
	{
		float x, y, z;
		float u, v;
	};

	VertexData quadVertex[6] =
	{
		-1.f, 1.f, 0.f,
		0.f, 0.f,

		1.f, 1.f, 0.f,
		1.f, 0.f,

		1.f, -1.f, 0.f,
		1.f, 1.f,

		-1.f, 1.f, 0.f,
		0.f, 0.f,

		1.f, -1.f, 0.f,
		1.f, 1.f,

		-1.f, -1.f, 0.f,
		0.f, 1.f
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(quadVertex);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = quadVertex;
	CHECK_HR(gDevice->CreateBuffer(&bufferDesc, &data, &gQuadBuffer));
}


void Application::DepthStencil()
{
	// Texutre2D
	D3D11_TEXTURE2D_DESC depthTextdesc;
	ZeroMemory(&depthTextdesc, sizeof(depthTextdesc));
	depthTextdesc.Width = width;
	depthTextdesc.Height = height;
	depthTextdesc.MipLevels = 1;
	depthTextdesc.ArraySize = 1;
	depthTextdesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTextdesc.SampleDesc.Count = 1;
	depthTextdesc.SampleDesc.Quality = 0;
	depthTextdesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTextdesc.MiscFlags = 0;
	depthTextdesc.CPUAccessFlags = 0;

	CHECK_HR(gDevice->CreateTexture2D(&depthTextdesc, NULL, &depthStencilBuffer));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	CHECK_HR(gDevice->CreateDepthStencilView(depthStencilBuffer, &depthStencilDesc, &depthStencilView));

	// Render Target View
	//D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	//ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	//rtvDesc.Format = depthStencilDesc.Format;
	//rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//rtvDesc.Texture2D.MipSlice = 0;

	//CHECK_HR(gDevice->CreateRenderTargetView(shadowBuffer, &rtvDesc, &shadowRTV));

	// Shader Reasource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	CHECK_HR(gDevice->CreateShaderResourceView(depthStencilBuffer, &srvDesc, &camDepth_SRV));
}


HRESULT Application::UpdateCamBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));
	hr = gDeviceContext->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//Constantbuffer* dataPtr = (Constantbuffer*)mappedResource.pData;
	memcpy(mappedResource.pData, &camera->getPosition(), sizeof(XMFLOAT4));
	gDeviceContext->Unmap(camBuffer, 0);
	gDeviceContext->GSSetConstantBuffers(0, 1, &camBuffer);
	gDeviceContext->PSSetConstantBuffers(1, 1, &camBuffer);
	return hr;
}

void Application::CreateCameraBuffer()
{
	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(Constantbuffer);
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.MiscFlags = 0;
	cbufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &camera->getPosition();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&cbufferDesc, &data, &camBuffer);
	gDeviceContext->GSSetConstantBuffers(0, 1, &camBuffer);
	gDeviceContext->PSSetConstantBuffers(1, 1, &camBuffer);
}

void Application::SetViewport() // directx, run once.
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)this->width;
	vp.Height = (float)this->height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}