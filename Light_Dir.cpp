#include "pch.h"
#include "Light_Dir.h"


Light_Dir::Light_Dir(XMFLOAT3 pos, XMFLOAT3 target, float width, float height)
{
	this->width = width;
	this->height = height;

	shadowView = nullptr;
	shadowBuffer = nullptr;
	shadowRTV = nullptr;
	shadowSRV = nullptr;
	lightPosition = pos;
	lightTarget = target;

	XMStoreFloat4x4(&lightView, XMMatrixLookAtLH(
			{ lightPosition.x, lightPosition.y, lightPosition.z },
			{ lightTarget.x,lightTarget.y,lightTarget.z },
			{ 0.f, 1.f, 0.f }));

	//XMStoreFloat4x4(&bufferData.world, XMMatrixIdentity());
	lightBufferData.lightView = lightView;
	XMStoreFloat4x4(&lightBufferData.lightProjection, XMMatrixPerspectiveFovLH(XM_PI*0.45, (this->width / this->height), 0.1f, 75.0f));
	//XMStoreFloat4x4(&lightBufferData.lightProjection, XMMatrixOrthographicLH(this->width, this->height, 1.f, 75.f));
}

void Light_Dir::InitDirLight(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext)
{
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;

	DepthStencil();
	CreateLightBuffers();
	SetViewport();
}


Light_Dir::~Light_Dir()
{
	//gDevice->Release();
	//gDeviceContext->Release();

	//shadowBuffer->Release();
	//shadowView->Release();
	//shadowRTV->Release();
	//shadowSRV->Release();

	//lightPosBuffer->Release();
	//lightBuffer->Release();
}

void Light_Dir::CreateLightBuffers()
{
	// ---------------------------------------------------------------------
	// Light Position Data

	D3D11_BUFFER_DESC posBufferDesc;
	ZeroMemory(&posBufferDesc, sizeof(D3D11_BUFFER_DESC));
	posBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	posBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	posBufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	posBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	posBufferDesc.MiscFlags = 0;
	posBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA PosData;
	PosData.pSysMem = &lightPosition;
	PosData.SysMemPitch = 0;
	PosData.SysMemSlicePitch = 0;

	CHECK_HR(gDevice->CreateBuffer(&posBufferDesc, &PosData, &lightPosBuffer));
	gDeviceContext->PSSetConstantBuffers(4, 1, &lightPosBuffer);

	// ---------------------------------------------------------------------
	// Constant Buffer Data

	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(ConstantLightbuffer);
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.MiscFlags = 0;
	cbufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &lightBufferData;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	CHECK_HR(gDevice->CreateBuffer(&cbufferDesc, &constData, &lightBuffer));
	gDeviceContext->VSSetConstantBuffers(1, 1, &lightBuffer);
	gDeviceContext->PSSetConstantBuffers(5, 1, &lightBuffer);

}

void Light_Dir::UpdateLightData()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CHECK_HR(gDeviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	//Constantbuffer* dataPtr = (Constantbuffer*)mappedResource.pData;
	memcpy(mappedResource.pData, &lightBufferData, sizeof(ConstantLightbuffer));
	gDeviceContext->Unmap(lightBuffer, 0);
	gDeviceContext->VSSetConstantBuffers(1, 1, &lightBuffer);
	gDeviceContext->PSSetConstantBuffers(5, 1, &lightBuffer);
}

void Light_Dir::Update()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));
	CHECK_HR(gDeviceContext->Map(lightPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	memcpy(mappedResource.pData, &lightPosition, sizeof(XMFLOAT4));
	gDeviceContext->Unmap(lightPosBuffer, 0);
	//gDeviceContext->VSSetConstantBuffers(1, 1, &lightCamBuffer);
	gDeviceContext->PSSetConstantBuffers(4, 1, &lightPosBuffer);
}

void Light_Dir::SetViewport() // directx, run once.
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

void Light_Dir::DepthStencil()
{
	// Texutre2D
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS; // -
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = 0;
	texDesc.CPUAccessFlags = 0;

	CHECK_HR(gDevice->CreateTexture2D(&texDesc, NULL, &shadowBuffer));

	// Depth Stencil View
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	CHECK_HR(gDevice->CreateDepthStencilView(shadowBuffer, &dsvDesc, &shadowView));

	//Render Target View
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

	CHECK_HR(gDevice->CreateShaderResourceView(shadowBuffer, &srvDesc, &shadowSRV));
}
