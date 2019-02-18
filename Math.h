#pragma once

namespace Math
{
	static const float PI = DirectX::XM_PI;

	inline int RandomInt(int min, int max) {
		return min + rand() % ((max + 1) - min);
	}

}
