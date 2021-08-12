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
	Vector4 color;		 // �F
	Vector4 ambient;	 // ����
	Vector4 direction;	 // ����
};

struct PointLightData
{
	Vector4 color;		 // �F
	Vector3 position;    // �ʒu
	float	range;       // �͈�
	Vector4 attenuation; // ����
};

struct SpotLightData
{
	Vector4 color;		 // �F
	Vector3 position;    // �ʒu
	float	range;       // �͈�
	Vector4 attenuation; // ����
	Vector3 direction;   // ����
	float	spot;        // �X�|�b�g
};