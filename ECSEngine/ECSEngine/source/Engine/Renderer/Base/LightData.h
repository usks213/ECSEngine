/*****************************************************************//**
 * \file   LightData.h
 * \brief  ���C�g�f�[�^
 * 
 * \author USAMI KOSHI
 * \date   2021/07/02
 *********************************************************************/
#pragma once

#include <Engine/Utility/Mathf.h>

 // ���C�g�f�[�^

struct DirectionalLightData
{
	Vector4 color		= Vector4(1,1,1,1);				// �F+����
	Vector4 ambient		= Vector4(0.1f, 0.1f, 0.1f, 1); // ����
	Vector4 direction	= Vector4(1.0f, -1.0f, 1.0f, 1);// ����
};

struct PointLightData
{
	Vector4 color		= Vector4(1, 1, 1, 1);	// �F+����
	Vector3 position;							// �ʒu
	float	range		= 1.0f;					// �͈�
};

struct SpotLightData
{
	Vector4 color		= Vector4(1, 1, 1, 1);	// �F+����
	Vector3 position;							// �ʒu
	float	range		= 2.0f;					// �͈�
	Vector3 direction	= Vector3(0, -1, 0);;	// ����
	float	spot		= 0.5f;					// �~���̑傫��(0.0f~1.0f)
};