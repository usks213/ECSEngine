// 頂点シェーダ
#include "Common.hlsli"

// パラメータ
struct VS_INPUT {
	float3	Position	: POSITION0;
	float3	Normal		: NORMAL0;
	float2	TexCoord	: TEXCOORD0;
	uint	instID		: SV_InstanceID;
};

struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float3	Normal		: NORMAL0;
	float2 TexCoord		: TEXCOORD0;
	float3 WorldPos		: POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	int n = input.instID % MAX_TRANSFORM;
	float4x4 mWVP = mul(_mWorld[n], _mainCamera._mView);
	mWVP = mul(mWVP, _mainCamera._mProj);
	output.Position = mul(float4(input.Position, 1.0f), mWVP);
	output.Normal = mul(input.Normal, (float3x3) _mWorld[n]).xyz;
	//output.TexCoord = mul(float4(input.TexCoord, 0.0f, 1.0f), _mTex).xy;
	output.TexCoord = input.TexCoord;
	output.WorldPos = mul(float4(input.Position, 1.0f), _mWorld[n]).xyz;
	return output;
}
