// 2D用頂点シェーダ
#include "Common.hlsli"

// パラメータ
struct VS_INPUT {
	float3	Position	: POSITION;
	float2	TexCoord	: TEXCOORD0;
	float4	Diffuse		: COLOR0;
	uint	instID		: SV_InstanceID;
};

struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float2	TexCoord	: TEXCOORD0;
	float4	Diffuse		: COLOR0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	int n = input.instID % MAX_TRANSFORM;
	float4 P = mul(float4(input.Position, 1.0f), _mWorld[n]);
	P = mul(P, _mainCamera._mView);
	output.Position = mul(P, _mainCamera._mProj);
	output.TexCoord = mul(float4(input.TexCoord, 0.0f, 1.0f), _mTex).xy;
	output.Diffuse = input.Diffuse;
	return output;
}
