#include "pch.h"
#include "Particles.h"


Particles::Particles()
{
}

void Particles::Init(ID3D11DeviceContext* gDeviceContext)
{
	this->gDeviceContext = gDeviceContext;
}

Particles::~Particles()
{
}

void Particles::Draw()
{
	gDeviceContext->Dispatch(20, 20, 1);
	gDeviceContext->CSSetShader(nullptr, nullptr, 0);
	gDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	gDeviceContext->DrawInstancedIndirect(aBuffer, 0);
}