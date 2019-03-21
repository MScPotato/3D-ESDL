#pragma once
class Frustum
{
	struct Plane
	{
		float a, b, c, d;
	};
public:
	Frustum();
	~Frustum();

	void ConstructFrustum(float screenDepth, XMFLOAT4X4 projMatrix, XMFLOAT4X4 viewMatrix);

	bool CheckCube(float centerX, float centerY, float centerZ, float radius);
private:
	Plane planes[6];

	float calcPlaneDotCoord(Plane plane, float x, float y, float z);
};

