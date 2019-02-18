#pragma once
class Light
{
private:

	LightData light;
	//ID3D11Buffer* lightBuffer;
public:
	Light(XMFLOAT4 pos = XMFLOAT4(0, 0, 0, 0), XMFLOAT4 rgba = XMFLOAT4(1, 1, 1, 1));
	~Light();

	//void CreateLightBuffer();
	//void updateLightBuff();

	void setLight(XMFLOAT4 pos, XMFLOAT4 rgba);
	LightData getLightData()const { return this->light; }
};

