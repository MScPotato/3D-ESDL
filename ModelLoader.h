#pragma once
#include "Model.h"

class ModelLoader
{
private:
	HRESULT hr;
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	ID3D11ShaderResourceView* gTextureSRV;

public:
	ModelLoader();
	~ModelLoader();
	void Init(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* gTextureSRV);
	std::wstring StringToWide(std::string s);

	void LoadModel(std::wstring filename, Model &model);
	std::vector<Material> LoadMTL(std::string filename);
	void LoadTexture(std::wstring filename);
	void LoadNormalMap(std::wstring filename);
	HRESULT useMTL(std::vector<Material> mtlcontent, std::string name, Model &model);
};