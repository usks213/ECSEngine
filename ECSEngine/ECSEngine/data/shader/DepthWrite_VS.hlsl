// 頂点シェーダ
#include "Common.hlsli"

// パラメータ
struct VS_INPUT {
	float3	Position	: POSITION0;
	uint	instID		: SV_InstanceID;
};

struct VS_OUTPUT {
	float4	Position	: SV_Position;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	int n = input.instID % MAX_TRANSFORM;
	float4x4 mWVP = mul(_mWorld[n], _shadowCamera._mView);
	mWVP = mul(mWVP, _shadowCamera._mProj);
	output.Position = mul(float4(input.Position, 1.0f), mWVP);
	return output;
}
