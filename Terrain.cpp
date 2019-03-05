#include "pch.h"
#include "Terrain.h"

Terrain::Terrain()
{
	mTerrain.terrainWidth = 64;
	mTerrain.terrainHeight = 64;
	mTerrain.heightScale = 24;
	mTerrain.quadSize = 1;
	mTerrain.filename = L"Textures/Grass_Hmap_8.raw";
	heightData.resize(mTerrain.terrainWidth * mTerrain.terrainHeight);

	nrOfRows = (mTerrain.terrainHeight);
	nrOfCols = (mTerrain.terrainWidth);

	nrOfQuadVertices = nrOfRows * nrOfCols;
	nrOfQuadFaces = (nrOfRows - 1) * (nrOfCols - 1);

	XMStoreFloat4x4(&this->modelSpace, XMMatrixIdentity());
	vertices.resize(nrOfRows * nrOfCols); // 64 * 64 = 4096

	
}

void Terrain::initTerrain(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext)
{
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
	LoadTexture(L"Textures/Grass_Terrain.png");
}

Terrain::~Terrain()
{
}

bool Terrain::loadHeightmap()
{
	int offset = 1;
	// Creation of vector with preset size for later
	std::vector<unsigned char> input(mTerrain.terrainWidth * mTerrain.terrainHeight *offset);//3 = offset

	std::ifstream heightFile;
	heightFile.open(mTerrain.filename, std::ios_base::binary);
	if (heightFile)
	{
		// Read file, save data to our preset vector.
		heightFile.read((char*)&input[0], (std::streamsize)input.size()*offset); //3 = offset
		heightFile.close();
	}
	else
	{
		MessageBox(NULL, L"TERRAIN ERROR: Could not open file for TerrainMap", L"ERROR", MB_OK);
		return false;
	}
	heightData.resize(mTerrain.terrainWidth * mTerrain.terrainHeight, 0);
	int k = 0;
	for (UINT i = 0; i < mTerrain.terrainWidth * mTerrain.terrainHeight; i++)
	{
		// Save input heightmap data scaled to our settings
		heightData[k++] = (input[i*offset]/255.f)*mTerrain.heightScale;//3 = offset
	}
	return true;
}

void Terrain::BuildQuadPatchVB()
{
	
	float Width = (mTerrain.terrainWidth - 1)*mTerrain.quadSize/2;// ((64-1)*1) / 2 = 31,5
	float Depth = (mTerrain.terrainHeight - 1)*mTerrain.quadSize/2;// ((64-1)*1) / 2 = 31,5

	float quadWidth = (mTerrain.terrainWidth - 1)*mTerrain.quadSize / (nrOfCols - 1);// ((64-1) * 1) / (64-1) = 1
	float quadDepth = (mTerrain.terrainHeight - 1)*mTerrain.quadSize / (nrOfRows - 1);// ((64-1) * 1) / (64 -1) = 1

	float du = 1.f / (nrOfCols - 1);
	float dv = 1.f / (nrOfRows - 1);

	for (UINT y = 0; y < nrOfCols; y++)// y = max 63
	{
		float posZ = Width - y * quadWidth;// 31,5 - y * 1 = -31,5
		for (UINT x = 0; x < nrOfRows; x++)// x = max 63
		{
			float posX = -Depth + x * quadDepth; // -31,5 + x * 1 = 31,5
			float posY = heightData[y*nrOfRows + x];// y * 64 + x = 4096

			vertices[y*nrOfRows + x].pos = XMFLOAT3(posX, posY, posZ);// 31,5, 24, -31,5
			
			// stretch texture
			vertices[y*nrOfRows + x].uv.x = x * du;
			vertices[y*nrOfRows + x].uv.y = y * dv;
			
			vertices[y*nrOfRows + x].normal = XMFLOAT3(0, 1, 0);
		}
	}

	calcNormal(vertices);
	// call function for calculating y here

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
	XMVECTOR normal3;
	XMVECTOR normal4;
	
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
			normal2 = XMVector3Cross(vec1, vec3);
			normal3 = XMVector3Cross(vec2, vec4);
			normal4 = XMVector3Cross(vec3, vec4);
			XMStoreFloat3(&vert[i * nrOfRows + j].normal, normal1);
			XMStoreFloat3(&vert[i * nrOfRows + j + 1].normal, normal2);
			XMStoreFloat3(&vert[(i + 1)*nrOfRows + j].normal, normal3);
			XMStoreFloat3(&vert[(i + 1)*nrOfRows + j + 1].normal, normal4);
		}
	}
}

void Terrain::BuildQuadPatchIB()
{
	std::vector<UINT> indices(nrOfQuadFaces * 4);

	int test1, test2, test3, test4;
	int k = 0;

	for (UINT i = 0; i < nrOfCols - 1; i++) { // 63
		for (UINT j = 0; j < nrOfRows - 1; j++) // 63
		{
			//top
			indices[k] = i * nrOfRows + j; // i * 64 + j = 4030, 0
			test1 = i * nrOfRows + j;
			indices[k+1] = i * nrOfRows + j + 1; // i * 64 + j + 1 = 4031, 1
			test2 = i * nrOfRows + j + 1;

			// bot
			indices[k+2] = (i + 1)*nrOfRows + j; // (i + 1) * 64 + j = 4094, 64
			test3 = (i + 1)*nrOfRows + j;
			indices[k+3] = (i + 1)*nrOfRows + j + 1; // (i + 1) * 64 + j + 1 = 4095, 65
			test4 = (i + 1)*nrOfRows + j + 1;
			k += 4;
		}
	}

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(UINT)*indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &indices[0];
	gDevice->CreateBuffer(&bufferDesc, &data, &indexBuffer);

}

float Terrain::getY(float x, float z)const
{
	float c = (x + 0.5f*GetWidth()) / mTerrain.quadSize;
	float d = (z - 0.5f*GetDepth()) / -mTerrain.quadSize;
	
	if (c < 0 || d < 0 || d > GetDepth() || c > GetWidth())
	{
		return -1;
	}

	int row = (int)floorf(d);
	int col = (int)floorf(c);

	float A = heightData[row*mTerrain.terrainWidth + col];
	float B = heightData[row*mTerrain.terrainWidth + col + 1];
	float C = heightData[(row + 1)*mTerrain.terrainWidth + col];
	float D = heightData[(row + 1)*mTerrain.terrainWidth + col + 1];

	float s = c - (float)col;
	float t = d - (float)row;

	float uy, vy;
	if (s + t <= 1.f) // triangle 1
	{
		uy = B - A;
		vy = C - A;
		return A + s * uy + t * vy + 1;
	}
	else // tri 2
	{
		uy = C - D;
		vy = B - D;
		return D + (1.f - s)*uy + (1.f - t)*vy + 1;
	}
	/*
	for(int i = 0; i < nrOfRows; i++)
		for (int j = 0; j < nrOfCols; j++)
		{
			if (vertices[i*nrOfRows + j].pos.x == x && vertices[i*nrOfRows + j].pos.z == z)
				return vertices[i*nrOfRows + j].pos.y + 1;
		}*/
	return -1;
}

float Terrain::GetWidth()const
{
	return (mTerrain.terrainWidth - 1)*mTerrain.quadSize;
}

float Terrain::GetDepth()const
{
	return (mTerrain.terrainHeight - 1)*mTerrain.quadSize;
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

	gDeviceContext->PSSetShaderResources(0, 1, &gTextureSRV); // set texure every frame
	gDeviceContext->DrawIndexed(nrOfQuadFaces * 4, 0, 0);
}

void Terrain::LoadTexture(std::wstring filename)
{	
	HRESULT hr = CreateWICTextureFromFile(gDevice, (const wchar_t*)filename.c_str(), nullptr, &gTextureSRV);
	if (FAILED(hr))
	{
		hr = CreateWICTextureFromFile(gDevice, L"Textures/Default.jpg", nullptr, &gTextureSRV);
		MessageBox(NULL, L"Loading TERRAIN texture Failed!", L"ERROR", MB_OK);
	}

	gDeviceContext->PSSetShaderResources(0, 1, &gTextureSRV); // innan render
}

//
//bool Terrain::InBounds(int i, int j)
//{
//	// True if ij are valid indices; false otherwise.
//	return
//		i >= 0 && i < (int)mInfo.HeightmapHeight &&
//		j >= 0 && j < (int)mInfo.HeightmapWidth;
//}