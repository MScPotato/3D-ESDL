#pragma once
#include "Light.h"

class LightHandler
{
private:
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
private:
	int lastID;
	int lightPos;
	std::vector<Light> lights;

	//----------------------------------------------------
	std::vector<XMFLOAT4> lightPosition;
	std::vector<XMFLOAT4> lightRGBA;
	//int nrOfLights;
	//----------------------------------------------------

	ID3D11Buffer* lightPosBuffer;
	ID3D11Buffer* lightRGBABuffer;

	//int find(int id);
	void setLight(XMFLOAT4 position, XMFLOAT4 rgba);
public:
	LightHandler();
	~LightHandler();
	void ClearLights();

	void Init(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext);
	bool addLight(XMFLOAT3 pos = { 0, 0, 0 }, XMFLOAT4 rgba = { 0,0,0,1 });//void?
	
	void ModifyLight(int id, std::string type, XMFLOAT3 newValue);

	Light getLight(int pos);
	
	void draw();

	void setLightArr();

	void CreateLightPosBuffer();
	void updateLightPosBuff();

	void CreateLightRGBABuffer();
	void updateLightRGBABuff();
};

