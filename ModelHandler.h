#pragma once
#include "ModelLoader.h"
#include "Model.h"

class ModelHandler
{
private:
	HRESULT hr;

	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	ID3D11ShaderResourceView* gTextureSRV;
	
	//Constantbuffer constBuffData;
	ID3D11Buffer* constantBuffer;

	ModelLoader* loader;
	std::vector<Model> models;

public:
	ModelHandler();
	~ModelHandler();
	void ClearObjects();
	void Init(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* gTextureSRV, /*Constantbuffer &constBuffData,*/ ID3D11Buffer* constantBuffer);

	//bool addModel(std::wstring objName = L"Cube.obj", float x = 0.f, float y = 0.f, float z = 0.f);
	bool addModel(float x = 0.f, float y = 0.f, float z = 0.f, float size = 1, float rx = 0, float ry = 0, float rz = 0, std::wstring objName = L"Cube.obj");
	bool addSphere(float x = 0.f, float y = 0.f, float z = 0.f, float size = 1, float rx = 0, float ry = 0, float rz = 0);
	bool addCube(float x = 0.f, float y = 0.f, float z = 0.f, float size = 1, float rx = 0, float ry = 0, float rz = 0);
	void add9x9Model(float x = 0.f, float y = 0.f, float z = 0.f, float size = 1, float rx = 0, float ry = 0, float rz = 0, std::wstring objName = L"Cube2.obj");
	void add9x9Cubes(float x = 0.f, float y = 0.f, float z = 0.f, float size = 1, float rx = 0, float ry = 0, float rz = 0);
	void add9x9Spheres(float x = 0.f, float y = 0.f, float z = 0.f, float size = 1, float rx = 0, float ry = 0, float rz = 0);
	int LoadScene1();
	void addRndBox();

	int getNrOfModels()const { return models.size(); }
	Model getModelAt(int pos)const { return models.at(pos); }

	HRESULT UpdateWorldConstBuffer(int pos, Constantbuffer &constBuffData);
	
	void update();
	void draw(Constantbuffer &constBuffData);
};

