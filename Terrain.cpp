#include "pch.h"
#include "Terrain.h"

Terrain::Terrain()
{
	mTerrain.terrainWidth = 500;
	mTerrain.terrainHeight = 500;
	mTerrain.heightScale = 1;
	mTerrain.quadSize = 1;
	mTerrain.filename = L"Textures/terrainEX.raw";
	heightData.resize(mTerrain.terrainWidth * mTerrain.terrainHeight);

	nrOfRows = (mTerrain.terrainHeight - 1);
	nrOfCols = (mTerrain.terrainWidth - 1);

	nrOfQuadVertices = nrOfRows * nrOfCols;
	nrOfQuadFaces = (nrOfRows - 1) * (nrOfCols - 1);

	XMStoreFloat4x4(&this->modelSpace, XMMatrixIdentity());
}

void Terrain::initTerrain(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext)
{
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
}

Terrain::~Terrain()
{
}

bool Terrain::loadHeightmap()
{
	// Creation of vector with preset size for later
	std::vector<unsigned char> input(mTerrain.terrainWidth * mTerrain.terrainHeight);

	std::ifstream heightFile;
	heightFile.open(mTerrain.filename, std::ios_base::binary);
	if (heightFile)
	{
		// Read file, save data to our preset vector.
		heightFile.read((char*)&input[0], (std::streamsize)input.size());
		heightFile.close();
	}
	else
	{
		MessageBox(NULL, L"TERRAIN ERROR: Could not open file for TerrainMap", L"ERROR", MB_OK);
		return false;
	}
	
	for (UINT i = 0; i < mTerrain.terrainWidth * mTerrain.terrainHeight; i++)
	{
		// Save input heightmap data scaled to our settings
		heightData[i] = (input[i]/255.f)*mTerrain.heightScale;
	}
	return true;
}

void Terrain::BuildQuadPatchVB()
{
	std::vector<TerrainVertex> vertices(nrOfRows * nrOfCols);
	
	float Width = (mTerrain.terrainWidth - 1)*mTerrain.quadSize/2;
	float Depth = (mTerrain.terrainHeight - 1)*mTerrain.quadSize/2;

	float quadWidth = (mTerrain.terrainWidth - 1)*mTerrain.quadSize / (nrOfCols - 1);
	float quadDepth = (mTerrain.terrainWidth - 1)*mTerrain.quadSize / (nrOfRows - 1);

	float du = 1.f / (nrOfCols - 1);
	float dv = 1.f / (nrOfRows - 1);

	for (UINT y = 0; y < nrOfCols; y++)
	{
		float posZ = Width - y * quadWidth;
		for (UINT x = 0; x < nrOfRows; x++)
		{
			float posX = -Depth + x * quadDepth;
			float posY = heightData[y*nrOfRows + x];
			vertices[y*nrOfRows + x].pos = XMFLOAT3(posX, 0.0f, posZ);
			
			// stretch texture
			vertices[y*nrOfRows + x].uv.x = x * du;
			vertices[y*nrOfRows + x].uv.y = y * dv;
			
			vertices[y*nrOfRows + x].normal = XMFLOAT3(0, 1, 0);
		}
	}

	calcNormal(vertices);

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(TerrainVertex) * vertices.size();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vertices[0];
	gDevice->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

}

void Terrain::calcNormal(std::vector<TerrainVertex> &vert)
{
	XMVECTOR vec1;
	XMVECTOR vec2;
	XMVECTOR vec3;
	XMVECTOR vec4;
	XMVECTOR p1;
	XMVECTOR p2;
	XMVECTOR p3;
	XMVECTOR p4;
	XMVECTOR normal1;
	XMVECTOR normal2;
	
	for (int i = 0; i < nrOfCols-1; i++)
	{
		for (int j = 0; j < nrOfRows - 1; j++)
		{
			p1 = XMLoadFloat3(&vert[i * nrOfRows + j].pos);
			p2 = XMLoadFloat3(&vert[i * nrOfRows + j + 1].pos);
			p3 = XMLoadFloat3(&vert[(i + 1)*nrOfRows + j].pos);
			p4 = XMLoadFloat3(&vert[(i + 1)*nrOfRows + j + 1].pos);

			vec1 = p2 - p1;
			vec2 = p3 - p1;
			vec3 = p2 - p4;
			vec4 = p3 - p4;

			normal1 = XMVector3Cross(vec1, vec2);
			normal2 = XMVector3Cross(vec3, vec4);
			XMStoreFloat3(&vert[i * nrOfRows + j].normal, normal1);
			XMStoreFloat3(&vert[i * nrOfRows + j + 1].normal, normal1);
			XMStoreFloat3(&vert[(i + 1)*nrOfRows + j].normal, normal1);
			XMStoreFloat3(&vert[(i + 1)*nrOfRows + j + 1].normal, normal2);
		}
	}
}

void Terrain::BuildQuadPatchIB()
{
	std::vector<UINT> indices(nrOfQuadFaces * 4);

	int k = 0;
	for (UINT i = 0; i < nrOfCols - 1; i++) {
		for (UINT j = 0; j < nrOfRows - 1; j++)
		{
			//top
			indices[k++] = i * nrOfRows + j;
			indices[k++] = i * nrOfRows + j + 1;

			// bot
			indices[k++] = (i + 1)*nrOfRows + j;
			indices[k++] = (i + 1)*nrOfRows + j + 1;
		}
	}

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(int)*indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &indices[0];
	gDevice->CreateBuffer(&bufferDesc, &data, &indexBuffer);

}

void Terrain::draw(Constantbuffer &constBuffData, ID3D11Buffer* gConstantBuffer)
{
	UINT stride = sizeof(TerrainVertex);
	UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CHECK_HR(gDeviceContext->Map(gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	constBuffData.world = modelSpace;
	memcpy(mappedResource.pData, &constBuffData, sizeof(Constantbuffer));
	gDeviceContext->Unmap(gConstantBuffer, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBuffer);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP/*D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST*/);
	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT/*DXGI_FORMAT_R16_UINT*/, 0);

	gDeviceContext->DrawIndexed(nrOfQuadFaces * 4, 0, 0);
}