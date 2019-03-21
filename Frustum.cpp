#include "pch.h"
#include "Frustum.h"


Frustum::Frustum()
{
}


Frustum::~Frustum()
{
}

float Frustum::calcPlaneDotCoord(Plane plane, float x, float y, float z)
{
	return plane.a * x + plane.b * y + plane.c * z + plane.d * 1.f;
}

void Frustum::ConstructFrustum(float screenDepth, XMFLOAT4X4 projMatrix, XMFLOAT4X4 viewMatrix)
{
	float miniZ, r;
	XMFLOAT4X4 matrix;

	//calc minimum z didstance in the frustum
	miniZ = -projMatrix._43 / projMatrix._33;
	r = screenDepth / (screenDepth - miniZ);
	projMatrix._33 = r;
	projMatrix._43 = -r * miniZ;
	
	XMStoreFloat4x4(&matrix, XMMatrixMultiply(XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projMatrix)));

	//calc near plane of frustum
	planes[0].a = matrix._14 + matrix._13;
	planes[0].b = matrix._24 + matrix._23;
	planes[0].c = matrix._34 + matrix._33;
	planes[0].d = matrix._44 + matrix._43;

	//calc far plane of frustum
	planes[1].a = matrix._14 - matrix._13;
	planes[1].b = matrix._24 - matrix._23;
	planes[1].c = matrix._34 - matrix._33;
	planes[1].d = matrix._44 - matrix._43;

	//calc left plane of frustum
	planes[2].a = matrix._14 + matrix._11;
	planes[2].b = matrix._24 + matrix._21;
	planes[2].c = matrix._34 + matrix._31;
	planes[2].d = matrix._44 + matrix._41;

	//calc right plane of frustum
	planes[3].a = matrix._14 - matrix._11;
	planes[3].b = matrix._24 - matrix._21;
	planes[3].c = matrix._34 - matrix._31;
	planes[3].d = matrix._44 - matrix._41;

	//calc top plane of frustum
	planes[4].a = matrix._14 - matrix._12;
	planes[4].b = matrix._24 - matrix._22;
	planes[4].c = matrix._34 - matrix._32;
	planes[4].d = matrix._44 - matrix._42;

	//calc bottom plane of frustum
	planes[5].a = matrix._14 + matrix._12;
	planes[5].b = matrix._24 + matrix._22;
	planes[5].c = matrix._34 + matrix._32;
	planes[5].d = matrix._44 + matrix._42;
}

bool Frustum::CheckCube(float centerX, float centerY, float centerZ, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		if(calcPlaneDotCoord(planes[i], centerX - radius, centerY - radius, centerZ - radius) >= 0.f)
			continue;

		if (calcPlaneDotCoord(planes[i], centerX + radius, centerY - radius, centerZ - radius) >= 0.f)
			continue;

		if (calcPlaneDotCoord(planes[i], centerX - radius, centerY + radius, centerZ - radius) >= 0.f)
			continue;

		if (calcPlaneDotCoord(planes[i], centerX + radius, centerY + radius, centerZ - radius) >= 0.f)
			continue;

		if (calcPlaneDotCoord(planes[i], centerX - radius, centerY - radius, centerZ + radius) >= 0.f)
			continue;

		if (calcPlaneDotCoord(planes[i], centerX + radius, centerY - radius, centerZ + radius) >= 0.f)
			continue;

		if (calcPlaneDotCoord(planes[i], centerX - radius, centerY + radius, centerZ + radius) >= 0.f)
			continue;

		if (calcPlaneDotCoord(planes[i], centerX + radius, centerY + radius, centerZ + radius) >= 0.f)
			continue;

		return false;
	}

	return true;
}