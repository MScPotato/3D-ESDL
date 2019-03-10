#include "pch.h"
#include "Model.h"


Model::Model(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* gTextureSRV)
{
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
	this->gTextureSRV = gTextureSRV;

	
	XMStoreFloat4x4(&this->modelSpace, XMMatrixIdentity());
}


Model::~Model()
{
	//VertexBuffer->Release();
	//mtlBuffer->Release();
}



void Model::setTexture(ID3D11ShaderResourceView* TextureSRV)
{
	gTextureSRV = TextureSRV;
}

ID3D11ShaderResourceView* Model::getTexture()const
{
	return gTextureSRV;
}

HRESULT Model::CreateMTLBufferDesc()
{
	D3D11_BUFFER_DESC mtlBufferDesc;
	ZeroMemory(&mtlBufferDesc, sizeof(D3D11_BUFFER_DESC));
	mtlBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mtlBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mtlBufferDesc.ByteWidth = sizeof(MTLBuffer);
	mtlBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mtlBufferDesc.MiscFlags = 0;
	mtlBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &mtlBufferData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&mtlBufferDesc, &data, &mtlBuffer);
	gDeviceContext->PSSetConstantBuffers(0, 1, &mtlBuffer);
	return hr;
}

void Model::updateMTL()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	gDeviceContext->Map(mtlBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, &mtlBufferData, sizeof(MTLBuffer));

	gDeviceContext->Unmap(mtlBuffer, 0);
	gDeviceContext->PSSetConstantBuffers(0, 1, &mtlBuffer);
	
}

void Model::setMtlData(std::vector<Material> mtlcontent, int pos)
{

	mtlBufferData.Kd = mtlcontent.at(pos).LightData.Kd;
	mtlBufferData.Ka = mtlcontent.at(pos).LightData.Ka;
	mtlBufferData.Ns = mtlcontent.at(pos).LightData.Ns;

	//mtlBufferData.Kd = mtlcontent.at(pos).LightData.Kd;
	//mtlBufferData.Ka = mtlcontent.at(pos).LightData.Ka;
	//mtlBufferData.Tf = mtlcontent.at(pos).LightData.Tf;
	//mtlBufferData.Ks = mtlcontent.at(pos).LightData.Ks;
	//mtlBufferData.NiNsIl = mtlcontent.at(pos).LightData.NiNsIl;

	//mtlBufferData.Kd.w = 1;
	//mtlBufferData.Ka.w = 1;
	//mtlBufferData.Tf.w = 1;
	//mtlBufferData.Ks.w = 1;
	//mtlBufferData.NiNsIl.w = 1;
}

HRESULT Model::CreateModelData()
{
	// Describe the Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	// what type of buffer will this be?
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// what type of usage (press F1, read the docs)
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// how big in bytes all element in the buffer is.
	bufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * mesh.size());
	//int check = sizeof(Vertex);
	// this struct is created just to set a pointer to the
	// data containing the vertices.
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = mesh.data();

	// create a Vertex Buffer
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &VertexBuffer);
	return hr;
}

void Model::setVertexBuffer()
{
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &vertexSize, &offset);
}

std::vector<Vertex> Model::getMesh()
{
	return mesh;
}

void Model::pushMesh(Vertex data)
{
	mesh.push_back(data);
}

void Model::setModelSpace(float x, float y, float z)
{
	modelSpace._41 = x;
	modelSpace._42 = y;
	modelSpace._43 = z;
}

XMFLOAT4X4 Model::getModelSpace()const
{
	return modelSpace;
}

void Model::setModelScaling(float size)
{
	modelSpace._11 *= size;
	modelSpace._22 *= size;
	modelSpace._33 *= size;
}

//void Model::setRotation(float angle, bool x, bool y, bool z)
void Model::setRotation(float x, float y, float z)
{
	//if (x && y && z)
	//	XMStoreFloat4x4(&modelSpace, XMLoadFloat4x4(&modelSpace) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(angle), XMConvertToRadians(angle), XMConvertToRadians(angle)));
	//else if (x && y)
	//	XMStoreFloat4x4(&modelSpace, XMLoadFloat4x4(&modelSpace) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(angle), XMConvertToRadians(angle), 0));
	//else if(x && z)
	//	XMStoreFloat4x4(&modelSpace, XMLoadFloat4x4(&modelSpace) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(angle), 0, XMConvertToRadians(angle)));
	//else if(y && z)
	//	XMStoreFloat4x4(&modelSpace, XMLoadFloat4x4(&modelSpace) * XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(angle), XMConvertToRadians(angle)));
	//else
	//{
	//	if (x)
	//		XMStoreFloat4x4(&modelSpace, XMLoadFloat4x4(&modelSpace) * XMMatrixRotationX(XMConvertToRadians(angle)));
	//	else if (y)
	//		XMStoreFloat4x4(&modelSpace, XMLoadFloat4x4(&modelSpace) * XMMatrixRotationY(XMConvertToRadians(angle)));
	//	else if (z)
	//		XMStoreFloat4x4(&modelSpace, XMLoadFloat4x4(&modelSpace) * XMMatrixRotationZ(XMConvertToRadians(angle)));
	//}

	XMStoreFloat4x4(&modelSpace, XMLoadFloat4x4(&modelSpace) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));

	
	//modelSpace._22 *= cos(XM_PI / 2);
	//modelSpace._23 *= sin(XM_PI / 2);
	//modelSpace._32 *= (-sin(XM_PI / 2));
	//modelSpace._33 *= cos(XM_PI / 2);

}