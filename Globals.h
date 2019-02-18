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
	//w är just nu alltid 1
	XMFLOAT4 Kd;
	XMFLOAT4 Ka;
	XMFLOAT4 Tf;
	XMFLOAT4 Ks; //Ka = ambient, Kd = diffuse, Ks = specular, 
	XMFLOAT4 NiNsIl; //x = Ni, y = Ns, z = illum, w = 1(for now)
};

struct Material
{
	MTLBuffer LightData;
	std::string map_Kd, map_Ka, name; //map_Kd = Diffuse tex map(more or less alltid samma som map_Ka), map_Ka = ambient tex map, name = namn för just denna delen
};

struct Constantbuffer
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};

struct LightData
{
	XMFLOAT4 position; // w = 1
	XMFLOAT4 rgba; // red, green, blue, intensity
};