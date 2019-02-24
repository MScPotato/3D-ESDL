#pragma once
#include "Check.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
using namespace DirectX;

#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Globals.h"
#include "Math.h"
#include "WICTextureLoader.h"
#include "Window.h"
#include "ModelLoader.h"
#include "Timer.h"

#include "ImGui/imconfig.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imstb_rectpack.h"
#include "ImGui/imstb_textedit.h"
#include "ImGui/imstb_truetype.h"
//#include "ImGuiManager.h"
#include "myGui.h"
