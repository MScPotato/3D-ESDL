#pragma once
class Particles
{
private:
	ID3D11DeviceContext* gDeviceContext;
	ID3D11Buffer* aBuffer;
public:
	Particles();
	void Init(ID3D11DeviceContext* gDeviceContext);
	~Particles();

	void Draw();
};

