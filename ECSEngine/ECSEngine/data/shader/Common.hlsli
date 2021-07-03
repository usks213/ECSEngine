// ���ʃV�F�[�_�[

#include "Light.hlsli"

// �t���O
#define TEXTURE_FLG		(1 << 1)
#define LIGHT_FLG		(1 << 2)
#define SHADOW_FLG		(1 << 3)
#define FOG_FLG			(1 << 4)


// �V�X�e���萔�o�b�t�@
cbuffer System : register(b5)
{
	float4x4 _mView;
	float4x4 _mProj;
	float4 _viewPos;
	DirectionalLightData _directionalLit;
}

// �g�����X�t�H�[���萔�o�b�t�@
cbuffer Transform : register(b6)
{
	float4x4 _mWorld;
}

// �}�e���A���萔�o�b�t�@
cbuffer Material : register(b7)
{
	float4		_Color;
	float4x4	_mTex;
	uint		_Flg;
	float3		_Dummy;
}



Texture2D		_MainTexture	: register(t4); // ���C���e�N�X�`��
Texture2D		_ShadowTexture	: register(t5); // �V���h�E�}�b�v
TextureCube		_SkyTexture		: register(t6); // �X�J�C�{�b�N�X

SamplerState	_MainSampler	: register(s4); // ���C���T���v��
SamplerState	_ShadowSampler	: register(s5); // �V���h�E�}�b�v
SamplerState	_SkySampler		: register(s6); // �X�J�C�{�b�N�X
