// 共通シェーダー

#include "Light.hlsli"

// フラグ
#define TEXTURE_FLG		(1 << 1)
#define LIGHT_FLG		(1 << 2)
#define SHADOW_FLG		(1 << 3)
#define FOG_FLG			(1 << 4)


// システム定数バッファ
cbuffer System : register(b5)
{
	float4x4 _mView;
	float4x4 _mProj;
	float4 _viewPos;
	DirectionalLightData _directionalLit;
}

// トランスフォーム定数バッファ
cbuffer Transform : register(b6)
{
	float4x4 _mWorld;
}

// マテリアル定数バッファ
cbuffer Material : register(b7)
{
	float4		_Color;
	float4x4	_mTex;
	uint		_Flg;
	float3		_Dummy;
}



Texture2D		_MainTexture	: register(t4); // メインテクスチャ
Texture2D		_ShadowTexture	: register(t5); // シャドウマップ
TextureCube		_SkyTexture		: register(t6); // スカイボックス

SamplerState	_MainSampler	: register(s4); // メインサンプラ
SamplerState	_ShadowSampler	: register(s5); // シャドウマップ
SamplerState	_SkySampler		: register(s6); // スカイボックス
