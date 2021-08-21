// ピクセルシェーダ
#include "Common.hlsli"
#include "PBR.hlsli"


// パラメータ
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3	Normal	: NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float3 WorldPos	: POSITION;
};

struct PS_OUTPUT
{
	float4 Out0 : SV_Target0;
	float4 Out1 : SV_Target1;
	float4 Out2 : SV_Target2;
};

// PBRパラメータ
cbuffer PBRParam : register(b0)
{
	float _metallic = 1.0f;
	float _roughness = 1.0f;
	float2 _Dumey;
}


PS_OUTPUT PS(VS_OUTPUT input)
{
	float4 Color = _Color;
	
	if (_Flg & TEXTURE_FLG)
	{
		Color *= _MainTexture.Sample(_MainSampler, input.TexCoord);
	}
	
	PS_OUTPUT output;
	
	output.Out0 = float4(Color.rgb, _metallic);
	output.Out1 = float4(input.Normal, _roughness);
	output.Out2 = float4(input.WorldPos, 1);
	return output;
}
