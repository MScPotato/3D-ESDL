#pragma once
#include "ModelHandler.h"
class myGui
{

private:
	// ImGui
	int ModelsCounter = 1;
	bool IncrOnSpawn = false;
	bool DecrOnSpawn = false;
	float size = 1;
	float fx = 0.f;
	float fy = 0.f;
	float fz = 0.f;
	float ax = 0.f;
	float ay = 0.f;
	float az = 0.f;
	XMFLOAT4 SpawnedAt = { 0,0,0,0 };
	ImGuiIO& io = ImGui::GetIO();

	//fps
	int fpsCounter = 0;
	float fpsTimer = 0.f;
	int fps = 0.f;

	
public:
	myGui();
	~myGui();

	void CalcFPS(double dt);
	void Update(ModelHandler* ObjHandler, XMFLOAT3 camPos, ID3D11ShaderResourceView* gDefTex[]);
};

