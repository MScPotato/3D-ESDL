#include "pch.h"
#include "myGui.h"


myGui::myGui()
{
}


myGui::~myGui()
{
}

void myGui::CalcFPS(double dt)
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

void myGui::Update(ModelHandler* ObjHandler, XMFLOAT3 camPos)
{
	// --------------------------------------------------------
	// ImGui
	// --------------------------------------------------------

	static bool imguiInit_Info = false;
	static bool imguiInit_Spawn = false;
	// Start ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Create ImGui Test window
	if (!imguiInit_Info)
	{
		ImGui::SetNextWindowSize(ImVec2(280, 80));
		ImGui::SetNextWindowPos(ImVec2(5, 0));
		imguiInit_Info = true;
	}
	ImGui::Begin("Info");
	ImGui::Text("FPS: %d", fps); ImGui::SameLine();
	ImGui::Text("(%d)", ImGui::GetFrameCount());
	std::string camX = std::to_string(camPos.x);
	std::string camY = std::to_string(camPos.y);
	std::string camZ = std::to_string(camPos.z);
	ImGui::Text("Camera: \n[x: %s/y: %s/z: %s]", camX.c_str(), camY.c_str(), camZ.c_str());
	ImGui::End();
	//10, 20, -10
	// --------------------------------------------------------

	if (!imguiInit_Spawn)
	{
		ImGui::SetNextWindowSize(ImVec2(280, 380));
		ImGui::SetNextWindowPos(ImVec2(5, 82));
		imguiInit_Spawn = true;
	}
	ImGui::Begin("Spawn Menu");
	ImGui::Text("Objects: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(ModelsCounter).c_str());
	ImGui::Separator();

	if (ImGui::Button("Add Random Cube"))
	{
		ObjHandler->addRndBox();
		ModelsCounter++;
	}
	ImGui::SameLine();

	if (ImGui::Button("Load Scene"))
	{
		ModelsCounter = 0;
		ObjHandler->ClearObjects();
		ModelsCounter += ObjHandler->LoadScene1();
		//lightHandler->ClearLights();
		//lightHandler->addLight(XMFLOAT3(0, 2, 0), XMFLOAT4(1, 1, 1, 10));
	}
	ImGui::Separator();

	ImGui::Text("Cube Position");
	ImGui::SliderFloat("Size", &size, 1, 5);
	ImGui::SliderFloat("Pos X", &fx, -35, 35);
	ImGui::SliderFloat("Pos Y", &fy, -35, 35);
	ImGui::SliderFloat("Pos Z", &fz, -35, 35);
	ImGui::SliderFloat("Angle X", &ax, -180, 180);
	ImGui::SliderFloat("Angle Y", &ay, -180, 180);
	ImGui::SliderFloat("Angle Z", &az, -180, 180);
	if (ImGui::Button("Reset"))
	{
		size = 1, fx = 0, fy = 0, fz = 0, ax = 0, ay = 0, az = 0;
	}
	ImGui::Separator();
	//ImGui::SameLine();
	ImGui::Checkbox("+", &IncrOnSpawn); ImGui::SameLine();
	if (IncrOnSpawn && DecrOnSpawn) { DecrOnSpawn = false; }
	ImGui::Checkbox("-", &DecrOnSpawn);
	if (IncrOnSpawn && DecrOnSpawn) { IncrOnSpawn = false; }

	if (ImGui::Button("Add Cube"))
	{
		ObjHandler->addModel(fx, fy, fz, size, ax, ay, az);
		if (IncrOnSpawn) fx += 1.5f;
		if (DecrOnSpawn) fx -= 1.5f;
		ModelsCounter++;
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Sphere"))
	{
		ObjHandler->addSphere(fx, fy, fz, size, ax, ay, az);
		if (IncrOnSpawn) fx += 1.5f;
		if (DecrOnSpawn) fx -= 1.5f;
		ModelsCounter++;
	}
	if (ImGui::Button("Add 9x9 Cubes"))
	{
		ObjHandler->add9x9Cubes(fx, fy, fz, size, ax, ay, az);
		if (IncrOnSpawn) fx += 1.5f * 3;
		if (DecrOnSpawn) fx -= 1.5f * 3;
		ModelsCounter += 81;
	}
	ImGui::SameLine();
	if (ImGui::Button("Add 9x9 Spheres"))
	{
		ObjHandler->add9x9Spheres(fx, fy, fz, size, ax, ay, az);
		if (IncrOnSpawn) fx += 1.5f * 3;
		if (DecrOnSpawn) fx -= 1.5f * 3;
		ModelsCounter += 81;
	}
	if (ImGui::Button("Clear Scene"))
	{
		ObjHandler->ClearObjects();
		size = 1, fx = 0, fy = 0, fz = 0, ax = 0, ay = 0, az = 0;
		ModelsCounter = 0;
	}

	ImGui::End();
}
