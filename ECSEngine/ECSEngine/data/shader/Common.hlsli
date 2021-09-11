// 共通シェーダー
/*
DX11では、リソースをバインドできる一定量のスロットが与えられます。
最大15個の定数バッファー、16個のサンプラー、最大128個のテクスチャ、
8個のレンダーターゲット/ UAVをバインドできます。
Vulkan / DX12では、この決定は開発者に任されています。
*/

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
	float4x4 _mProj2D;
	float4x4 _mViewInv;
	float4x4 _mProjInv;
	float4 _viewPos;
	DirectionalLightData _directionalLit;
	uint _pointLightNum;
	uint _spotLightNum;
	float2 _padding2;
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
	float3		_padding3;
}



Texture2D		_MainTexture	: register(t4); // メインテクスチャ
Texture2D		_ShadowTexture	: register(t5); // シャドウマップ
Texture2D		_SkyTexture		: register(t6);	// スカイボックス

StructuredBuffer<PointLightData> _PointLights : register(t8);	// ポイントライト
StructuredBuffer<PointLightData> _SpotLights  : register(t9);	// スポットライト

SamplerState	_MainSampler	: register(s4); // メインサンプラ
SamplerState	_ShadowSampler	: register(s5); // シャドウマップ
SamplerState	_SkySampler		: register(s6); // スカイボックス


// 引数には正規化された反射ベクトルを代入
float2 SkyMapEquirect(float3 reflectionVector)
{
	float PI = 3.14159265359f;
	float phi = acos(-reflectionVector.y);
	float theta = atan2(-1.0f * reflectionVector.x, reflectionVector.z) + PI;
	return float2(theta / (PI * 2.0f), phi / PI);
}

// 深度からワールド座標を求める
float3 DepthToWorldPosition(float depth, float2 texCoord, float4x4 viewMatrixInv, float4x4 projMatrixInv)
{
	float z = depth * 2.0 - 1.0;

	float4 clipSpacePosition = float4(texCoord * 2.0 - 1.0, z, 1.0);
	float4 viewSpacePosition = mul(projMatrixInv, clipSpacePosition);

    // Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	float4 worldSpacePosition = mul(viewMatrixInv, viewSpacePosition);

	return worldSpacePosition.xyz;
}