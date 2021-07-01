/*****************************************************************//**
 * \file   D3D11Utility.h
 * \brief  DirectX11Utility
 * 
 * \author USAMI KOSHI
 * \date   2021/06/21
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/D3DUtility.h>
#include <d3d11_1.h>
#include <Engine/Utility/Mathf.h>


namespace D3D {

	constexpr char SHADER_CB_NAME_SYSTEM[]		= "System";
	constexpr char SHADER_CB_NAME_TRANSFORM[]	= "Transform";
	constexpr char SHADER_CB_NAME_MATERIAL[]	= "Material";

	constexpr std::uint32_t SHADER_CB_SLOT_SYSTEM	 = 5;
	constexpr std::uint32_t SHADER_CB_SLOT_TRANSFORM = 6;
	constexpr std::uint32_t SHADER_CB_SLOT_MATERIAL  = 7;


	// マテリアルフラグ
	enum class Material_Flg : UINT
	{
		TEXTURE		= (1 << 1),
		LIGHT		= (1 << 2),
		SHADOW		= (1 << 3),
		FOG			= (1 << 4),
		MAX			= std::numeric_limits<UINT>::max(),
	};

	// システム定数バッファ
	struct SystemBuffer
	{
		Matrix _mView;
		Matrix _mProj;
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
		VectorUint4	_Flg;
	};
}
