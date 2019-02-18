#include "pch.h"
#include "Light.h"

Light::Light(XMFLOAT4 pos, XMFLOAT4 rgba)
{
	light.position = pos;
	light.rgba = rgba;

	//this->lightBuffer = nullptr;
}

Light::~Light()
{

}

//void Light::CreateLightBuffer()
//{
//	D3D11_BUFFER_DESC cbufferDesc;
//	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
//	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	cbufferDesc.ByteWidth = sizeof(LightData);
//	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	cbufferDesc.MiscFlags = 0;
//	cbufferDesc.StructureByteStride = 0;
//
//	D3D11_SUBRESOURCE_DATA data;
//	data.pSysMem = &light;
//	data.SysMemPitch = 0;
//	data.SysMemSlicePitch = 0;
//
//	gDevice->CreateBuffer(&cbufferDesc, &data, &lightBuffer);
//	gDeviceContext->PSSetConstantBuffers(2, 1, &lightBuffer);
//}

//void Light::updateLightBuff()
//{
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//
//	gDeviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//
//	memcpy(mappedResource.pData, &light, sizeof(LightData));
//
//	gDeviceContext->Unmap(lightBuffer, 0);
//	gDeviceContext->PSSetConstantBuffers(2, 1, &lightBuffer);
//}

void Light::setLight(XMFLOAT4 pos, XMFLOAT4 rgba)
{
	light.position = pos;
	light.rgba = rgba;
}