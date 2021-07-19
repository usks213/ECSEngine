/*****************************************************************//**
 * \file   Mathf.h
 * \brief  �Z�p
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


struct VectorUint4
{
	VectorUint4() {
		x = y = z = w = 0;
	}

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

namespace Mathf
{
	// ����
	inline float Dot(Vector3 vec1, Vector3 vec2)
	{
		float dot;

		dot = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;

		return dot;
	}

	// �O��
	inline Vector3 Cross(Vector3 vec1, Vector3 vec2)
	{
		Vector3 vec;
		vec.x = vec1.y * vec2.z - vec1.z * vec2.y;
		vec.y = vec1.z * vec2.x - vec1.x * vec2.z;
		vec.z = vec1.x * vec2.y - vec1.y * vec2.x;
		return vec;
	}

	// �O�_�O��
	inline Vector3 Cross(Vector3 center, Vector3 pos1, Vector3 pos2)
	{
		return Cross(pos1 - center, pos2 - center);
	}

	// �O��2D
	inline float Cross2D(Vector3 vec1, Vector3 vec2)
	{
		return vec1.x * vec2.y - vec1.y * vec2.x;
	}

	// �x�N�g���̒���
	inline float Length(Vector3 vec)
	{
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	// ��_�Ԃ̋���
	inline float Length(Vector3 vec1, Vector3 vec2)
	{
		Vector3 vec = vec1 - vec2;
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	// �x�N�g���̐��K��
	inline Vector3 Normalize(Vector3 v)
	{
		Vector3 vec = v;
		float mag = Length(vec);
		if (!mag) return vec;
		vec.x /= mag;
		vec.y /= mag;
		vec.z /= mag;

		return vec;
	}

	// �ǂ���x�N�g��
	inline Vector3 WallScratchVector(Vector3 front, Vector3 normal)
	{
		normal.Normalize();
		return (front - normal * Dot(front, normal));
	}

	// ���˃x�N�g��
	inline Vector3 WallReflectVector(Vector3 front, Vector3 normal)
	{
		normal.Normalize();
		return (front - normal * Dot(front, normal) * 2.0f);
	}

	// �����x�N�g��
	inline Vector3 WallVerticalVector(Vector3 front, Vector3 normal)
	{
		normal.Normalize();
		return normal * Dot(front, normal);
	}

	// X����]
	inline Vector3 RotationX(const Vector3& vec, float angle)
	{
		float rad = DirectX::XMConvertToRadians(angle);
		Vector3 v = vec;

		v.y = vec.y * cosf(rad) + vec.z * sinf(rad);
		v.z = -vec.y * sinf(rad) + vec.z * cosf(rad);

		return v;
	}

	// Y����]
	inline Vector3 RotationY(const Vector3& vec, float angle)
	{
		float rad = DirectX::XMConvertToRadians(angle);
		Vector3 v = vec;

		v.x = vec.x * cosf(rad) - vec.z * sinf(rad);
		v.z = vec.x * sinf(rad) + vec.z * cosf(rad);

		return v;
	}

	// Z����]
	inline Vector3 RotationZ(const Vector3& vec, float angle)
	{
		float rad = DirectX::XMConvertToRadians(angle);
		Vector3 v = vec;

		v.x = vec.x * cosf(rad) + vec.y * sinf(rad);
		v.y = -vec.x * sinf(rad) + vec.y * cosf(rad);

		return v;
	}
}