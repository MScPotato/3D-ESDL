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
	gIndexBuffer = nullptr;
	gVertexLayout = nullptr;
	gVertexShader = nullptr;
	gGeometryShader = nullptr;
	gPixelShader = nullptr;
	gSampleStateWrap = nullptr;
	gTextureSRV = nullptr;

	gConstantBuffer = nullptr;
	gDefVS_Layout = nullptr;
	gDefVS_Color = nullptr;
	gDefPS_Color = nullptr;

	depthStencilView = nullptr;
	depthStencilBuffer = nullptr;
	nrOfIndicies = 0;

	camera = new Camera(width, height, wndHandle);
	XMStoreFloat4x4(&constBuffData.world, XMMatrixIdentity());
	constBuffData.view = camera->getView();
	XMStoreFloat4x4(&constBuffData.projection, XMMatrixPerspectiveFovLH(XM_PI*0.45, (this->width / this->height), 0.1, 50));
	 
	
	ObjHandler = new ModelHandler();
	lightHandler = new LightHandler();
}

Application::~Application() // REMEMBER TO RELEASE ALL COM OBJs
{
	//gVertexBuffer->Release();

	//gVertexLayout->Release();
	//gVertexShader->Release();
	//gPixelShader->Release();

	//gBackbufferRTV->Release();
	//gSwapChain->Release();
	//gDevice->Release();
	//gDeviceContext->Release();
}

HRESULT Application::initiateApplication()
{
	hr = CreateDirect3DContext(hwnd); //2. Skapa och koppla SwapChain, Device och Device Context
	if (FAILED(hr))
		MessageBox(NULL, L"CRITICAL ERROR: Could not create 3DContext", L"ERROR", MB_OK);
	
	CreateConstantbufferDescription();
	CreateCameraBuffer();
	SetViewport(); //3. Sätt viewport
	SetSampleState();

	//hr = CreateShaders(); //4. Skapa vertex- och pixel-shaders
	hr = CreateDefShaders_Color();

	if (FAILED(hr))
		MessageBox(NULL, L"CRITICAL ERROR: Could not create Shaders", L"ERROR", MB_OK);

	initModels();
	if (FAILED(hr))
	{
		MessageBox(NULL, L"CRITICAL ERROR: Could not create Models", L"ERROR", MB_OK);
	}

	//app.CreateTriangleData(); 
	//5. Definiera triangelvertiser, 6. Skapa vertex buffer, 7. Skapa input layout
	return hr;
}

bool Application::initModels()
{
	ObjHandler->Init(gDevice, gDeviceContext, gTextureSRV, gConstantBuffer);
	lightHandler->Init(gDevice, gDeviceContext);

	//ObjHandler->addModel(0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, L"candysun.obj");
	//ObjHandler->addModel(-1.5, -3.0, -5.5, 1.0, 0.0, 0.0, 0.0, L"candyblue.obj");

	lightHandler->addLight(XMFLOAT3(0, 3, 0), XMFLOAT4(1, 0, 0, 3));
	//lightHandler->addLight(XMFLOAT3(0, -3, 0), XMFLOAT4(0, 1, 0, 3));
	//lightHandler->addLight(XMFLOAT3(0, 0, -3), XMFLOAT4(0, 0, 1, 3));
	//lightHandler->addLight(XMFLOAT3(0, 0, 3), XMFLOAT4(1, 0, 1, 3));
	//lightHandler->addLight(XMFLOAT3(3, 0, 0), XMFLOAT4(1, 1, 0, 3));
	//lightHandler->addLight(XMFLOAT3(-3, 0, 0), XMFLOAT4(0, 1, 1, 3));

	lightHandler->CreateLightPosBuffer();
	lightHandler->CreateLightRGBABuffer();

	//test för olika texture
	ObjHandler->addModel(0, 0, 0, 1, 0, 0, 0, L"candysun.obj");

	//3x trains
	//ObjHandler->addModel(-1, 0, 0, 0.5, L"steyerdorf.obj");
	//ObjHandler->addModel(0, 0, 0, 1, 0, 0, 0, L"steyerdorf.obj");
	//ObjHandler->addModel(constBuffData.world, 5, 0, -1, 1, 0, -90, 0, L"steyerdorf.obj");

	return true;
}

void Application::Update()
{
	dt = timer.GetMillisecondsElapse();
	timer.Restart();

	ObjHandler->update();
	camera->UpdateCamera(dt);
	CalcFPS(dt);
}

void Application::CalcFPS(double dt)
{
	fpsCounter++;
	fpsTimer += dt;

	if (fpsTimer >= 1000.0)
	{
		fps = fpsCounter;
		fpsCounter = 0;
		fpsTimer = 0;
	}
}


HRESULT Application::CreateDefShaders_Color()
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	/** Vertex Shader **/

	hr = D3DCompileFromFile(
		L"DefV_Color.hlsl",		// filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"VS_main",			// entry point
		"vs_5_0",			// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,					// IGNORE...DEPRECATED.
		&pVS,				// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (pVS)
			pVS->Release();
		return hr;
	}

	gDevice->CreateVertexShader(
		pVS->GetBufferPointer(),
		pVS->GetBufferSize(),
		nullptr,
		&gDefVS_Color
	);

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

	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gDefVS_Layout);

	// we do not need anymore this COM object, so we release it.
	pVS->Release();



	/** Geometry Shader **/
	//ID3DBlob* pGS = nullptr;
	//if (errorBlob) errorBlob->Release();
	//errorBlob = nullptr;

	//hr = D3DCompileFromFile(
	//	L"Shaders/Geometry.hlsl",		// filename
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
	hr = D3DCompileFromFile(
		L"DefP_Color.hlsl",		// filename
		nullptr,				// optional macros
		nullptr,				// optional include files
		"PS_main",				// entry point
		"ps_5_0",				// shader model (target)
		D3DCOMPILE_DEBUG,		// shader compile options
		0,						// effect compile options
		&pPS,					// double pointer to ID3DBlob		
		&errorBlob				// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (pPS)
			pPS->Release();
		return hr;
	}

	hr = gDevice->CreatePixelShader(
		pPS->GetBufferPointer(),
		pPS->GetBufferSize(),
		nullptr, &gDefPS_Color);
	pPS->Release();

	DepthStencil();
	SetupImGui();

	return hr;
}

HRESULT Application::CreateShaders()
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	/** Vertex Shader **/

	hr = D3DCompileFromFile(
		L"Vertex.hlsl",		// filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"VS_main",			// entry point
		"vs_5_0",			// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,					// IGNORE...DEPRECATED.
		&pVS,				// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (pVS)
			pVS->Release();
		return hr;
	}

	gDevice->CreateVertexShader(
		pVS->GetBufferPointer(),
		pVS->GetBufferSize(),
		nullptr,
		&gVertexShader
	);

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

	hr = gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);

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
	hr = D3DCompileFromFile(
		L"Pixel.hlsl",		// filename
		nullptr,				// optional macros
		nullptr,				// optional include files
		"PS_main",				// entry point
		"ps_5_0",				// shader model (target)
		D3DCOMPILE_DEBUG,		// shader compile options
		0,						// effect compile options
		&pPS,					// double pointer to ID3DBlob		
		&errorBlob				// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (pPS)
			pPS->Release();
		return hr;
	}

	hr = gDevice->CreatePixelShader(
		pPS->GetBufferPointer(),
		pPS->GetBufferSize(),
		nullptr, &gPixelShader);
	pPS->Release();

	DepthStencil();
	SetupImGui();

	return hr;
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
}

HRESULT Application::UpdateConstBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = gDeviceContext->Map(gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	constBuffData.view = camera->getView();
	//Constantbuffer* dataPtr = (Constantbuffer*)mappedResource.pData;
	memcpy(mappedResource.pData, &constBuffData, sizeof(Constantbuffer));
	gDeviceContext->Unmap(gConstantBuffer, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBuffer);
	return hr;
}

HRESULT Application::UpdateCamBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = gDeviceContext->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//Constantbuffer* dataPtr = (Constantbuffer*)mappedResource.pData;
	memcpy(mappedResource.pData, &camera->getPosition(), sizeof(XMFLOAT4));
	gDeviceContext->Unmap(camBuffer, 0);
	gDeviceContext->GSSetConstantBuffers(0, 1, &camBuffer);
	return hr;
}

void Application::Render()
{
	// clear the back buffer to a deep blue
	float clearColor[] = { 0.2, 0.3, 0.3, 1 };
	//float clearColor[] = { 0.8, 0.8, 0.8, 1 };
	// use DeviceContext to talk to the API
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//D3D11_MAPPED_SUBRESOURCE dataptr;
	//gDeviceContext->Map(gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataptr);
	//memcpy(dataptr.pData, &constBuffData, sizeof(Constantbuffer));
	//gDeviceContext->Unmap(gConstantBuffer, 0);

	// specifying NULL or nullptr we are disabling that stage
	// in the pipeline
	gDeviceContext->VSSetShader(gDefVS_Color, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(gDefPS_Color, nullptr, 0);

	UpdateConstBuffer();
	UpdateCamBuffer();

	//gDeviceContext->PSSetSamplers(0, 1, &gSampleStateClamp); // once
	//UINT32 aSize = sizeof(Constantbuffer);
	// specify which vertex buffer to use next.
	//gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	//setvertexbuffer
	//gDeviceContext->IASetVertexBuffers(1, 1, &gConstantBuffer, &aSize, &offset);
	//gDeviceContext->IASetIndexBuffer(gIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// specify the topology to use when drawing
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// specify the IA Layout (how is data passed)
	gDeviceContext->IASetInputLayout(gDefVS_Layout);

	// issue a draw call of 3 vertices (similar to OpenGL)
	drawAll();
	ImGuiRender();

	// Present the backbuffer
	gSwapChain->Present(0, 0); //9. Växla front- och back-buffer
}

void Application::ImGuiRender()
{
	// --------------------------------------------------------
	// ImGui
	// --------------------------------------------------------

	// Start ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Create ImGui Test window
	if (!imguiInit)
	{
		ImGui::SetNextWindowSize(ImVec2(230, 350));
		imguiInit = true;
	}
	ImGui::Begin("Debug Cubes");
	ImGui::Text("FPS: %d", fps);
	ImGui::Text("Frames: %d", ImGui::GetFrameCount());
	
	ImGui::Text("Cubes: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(boxCounter).c_str());
	ImGui::Separator();

	if (ImGui::Button("Add Random Cube"))
	{
		ObjHandler->addRndBox();
		boxCounter++;
	}
	ImGui::SameLine();
	if (ImGui::Button("Add 9x9 Cubes"))
	{
		ObjHandler->add9x9Model();
		boxCounter += 81;
	}
	ImGui::Separator();

	ImGui::Text("Cube Position");
	ImGui::SliderFloat("Size", &size, 1, 5);
	ImGui::SliderFloat("Pos X", &fx, -16, 16);
	ImGui::SliderFloat("Pos Y", &fy, -16, 16);
	ImGui::SliderFloat("Pos Z", &fz, -16, 16);
	ImGui::SliderFloat("Angle X", &ax, -180, 180);
	ImGui::SliderFloat("Angle Y", &ay, -180, 180);
	ImGui::SliderFloat("Angle Z", &az, -180, 180);

	if (ImGui::Button("Add Cube"))
	{
		ObjHandler->addModel(fx, fy, fz, size, ax, ay, az);
		boxCounter++;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		size = 1, fx = 0, fy = 0, fz = 0, ax = 0, ay = 0, az = 0;
	}
	ImGui::Separator();

	if (ImGui::Button("Remove All Objs"))
	{
		ObjHandler->ClearObjects();
		boxCounter = 0;
	}

	ImGui::End();

	gDeviceContext->GSSetShader(nullptr, nullptr, 0);

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// --------------------------------------------------------
}

void Application::drawAll()
{
	ObjHandler->draw(constBuffData);
	lightHandler->draw();
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

															// create a device, device context and swap chain using the information in the scd struct
	hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
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
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, NULL);
	}
	return hr;
}

void Application::DepthStencil()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.CPUAccessFlags = 0;

	gDevice->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	gDevice->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, depthStencilView);
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

