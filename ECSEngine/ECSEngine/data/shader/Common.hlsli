// 共通シェーダー

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
	uint4		_Flg;
}


Texture2D		_MainTexture	: register(t7); // メインテクスチャ
SamplerState	_MainSampler	: register(s7); // メインサンプラ
