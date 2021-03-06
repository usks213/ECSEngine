/*****************************************************************//**
 * \file   ShaderResource.h
 * \brief  シェーダーリソース
 * 
 * \author USAMI KOSHI
 * \date   2021/08/24
 *********************************************************************/
#pragma once

#include "LightData.h"


namespace SHADER {
	// CBuffer Name
	constexpr char SHADER_CB_NAME_GBUFFER[] = "Gbuffer";
	constexpr char SHADER_CB_NAME_SYSTEM[] = "System";
	constexpr char SHADER_CB_NAME_TRANSFORM[] = "Transform";
	constexpr char SHADER_CB_NAME_ANIMATION[] = "Animation";
	constexpr char SHADER_CB_NAME_MATERIAL[] = "Material";
	// CBuffer Slot
	constexpr std::uint32_t SHADER_CB_SLOT_GBUFFER = 4;
	constexpr std::uint32_t SHADER_CB_SLOT_SYSTEM = 5;
	constexpr std::uint32_t SHADER_CB_SLOT_TRANSFORM = 6;
	constexpr std::uint32_t SHADER_CB_SLOT_ANIMATION = 7;
	constexpr std::uint32_t SHADER_CB_SLOT_MATERIAL = 8;
	// Texture Slot
	constexpr std::uint32_t SHADER_SRV_SLOT_MAINTEX = 4;
	constexpr std::uint32_t SHADER_SRV_SLOT_SHADOWMAP = 5;
	constexpr std::uint32_t SHADER_SRV_SLOT_SKYDOOM = 6;
	constexpr std::uint32_t SHADER_SRV_SLOT_GRABTEX = 7;
	// StructuredBuffer Slot
	constexpr std::uint32_t SHADER_SRV_SLOT_POINTLIGHT = 8;
	constexpr std::uint32_t SHADER_SRV_SLOT_SPOTLIGHT = 9;
	// Samplear Slot
	constexpr std::uint32_t SHADER_SS_SLOT_MAIN = 4;
	constexpr std::uint32_t SHADER_SS_SLOT_SHADOW = 5;
	constexpr std::uint32_t SHADER_SS_SLOT_SKYBOX = 6;
	// UAV Slot

	// max register
	constexpr std::uint32_t MAX_CB_SLOT_COUNT = 15;
	constexpr std::uint32_t MAX_SS_SLOT_COUNT = 16;
	constexpr std::uint32_t MAX_SRV_SLOT_COUNT = 16;
	constexpr std::uint32_t MAX_UAV_SLOT_COUNT = 16;

	// Light
	constexpr std::uint32_t MAX_POINT_LIGHT_COUNT = 128;
	constexpr std::uint32_t MAX_SPOT_LIGHT_COUNT = 128;

	// TransformCB WorldMatrix
	constexpr std::uint32_t MAX_TRANSFORM_MATRIX_COUNT = 1000;

	// AnimationCB Bone
	constexpr std::uint32_t MAX_ANIMATION_BONE_COUNT = 512;

	// マテリアルフラグ
	enum class Material_Flg : UINT
	{
		NONE = 0,
		TEXTURE = (1 << 1),
		LIGHT = (1 << 2),
		SHADOW = (1 << 3),
		FOG = (1 << 4),
		ALL = std::numeric_limits<UINT>::max(),
		MAX = ALL,
	};

	struct CameraData
	{
		Matrix _mView;
		Matrix _mProj;
		Matrix _mProj2D;
		Matrix _mViewInv;
		Matrix _mProjInv;
		Vector4 _viewPos;
	};

	// システム定数バッファ
	struct SystemBuffer
	{
		CameraData _mainCamera;
		CameraData _shadowCamera;

		DirectionalLightData _directionalLit;

		std::uint32_t _pointLightNum;
		std::uint32_t _spotLightNum;

		float			_time;
		std::uint32_t	_frame;
	};

	// トランスフォーム定数バッファ
	struct TransformBuffer
	{
		Matrix _mWorld[MAX_TRANSFORM_MATRIX_COUNT];
	};

	// アニメーション定数バッファ
	struct AnimationBuffer
	{
		Matrix _mBone[MAX_ANIMATION_BONE_COUNT];
	};

	// マテリアル定数バッファ
	struct MaterialBuffer
	{
		Vector4		_Color = Vector4(1, 1, 1, 1);
		Matrix		_mTex;
		UINT		_Flg = (UINT)Material_Flg::ALL;
		Vector3		_padding3;
	};
}
