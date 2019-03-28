#pragma once

#define NROF_LIGHTS 6;

struct Vertex
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
};

struct MTLBuffer
{
	XMFLOAT4 Kd; // diff
	XMFLOAT3 Ka; // amb
	float Ns; // spec

//	//w är just nu alltid 1
//	XMFLOAT4 Kd;
//	XMFLOAT4 Ka;
//	XMFLOAT4 Tf;
//	XMFLOAT4 Ks; //Ka = ambient, Kd = diffuse, Ks = specular, 
//	XMFLOAT4 NiNsIl; //x = Ni, y = Ns, z = illum, w = 1(for now)
};

struct Material
{
	MTLBuffer LightData;
	std::string map_Kd, map_Nor, name; //map_Kd = Diffuse tex map(more or less alltid samma som map_Ka), map_Ka = ambient tex map, name = namn för just denna delen
};

struct Constantbuffer
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};

struct ConstantLightbuffer
{
	//XMFLOAT4X4 world;
	XMFLOAT4X4 lightView;
	XMFLOAT4X4 lightProjection;
};

struct LightData
{
	XMFLOAT4 position; // w = 1
	XMFLOAT4 rgba; // red, green, blue, intensity
};

struct TerrainVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};

struct Particle
{
	XMFLOAT3 pos;
};