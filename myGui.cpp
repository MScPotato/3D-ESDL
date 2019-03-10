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

void myGui::Update(ModelHandler* ObjHandler, XMFLOAT3 camPos, ID3D11ShaderResourceView* gDefTex[])
{
	// --------------------------------------------------------
	// ImGui
	// --------------------------------------------------------

	static bool imguiInit_Menu = false;
	// Start ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// --------------------------------------------------------
	// Menu
	// --------------------------------------------------------

	if (!imguiInit_Menu)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 440));
		ImGui::SetNextWindowPos(ImVec2(5, 1));
		imguiInit_Menu = true;
	}

	ImGui::Begin("ESDL Engine");
	if (ImGui::CollapsingHeader("General Info"))
	{
		// Display FPS/Frames
		ImGui::Text("FPS: %d", fps); ImGui::SameLine();
		ImGui::Text("(%d)", ImGui::GetFrameCount());
		// Display camera data
		std::string camX = std::to_string(camPos.x);
		std::string camY = std::to_string(camPos.y);
		std::string camZ = std::to_string(camPos.z);
		ImGui::Text("Camera: \n[x: %s/y: %s/z: %s]", camX.c_str(), camY.c_str(), camZ.c_str());
	}

	if (ImGui::CollapsingHeader("Render Data"))
	{
		// Display G buffers
		ImGui::Text("Normal"); ImGui::SameLine(0, 160); ImGui::Text("Color");
		ImGui::Image((void*)gDefTex[0], ImVec2(192, 108));
		ImGui::SameLine();
		ImGui::Image((void*)gDefTex[1], ImVec2(192, 108));
		ImGui::Text("Position"); ImGui::SameLine(0, 145); ImGui::Text("Light (Not in use)");
		ImGui::Image((void*)gDefTex[2], ImVec2(192, 108));
		ImGui::SameLine();
		ImGui::Image((void*)gDefTex[3], ImVec2(192, 108));
	}
	
	// Spawn Menu
	if (ImGui::CollapsingHeader("Spawn Menu"))
	{
		ImGui::Text("Nr of Existing Models: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(ModelsCounter).c_str());
		ImGui::Separator();

		//if (ImGui::CollapsingHeader("Scenes"))
		if (ImGui::TreeNode("Scene Loader"))
		{
			if (ImGui::Button("Add Random Cube"))
			{
				SpawnedAt = ObjHandler->addRndBox();
				ModelsCounter++;
			}
			if (ImGui::Button("Add Random Sphere"))
			{
				SpawnedAt = ObjHandler->addRndSphere();
				ModelsCounter++;
			}

			std::string cubeX = std::to_string(SpawnedAt.x);
			std::string cubeY = std::to_string(SpawnedAt.y);
			std::string cubeZ = std::to_string(SpawnedAt.z);
			std::string cubeW = std::to_string(SpawnedAt.w);
			//ImGui::SameLine();
			if (SpawnedAt.w == 0)
			{
				ImGui::Text("Spawned at: N/A");
			}
			else
				ImGui::Text("Spawned at: %.2s, %.2s, %.2s\nSize: %.1s", cubeX.c_str(), cubeY.c_str(), cubeZ.c_str(), cubeW.c_str());
			
			ImGui::Separator();
			if (ImGui::Button("Scene #1"))
			{
				ModelsCounter = 0;
				ObjHandler->ClearObjects();
				ModelsCounter += ObjHandler->LoadScene1();
				SpawnedAt = { 0,0,0,0 };
				//lightHandler->ClearLights();
				//lightHandler->addLight(XMFLOAT3(0, 2, 0), XMFLOAT4(1, 1, 1, 10));
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
		ImGui::Text("Spawn Position");
		ImGui::SliderFloat("Size", &size, 1, 5, "%.3f");
		ImGui::SliderFloat("Pos X", &fx, -35, 35, "%.3f");
		ImGui::SliderFloat("Pos Y", &fy, -35, 35, "%.3f");
		ImGui::SliderFloat("Pos Z", &fz, -35, 35, "%.3f");
		ImGui::SliderFloat("Angle X", &ax, -175, 175, "%.3f");
		ImGui::SliderFloat("Angle Y", &ay, -175, 175, "%.3f");
		ImGui::SliderFloat("Angle Z", &az, -175, 175, "%.3f");
		
		if (ImGui::Button("Reset All"))
		{
			size = 1.f, fx = 0.f, fy = 4.9f, fz = 0.f, ax = 0.f, ay = 0.f, az = 0.f;
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
			ModelsCounter += 27;
		}
		ImGui::SameLine();
		if (ImGui::Button("Add 9x9 Spheres"))
		{
			ObjHandler->add9x9Spheres(fx, fy, fz, size, ax, ay, az);
			if (IncrOnSpawn) fx += 1.5f * 3;
			if (DecrOnSpawn) fx -= 1.5f * 3;
			ModelsCounter += 81;
		}
		if (ImGui::Button("Add train (CAUTION)"))
		{
			ObjHandler->addTrain(fx, fy, fz, size, ax, ay, az);
			if (IncrOnSpawn) fx += 1.5f * 3;
			if (DecrOnSpawn) fx -= 1.5f * 3;
			ModelsCounter += 1;
		}
		if (ImGui::Button("Clear Scene"))
		{
			ObjHandler->ClearObjects();
			size = 1, fx = 0, fy = 4.9, fz = 0, ax = 0, ay = 0, az = 0;
			SpawnedAt = { 0,0,0,0 };
			ModelsCounter = 0;
		}
	}

	ImGui::End();
}
