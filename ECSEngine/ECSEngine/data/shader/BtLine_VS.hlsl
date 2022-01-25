// 2D用頂点シェーダ
#include "Common.hlsli"

// パラメータ
struct VS_INPUT {
	float3	Position	: POSITION;
	float3	Color		: TEXCOORD0;
};

struct VS_OUTPUT {
	float4 Position		: SV_Position;
	float3 Color		: TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	float4x4 mVP = mul(_mainCamera._mView, _mainCamera._mProj);
	output.Position = mul(float4(input.Position, 1.0f), mVP);
	output.Color = input.Color;
	return output;
}
