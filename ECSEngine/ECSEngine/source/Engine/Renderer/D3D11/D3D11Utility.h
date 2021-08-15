/*****************************************************************//**
 * \file   D3D11Utility.h
 * \brief  DirectX11Utility
 * 
 * \author USAMI KOSHI
 * \date   2021/06/21
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/D3DUtility.h>
#include <Engine/Renderer/Base/LightData.h>
#include <d3d11_1.h>
#include <Engine/Utility/Mathf.h>


namespace D3D {

	constexpr char SHADER_CB_NAME_SYSTEM[]		= "System";
	constexpr char SHADER_CB_NAME_TRANSFORM[]	= "Transform";
	constexpr char SHADER_CB_NAME_MATERIAL[]	= "Material";

	constexpr std::uint32_t SHADER_CB_SLOT_SYSTEM	 = 5;
	constexpr std::uint32_t SHADER_CB_SLOT_TRANSFORM = 6;
	constexpr std::uint32_t SHADER_CB_SLOT_MATERIAL  = 7;

	constexpr std::uint32_t SHADER_TEX_SLOT_MAIN	= 4;
	constexpr std::uint32_t SHADER_TEX_SLOT_SHADOW  = 5;
	constexpr std::uint32_t SHADER_TEX_SLOT_SKYBOX  = 6;

	constexpr std::uint32_t SHADER_SS_SLOT_MAIN		= 4;
	constexpr std::uint32_t SHADER_SS_SLOT_SHADOW	= 5;
	constexpr std::uint32_t SHADER_SS_SLOT_SKYBOX	= 6;

	constexpr std::uint32_t MAX_CBUFFER_SLOT_COUNT = 8;
	constexpr std::uint32_t MAX_SAMPLER_SLOT_COUNT = 8;
	constexpr std::uint32_t MAX_TEXTURE_SLOT_COUNT = 8;


	// マテリアルフラグ
	enum class Material_Flg : UINT
	{
		NONE		= 0,
		TEXTURE		= (1 << 1),
		LIGHT		= (1 << 2),
		SHADOW		= (1 << 3),
		FOG			= (1 << 4),
		ALL			= std::numeric_limits<UINT>::max(),
		MAX			= ALL,
	};

	// システム定数バッファ
	struct SystemBuffer
	{
		Matrix	_mView;
		Matrix	_mProj;
		Matrix	_mProj2D;
		Matrix	_mViewInv;
		Matrix	_mProjInv;
		Vector4 _viewPos;
		DirectionalLightData _directionalLit;
	};

	// トランスフォーム定数バッファ
	struct TransformBuffer
	{
		Matrix _mWorld;
	};

	// マテリアル定数バッファ
	struct MaterialBuffer
	{
		Vector4		_Color = Vector4(1,1,1,1);
		Matrix		_mTex;
		UINT		_Flg = (UINT)Material_Flg::ALL;
		Vector3		_Dummy;
	};
}
