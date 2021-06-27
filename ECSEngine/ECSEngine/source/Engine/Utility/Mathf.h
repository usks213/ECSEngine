/*****************************************************************//**
 * \file   Mathf.h
 * \brief  éZèp
 * 
 * \author USAMI KOSHI
 * \date   2021/06/24
 *********************************************************************/
#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace DirectX;
using namespace DirectX::SimpleMath;


//struct Vector2 : public DirectX::XMFLOAT2
//{
//	Vector2() = default;
//	Vector2(float x, float y) {
//		this->x = x; this->y = y;
//	}
//};
//struct Vector3 : public DirectX::XMFLOAT3
//{
//	Vector3() = default;
//	Vector3(float x, float y, float z) {
//		this->x = x; this->y = y; this->z = z;
//	}
//};
//struct Vector4 : public DirectX::XMFLOAT4
//{
//	Vector4() = default;
//	Vector4(float x, float y, float z, float w) {
//		this->x = x; this->y = y; 
//		this->z = z; this->w = w;
//	}
//};

struct VectorUint4
{
	union 
	{
		struct
		{
			int x, y, z, w;
		};
		int n[4];
	};
};

struct AABB {
	Vector3 center;
	Vector3 scale;
	//Vector3 Min() { return center; };
	//Vector3 Max() { return center; };
};
