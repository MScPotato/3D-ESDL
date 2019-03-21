#include "pch.h"
#include "ModelLoader.h"

ModelLoader::ModelLoader()
{
}

void ModelLoader::Init(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* gTextureSRV)
{
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
	this->gTextureSRV = gTextureSRV;
}

ModelLoader::~ModelLoader()
{
}

void ModelLoader::LoadModel(std::wstring filename, Model &model)
{
	//Model temp(gDevice, gDeviceContext, gTextureSRV, world);

	std::wstring filepath = L"models/" + filename;
	struct xyz { float x, y, z; } position, normal;
	struct uv { float u, v; } coords;

	std::vector<xyz> vertices;
	std::vector<uv> texCoords;
	std::vector<xyz> normals;
	std::vector<Material> mtl;

	std::ifstream targetFile;
	targetFile.open(filepath);
	std::string line, special, tmp;
	
	if (targetFile.is_open())
	{
		while (std::getline(targetFile, line))
		{
			std::stringstream inputStr(line);
			inputStr >> special;

			if (special == "mtllib")
			{
				inputStr >> tmp;
				mtl = LoadMTL(tmp);
			}
			else if (special == "v")
			{
				inputStr >> position.x;
				inputStr >> position.y;
				inputStr >> position.z;

				vertices.push_back(position);
			}
			else if (special == "vt")
			{
				inputStr >> coords.u;
				inputStr >> coords.v;
				texCoords.push_back(coords);
			}
			else if (special == "vn")
			{
				inputStr >> normal.x;
				inputStr >> normal.y;
				inputStr >> normal.z;
				normals.push_back(normal);
			}
			else if (special == "s")
			{
				continue;
			}
			else if (special == "usemtl")
			{
				inputStr >> tmp;
				CHECK_HR(useMTL(mtl, tmp, model));
			}
			else if (special == "f")
			{
				int pos;
				int startPos = 2;
				std::string v, vt, vn;
				int vNr, vtNr, vnNr;
				Vertex vData;

				//point 1, 2 and 3
				for (int i = 0; i < 3; i++)
				{

					pos = line.find('/', startPos);
					v = line.substr(startPos, pos - startPos);
					vNr = std::stoi(v);
					startPos = pos + 1;

					pos = line.find('/', startPos);
					vt = line.substr(startPos, pos - startPos);
					vtNr = std::stoi(vt);
					startPos = pos + 1;
					if (i < 2)
					{
						pos = line.find(' ', startPos);
						vn = line.substr(startPos, pos - startPos);
						vnNr = std::stoi(vn);
						startPos = pos + 1;
					}
					else if (i == 2)
					{
						vn = line.substr(startPos);
						vnNr = std::stoi(vn);
					}
					vData.x = vertices.at(vNr - 1).x;
					vData.y = vertices.at(vNr - 1).y;
					vData.z = vertices.at(vNr - 1).z;

					vData.u = texCoords.at(vtNr - 1).u;
					vData.v = texCoords.at(vtNr - 1).v;

					vData.nx = normals.at(vnNr - 1).x;
					vData.ny = normals.at(vnNr - 1).y;
					vData.nz = normals.at(vnNr - 1).z;
					model.pushMesh(vData);
				}
			}
		}
		targetFile.close();
	}
	else
	{
		std::wstring ErMsg = L"Could not open: ";
		ErMsg += filename;
		MessageBox(NULL, ErMsg.c_str(), L"ERROR", MB_OK);
	}
	//return temp;
}

std::vector<Material> ModelLoader::LoadMTL(std::string filename)
{

	struct xyz { float x, y, z; } Kd, Ka, Tf, Ks, Ns;
	bool newmtl = false;

	Material material;
	std::vector<Material> allMats;

	std::string filepath = "models/" + filename;
	std::ifstream targetFile;
	std::string line, special, tmp;

	targetFile.open(filepath);

	if (targetFile.is_open())
	{
		while (std::getline(targetFile, line))
		{
			std::stringstream inputStr(line);
			inputStr >> special;
			if (special == "newmtl")
			{
				if(newmtl)
					allMats.push_back(material);

				inputStr >> material.name;
				newmtl = true;
			}
			else if (special == "illum")
			{
				//inputStr >> material.LightData.NiNsIl.z;
			}
			else if (special == "Kd")
			{
				inputStr >> material.LightData.Kd.x;
				inputStr >> material.LightData.Kd.y;
				inputStr >> material.LightData.Kd.z;
			}
			else if (special == "Ka")
			{
				inputStr >> material.LightData.Ka.x;
				inputStr >> material.LightData.Ka.y;
				inputStr >> material.LightData.Ka.z;
			}
			else if (special == "Tf")
			{
				//inputStr >> material.LightData.Tf.x;
				//inputStr >> material.LightData.Tf.y;
				//inputStr >> material.LightData.Tf.z;
			}
			else if (special == "map_Kd")
			{
				inputStr >> material.map_Kd;
			}
			else if (special == "map_Nor")
			{
				inputStr >> material.map_Nor;
			}
			else if (special == "Ni")
			{
				//inputStr >> material.LightData.NiNsIl.x;
			}
			else if (special == "Ks")
			{
				//inputStr >> material.LightData.Ks.x;
				//inputStr >> material.LightData.Ks.y;
				//inputStr >> material.LightData.Ks.z;
			}
			else if (special == "Ns")
			{
				inputStr >> material.LightData.Ns;
			}
			else
			{
				continue;
			}
		}
		targetFile.close();
		allMats.push_back(material);
	}
	return allMats;
}

HRESULT ModelLoader::useMTL(std::vector<Material> mtlcontent, std::string name, Model &model)
{
	int pos = -1;
	for (int i = 0; i < mtlcontent.size() && pos == -1; i++)
	{
		if (mtlcontent.at(i).name == name)
		{
			pos = i;
		}
	}
	if (pos != -1)
	{
	std::wstring filename = StringToWide(mtlcontent.at(pos).map_Kd);
	std::wstring filePath = L"Textures/" + filename;
	//std::wstring filePath = L"models/greenhouse/" + filename;
	LoadTexture(filePath);
	model.setTexture(gTextureSRV);
	filename = StringToWide(mtlcontent.at(pos).map_Nor);
	filePath = L"Textures/" + filename;
	LoadNormalMap(filePath);
	model.setNormalMap(gTextureSRV);
	model.setMtlData(mtlcontent, pos);
	//mtlBufferData.Kd = mtlcontent.at(pos).LightData.Kd;
	//mtlBufferData.Ka = mtlcontent.at(pos).LightData.Ka;
	//mtlBufferData.Tf = mtlcontent.at(pos).LightData.Tf;
	//mtlBufferData.Ks = mtlcontent.at(pos).LightData.Ks;
	//mtlBufferData.NiNsIl = mtlcontent.at(pos).LightData.NiNsIl;
	//
	//mtlBufferData.Kd.w = 1;
	//mtlBufferData.Ka.w = 1;
	//mtlBufferData.Tf.w = 1;
	//mtlBufferData.Ks.w = 1;
	//mtlBufferData.NiNsIl.w = 1;

	hr = model.CreateMTLBufferDesc();
	}
	return hr;
}

//flyttas till model
void ModelLoader::LoadTexture(std::wstring filename)
{
	hr = CreateWICTextureFromFile(gDevice, (const wchar_t*)filename.c_str(), nullptr, &gTextureSRV);
	if (FAILED(hr))
	{
		hr = CreateWICTextureFromFile(gDevice, L"Textures/Default.jpg", nullptr, &gTextureSRV);
		MessageBox(NULL, L"Loading texture Failed!", L"ERROR", MB_OK);
	}
	
	gDeviceContext->PSSetShaderResources(0, 1, &gTextureSRV); // innan render
	//return hr;
}

void ModelLoader::LoadNormalMap(std::wstring filename)
{
	// overwrites current texture in gTextureSRV
	hr = CreateWICTextureFromFile(gDevice, (const wchar_t*)filename.c_str(), nullptr, &gTextureSRV);
	if (FAILED(hr))
	{
		hr = CreateWICTextureFromFile(gDevice, L"Textures/Default.jpg", nullptr, &gTextureSRV);
		MessageBox(NULL, L"Loading Normalmap texture Failed!", L"ERROR", MB_OK);
	}

	gDeviceContext->PSSetShaderResources(1, 1, &gTextureSRV); // innan render

}

std::wstring ModelLoader::StringToWide(std::string s)
{
	std::wstring ws;

	for (int i = 0; i < s.length(); i++)
	{
		ws += (wchar_t)s[i];
	}

	const wchar_t* res = ws.c_str();

	return res;
}