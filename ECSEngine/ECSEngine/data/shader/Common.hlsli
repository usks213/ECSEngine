// ���ʃV�F�[�_�[

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
	uint4		_Flg;
}


Texture2D		_MainTexture	: register(t7); // ���C���e�N�X�`��
SamplerState	_MainSampler	: register(s7); // ���C���T���v��
