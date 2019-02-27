#pragma once
class Terrain
{
	struct HeightMap
	{
		int terrainWidth;
		int terrainHeight;
		float heightScale;
		float quadSize;
		XMFLOAT3* heightMap;
		std::wstring filename;
	};
private:
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	XMFLOAT4X4 modelSpace;
	ID3D11ShaderResourceView* gTextureSRV;
private:
	HeightMap mTerrain;
	std::vector<float> heightData;
	int nrOfRows;
	int nrOfCols;
	unsigned int nrOfQuadVertices;
	unsigned int nrOfQuadFaces;
public:
	Terrain();
	~Terrain();
	void initTerrain(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext);
	bool loadHeightmap();
	void LoadTexture(std::wstring filename);
	void BuildQuadPatchVB();
	void calcNormal(std::vector<TerrainVertex> &vert);
	//void BuildQuadPatchSRV();
	void BuildQuadPatchIB();
	void draw(Constantbuffer &constBuffData, ID3D11Buffer* gConstantBuffer);
};

