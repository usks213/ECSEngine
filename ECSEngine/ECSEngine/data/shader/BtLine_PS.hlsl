// 2D用ピクセルシェーダ
#include "Common.hlsli"

// パラメータ
struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3 Color	: TEXCOORD0;
};


float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 Color = _Color;
	Color *= float4(input.Color, 1.0f);
	
	return Color;
}
