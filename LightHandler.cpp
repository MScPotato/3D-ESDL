#include "pch.h"
#include "LightHandler.h"


LightHandler::LightHandler()
{
	//this->lightarr = new LightData[5];
	//nrOfLights = 5;
}


LightHandler::~LightHandler()
{
}

void LightHandler::ClearLights()
{
	lights.clear();
}

void LightHandler::Init(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext)
{
		this->gDevice = gDevice;
		this->gDeviceContext = gDeviceContext;
}

bool LightHandler::addLight(XMFLOAT3 pos, XMFLOAT4 rgba) // id, pos (xyz) , rd, green, blue, intensity
{
	bool added = false;

		Light temp(XMFLOAT4(pos.x, pos.y, pos.z, 1), rgba);
		//temp.CreateLightBuffer();
		lights.push_back(temp);
		added = true;

	return added;
}

void LightHandler::setLight(XMFLOAT4 position, XMFLOAT4 rgba)
{
	
		lights.at(lightPos).setLight(position, rgba);
}

void LightHandler::ModifyLight(int index, std::string type, XMFLOAT3 newValue)//type: intensity, position, rgb
{

	if (index > -1)
	{
		lightPos = index;
		if (type == "intensity")
		{
			lights.at(lightPos).getLightData().rgba;
			setLight(lights.at(lightPos).getLightData().position,
				XMFLOAT4(lights.at(lightPos).getLightData().rgba.x, lights.at(lightPos).getLightData().rgba.y, lights.at(lightPos).getLightData().rgba.z, newValue.x));
		}
		else if (type == "position")
		{
			setLight(XMFLOAT4(newValue.x, newValue.y, newValue.z, 1), lights.at(lightPos).getLightData().rgba);
		}
		else if (type == "rgb")
		{
			setLight(lights.at(lightPos).getLightData().position, XMFLOAT4(newValue.x, newValue.y, newValue.z, lights.at(lightPos).getLightData().rgba.w));
		}
		else
		{
			MessageBox(NULL, L"Light type not found", L"ERROR", MB_OK);
		}
	}
	else
	{
		MessageBox(NULL, L"Light id not found", L"ERROR", MB_OK);
	}
}

Light LightHandler::getLight(int pos)
{
	return lights.at(pos);
}

void LightHandler::draw()
{
	//for (int i = 0; i < lights.size(); i++)
	{
		setLightArr();
		updateLightPosBuff();
		updateLightRGBABuff();
		
		//lights.at(i).updateLightBuff();
		//gDeviceContext->Draw(0, 0);
	}
}

void LightHandler::setLightArr()
{
	lightPosition.clear();
	lightRGBA.clear();
	for (int i = 0; i < lights.size(); i++)
	{
		lightPosition.push_back(lights.at(i).getLightData().position);
		lightRGBA.push_back(lights.at(i).getLightData().rgba);
		//nrOfLights++;
	}
}

void LightHandler::CreateLightPosBuffer()
{
	setLightArr();
	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(XMFLOAT4)*lightPosition.size();
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.MiscFlags = 0;
	cbufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = nullptr;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&cbufferDesc, NULL, &lightPosBuffer);
	gDeviceContext->PSSetConstantBuffers(2, 1, &lightPosBuffer);
}

void LightHandler::updateLightPosBuff()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	gDeviceContext->Map(lightPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, lightPosition.data(), sizeof(XMFLOAT4)*lightPosition.size());

	gDeviceContext->Unmap(lightPosBuffer, 0);
	gDeviceContext->PSSetConstantBuffers(2, 1, &lightPosBuffer);
}

void LightHandler::CreateLightRGBABuffer()
{
	setLightArr();
	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(XMFLOAT4)*lightRGBA.size();
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.MiscFlags = 0;
	cbufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = nullptr;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&cbufferDesc, NULL, &lightRGBABuffer);
	gDeviceContext->PSSetConstantBuffers(3, 1, &lightRGBABuffer);
}

void LightHandler::updateLightRGBABuff()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	gDeviceContext->Map(lightRGBABuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, lightRGBA.data(), sizeof(XMFLOAT4)*lightRGBA.size());

	gDeviceContext->Unmap(lightRGBABuffer, 0);
	gDeviceContext->PSSetConstantBuffers(3, 1, &lightRGBABuffer);
}