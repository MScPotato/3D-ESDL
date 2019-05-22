#pragma once

class Model
{
private:
	HRESULT hr;
	std::vector<Vertex> mesh;
	//ModelLoader* loader;
	ID3D11Buffer* VertexBuffer;

	ID3D11Device* gDevice; 
	ID3D11DeviceContext* gDeviceContext;
	ID3D11ShaderResourceView* gTextureSRV;
	ID3D11ShaderResourceView* gNormalSRV;
	MTLBuffer mtlBufferData;
	ID3D11Buffer* mtlBuffer;
	//ID3D11Buffer* normalmapBuffer;

	XMFLOAT4X4 modelSpace;
	XMFLOAT3 worldCoords;

public:
	Model(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* gTextureSRV);
	~Model();
	
	void setVertexBuffer();
	void pushMesh(Vertex data);

	void setModelSpace(float x, float y, float z);
	XMFLOAT4X4 getModelSpace()const;
	XMFLOAT3 getWorldCoords()const { return worldCoords; }

	void setModelScaling(float size);
	//void setRotation(float angle, bool x, bool y, bool z);
	void setRotation(float x = 0, float y = 0, float z = 0);

	HRESULT CreateModelData();
	std::vector<Vertex> getMesh();

	void setTexture(ID3D11ShaderResourceView* TextureSRV);
	void setNormalMap(ID3D11ShaderResourceView* NormalSRV);
	ID3D11ShaderResourceView* getTexture()const;
	ID3D11ShaderResourceView* getNormalMap()const;

	void setMtlData(std::vector<Material> mtlcontent, int pos);
	HRESULT CreateMTLBufferDesc();
	void updateMTL();
	//void updateWorld();
};

