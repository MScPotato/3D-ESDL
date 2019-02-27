#include "pch.h"
#include "ModelHandler.h"


ModelHandler::ModelHandler()
{
	loader = new ModelLoader();
}


ModelHandler::~ModelHandler()
{	
}

void ModelHandler::ClearObjects()
{
	models.clear();
}

void ModelHandler::Init(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* gTextureSRV, /*Constantbuffer &constBuffData,*/ ID3D11Buffer* constantBuffer)
{
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
	this->gTextureSRV = gTextureSRV;
	//this->constBuffData = constBuffData;
	this->constantBuffer = constantBuffer;
	loader->Init(gDevice, gDeviceContext, gTextureSRV);
}

//bool ModelHandler::addModel(std::wstring objName, float x, float y, float z)
bool ModelHandler::addModel(float x, float y, float z, float size, float rx, float ry, float rz, std::wstring objName)
{
	bool added = false;
	
	Model temp(gDevice, gDeviceContext, gTextureSRV);
	temp.setModelScaling(size);
	//temp.setRotation(45, true, true, false);
	temp.setRotation(rx, ry, rz);
	temp.setModelSpace(x, y, z);
	loader->LoadModel(objName, temp);
	hr = temp.CreateModelData();
	if (SUCCEEDED(hr))
	{
		DirectX::XMFLOAT4X4 test = temp.getModelSpace();
		models.push_back(temp);
		added = true;
	}
	
	return added;
}

bool ModelHandler::addCube(float x, float y, float z, float size, float rx, float ry, float rz)
{
	return addModel(x, y, z, size, rx, ry, rz, L"Cube.obj");
}

bool ModelHandler::addSphere(float x, float y, float z, float size, float rx, float ry, float rz)
{
	return addModel(x, y, z, size, rx, ry, rz, L"candysun.obj");
}

bool ModelHandler::addTrain(float x, float y, float z, float size, float rx, float ry, float rz)
{
	return addModel(x, y, z, size, rx, ry, rz, L"steyerdorf.obj");
}

void ModelHandler::add9x9Model(float x, float y, float z, float size, float rx, float ry, float rz, std::wstring objName)
{
	addModel(0 + x, 0 + y, 0 + z, size, rx, ry, rz, objName);
	addModel(1.5 + x, 0 + y, 0 + z, size, rx, ry, rz, objName);
	addModel(1.5 + x, 1.5 + y, 0 + z, size, rx, ry, rz, objName);
	addModel(1.5 + x, -1.5 + y, 0 + z, size, rx, ry, rz, objName);
	addModel(-1.5 + x, 0 + y, 0 + z, size, rx, ry, rz, objName);
	addModel(-1.5 + x, 1.5 + y, 0 + z, size, rx, ry, rz, objName);
	addModel(-1.5 + x, -1.5 + y, 0 + z, size, rx, ry, rz, objName);
	addModel(0 + x, 1.5 + y, 0 + z, size, rx, ry, rz, objName);
	addModel(0 + x, -1.5 + y, 0 + z, size, rx, ry, rz, objName);
	addModel(0 + x, 0 + y, 1.5 + z, size, rx, ry, rz, objName);
	addModel(0 + x, 0 + y, -1.5 + z, size, rx, ry, rz, objName);
	addModel(0 + x, 1.5 + y, 1.5 + z, size, rx, ry, rz, objName);
	addModel(0 + x, -1.5 + y, 1.5 + z, size, rx, ry, rz, objName);
	addModel(1.5 + x, 0 + y, 1.5 + z, size, rx, ry, rz, objName);
	addModel(1.5 + x, 1.5 + y, 1.5 + z, size, rx, ry, rz, objName);
	addModel(1.5 + x, -1.5 + y, 1.5 + z, size, rx, ry, rz, objName);
	addModel(-1.5 + x, 0 + y, 1.5 + z, size, rx, ry, rz, objName);
	addModel(-1.5 + x, 1.5 + y, 1.5 + z, size, rx, ry, rz, objName);
	addModel(-1.5 + x, -1.5 + y, 1.5 + z, size, rx, ry, rz, objName);
	addModel(0 + x, 1.5 + y, -1.5 + z, size, rx, ry, rz, objName);
	addModel(0 + x, -1.5 + y, -1.5 + z, size, rx, ry, rz, objName);
	addModel(1.5 + x, 0 + y, -1.5 + z, size, rx, ry, rz, objName);
	addModel(1.5 + x, 1.5 + y, -1.5 + z, size, rx, ry, rz, objName);
	addModel(1.5 + x, -1.5 + y, -1.5 + z, size, rx, ry, rz, objName);
	addModel(-1.5 + x, 0 + y, -1.5 + z, size, rx, ry, rz, objName);
	addModel(-1.5 + x, 1.5 + y, -1.5 + z, size, rx, ry, rz, objName);
	addModel(-1.5 + x, -1.5 + y, -1.5 + z, size, rx, ry, rz, objName);
}


void ModelHandler::add9x9Cubes(float x, float y, float z, float size, float rx, float ry, float rz)
{
	add9x9Model(x, y, z, size, rx, ry, rz, L"Cube2.obj");
	
}

void ModelHandler::add9x9Spheres(float x, float y, float z, float size, float rx, float ry, float rz)
{
	add9x9Model(x, y, z, size, rx, ry, rz, L"candysun.obj");
}

int ModelHandler::LoadScene1() // return nr of obj in the scene
{
	addSphere(0, 10, 11.34);
	addSphere(-4, -0.4+10, -1, 0.4);
	addCube(-2.44, -0.5+10, 1, 1, 27, 33, 6);
	addCube(2.44, 10, -.5, 1, -27, -33, -6);
	addCube(4, 0, -2+10, 1.45, -45, 0, 3);
	addCube(4, 0, -2+10, 1.45, -45, 0, 3);
	addCube(0, -0, 0, 10);
	addCube(10, 10, 0, 10);
	addCube(-10, 10, 0, 10);
	addCube(0, 10, -10, 10);
	addCube(0, 10, 10, 10);
	return 11;
}

XMFLOAT4 ModelHandler::addRndBox()
{
	srand(time(NULL));
	float size = Math::RandomInt(1, 3);

	float x = Math::RandomInt(-25, 25);
	float y = Math::RandomInt(-25, 25);
	float z = Math::RandomInt(0, 25);

	float rx = Math::RandomInt(0, 85);
	float ry = Math::RandomInt(0, 85);
	float rz = Math::RandomInt(0, 85);

	std::wstring model = L"";
	int RandNr = Math::RandomInt(0, 2);
	switch (RandNr)
	{
	case 0:
		model = L"Cube2.obj";
		break;
	case 1:
		model = L"CubeTris-1.obj";
		break;
	case 2:
		model = L"Cube.obj";
		break;
	}

	addModel(size, x, y, z, rx, ry, rz, model);
	return XMFLOAT4(x, y, z, size);
}

XMFLOAT4 ModelHandler::addRndSphere()
{
	srand(time(NULL));
	float size = Math::RandomInt(1, 3);

	float x = Math::RandomInt(-25, 25);
	float y = Math::RandomInt(-25, 25);
	float z = Math::RandomInt(0, 25);
	
	float rx = Math::RandomInt(0, 85);
	float ry = Math::RandomInt(0, 85);
	float rz = Math::RandomInt(0, 85);

	addModel(size, x, y, z, rx, ry, rz, L"candysun.obj");
	return XMFLOAT4(x, y, z, size);
}


void ModelHandler::update()
{

}

void ModelHandler::draw(Constantbuffer &constBuffData)
{
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (int i = 0; i < models.size(); i++)
	{
		UpdateWorldConstBuffer(i, constBuffData);
		models.at(i).updateMTL();
		models.at(i).setVertexBuffer();
		gTextureSRV = models.at(i).getTexture();
		gDeviceContext->PSSetShaderResources(0, 1, &gTextureSRV);
		gDeviceContext->Draw(models.at(i).getMesh().size(), 0);
	}
}

HRESULT ModelHandler::UpdateWorldConstBuffer(int pos, Constantbuffer &constBuffData)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = gDeviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	constBuffData.world = models.at(pos).getModelSpace();
	memcpy(mappedResource.pData, &constBuffData, sizeof(Constantbuffer));
	gDeviceContext->Unmap(constantBuffer, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	return hr;
}