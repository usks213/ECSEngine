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

cbuffer WaveParam : register(b0)
{
	float _waveCycle = 2.5f;
	float _waveLength = 5.0f;
	float _waveAmplitude = 1.0f;
	float _waveMaxLength = 100.0f;
	float _waveOffset = 0.1f;
}

float GetWaveHeight(float2 posXZ)
{
	float PI = 3.14159265359f;
	float len = length(posXZ);
	float atte = max(-abs(len * len * (0.1f / _waveMaxLength)) + 1, 0.0f);
	return sin(2 * PI * (len / _waveLength - _time / _waveCycle)) * _waveAmplitude * atte;
}

VS_OUTPUT VS(VS_INPUT input)
{
	// 波の計算
	
	// 座標
	float4 pos = float4(input.Position, 1.0f);
	pos.y += GetWaveHeight(input.Position.xz);
	
	// 法線の再計算
	float hx0 = GetWaveHeight(float2(pos.x - _waveOffset, pos.z));
	float hx1 = GetWaveHeight(float2(pos.x + _waveOffset, pos.z));
	float hz0 = GetWaveHeight(float2(pos.x, pos.z - _waveOffset));
	float hz1 = GetWaveHeight(float2(pos.x, pos.z + _waveOffset));
	float3 normal = normalize(float3(hx1 - hx0, 1.0f, hz1 - hz0));
	
	VS_OUTPUT output;
	int n = input.instID % MAX_TRANSFORM;
	float4x4 mWVP = mul(_mWorld[n], _mainCamera._mView);
	mWVP = mul(mWVP, _mainCamera._mProj);
	output.Position = mul(pos, mWVP);
	output.Normal = mul(normal, (float3x3) _mWorld[n]).xyz;
	//output.TexCoord = mul(float4(input.TexCoord, 0.0f, 1.0f), _mTex).xy;
	output.TexCoord = input.TexCoord;
	output.WorldPos = mul(float4(input.Position, 1.0f), _mWorld[n]).xyz;
	
	return output;
}
