#pragma once
class Light_Dir
{

private:
	ConstantLightbuffer lightBufferData;

	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;

	ID3D11Texture2D* shadowBuffer;
	ID3D11DepthStencilView* shadowView;
	ID3D11RenderTargetView* shadowRTV;
	ID3D11ShaderResourceView* shadowSRV;

	ID3D11Buffer* lightPosBuffer;
	ID3D11Buffer* lightBuffer;
	void CreateLightBuffers();
private:
	XMFLOAT3 lightPosition;
	XMFLOAT3 lightTarget;
	XMFLOAT4X4 lightView;

	float width, height;
public:
	Light_Dir(XMFLOAT3 pos = { 0.f, 2.f, 0.f }, XMFLOAT3 target = { 0.f, 0.f, 1.f }, float width = 1024, float height = 768);
	void InitDirLight(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext);
	~Light_Dir();	
	
	void UpdateLightData();
	void Update();
	void SetViewport();
	void DepthStencil();

	ID3D11DepthStencilView* getShadowDSV() { return this->shadowView; }
	ID3D11RenderTargetView* getShadowRTV() { return this->shadowRTV; }
	ID3D11ShaderResourceView* getShadowSRV()const { return this->shadowSRV; }
	ID3D11Buffer* getLightPosBuffer() { return this->lightPosBuffer; }
	ID3D11Buffer* getLightBuffer() { return this->lightBuffer; }
	ConstantLightbuffer getBufferData() { return this->lightBufferData; }
};

