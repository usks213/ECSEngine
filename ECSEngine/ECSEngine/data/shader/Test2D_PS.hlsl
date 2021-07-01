// 2D�p�s�N�Z���V�F�[�_
#include "Common.hlsli"

// �p�����[�^
struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float2	TexCoord	: TEXCOORD0;
	float4	Diffuse		: COLOR0;
};

Texture2D    g_texture : register(t0);	// �e�N�X�`��
SamplerState g_sampler : register(s0);	// �T���v��

float4 PS(VS_OUTPUT input) : SV_Target0
{
	float4 Color = input.Diffuse;

	return Color;
}
