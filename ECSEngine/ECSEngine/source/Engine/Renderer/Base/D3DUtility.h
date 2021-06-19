/*****************************************************************//**
 * \file   D3DUtility.h
 * \brief  DirectX11�ėp
 * 
 * \author USAMI KOSHI
 * \date   2021/06/17
 *********************************************************************/
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <memory>

#include <comdef.h>
#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;



#define CHECK_FAILED(hr)													\
	if (FAILED(hr)) {														\
		static char szResult[256];                                          \
		_com_error  err(hr);                                                \
		LPCTSTR     errMsg = err.ErrorMessage();							\
        sprintf_s(szResult, sizeof(szResult), "[ERROR] HRESULT: %s",errMsg);\
		MessageBox(nullptr, szResult, "Err", MB_ICONSTOP);					\
		throw std::exception(szResult);     								\
	}

/// @enum ERasterizeState
/// @brief ���X�^���C�U�X�e�[�g
enum class ERasterizeState : std::uint8_t
{
	UNKNOWN = 0,				// �s��
	CULL_NONE,				// �J�����O�Ȃ�
	CULL_FRONT,				// �\�ʃJ�����O
	CULL_BACK,				// ���ʃJ�����O
	CULL_NONE_WIREFRAME,	// ���C���[�t���[�� �J�����O�Ȃ�
	CULL_FRONT_WIREFRAME,	// ���C���[�t���[�� �\�ʃJ�����O
	CULL_BACK_WIREFRAME,	// ���C���[�t���[�� ���ʃJ�����O
	SHADOW,					// �V���h�E�p
	MAX,					// ���X�^���C�U�X�e�[�g�̐�
};

/// @enum ESamplerState
/// @brief �T���v���X�e�[�g
enum class ESamplerState : std::uint8_t
{
	NONE = 0,			// �T���v���Ȃ�
	LINEAR_CLAMP,		// ���j�A�N�����v
	POINT_CLAMP,		// �|�C���g�N�����v
	ANISOTROPIC_CLAMP,	// �ٕ����N�����v
	LINEAR_WRAP,			// ���j�A���b�v
	POINT_WRAP,			// �|�C���g���b�v
	ANISOTROPIC_WRAP,	// �ٕ������b�v
	SHADOW,				// �V���h�E�p
	MAX,				// �T���v���X�e�[�g�̐�
};

/// @enum EBlendState
/// @brief �u�����h�X�e�[�g
enum class EBlendState : std::uint8_t
{
	UNKNOWN = 0,	// �s��
	NONE,		// �u�����h�Ȃ�
	ALPHA,		// ����������
	ADD,		// ���Z����
	SUB,		// ���Z����
	MUL,		// ��Z����
	INV,		// ���]����
	MAX,		// �u�����h�X�e�[�g�̐�
};

/// @enum EDepthStencilState
/// @brief �[�x�X�e���V���X�e�[�g
enum class EDepthStencilState : std::uint8_t
{
	UNKNOWN = 0,
	ENABLE_TEST_AND_ENABLE_WRITE,	// �[�x�X�e���V���e�X�g�L�� & �[�x�X�e���V���X�V�L��
	ENABLE_TEST_AND_DISABLE_WRITE,	// �[�x�X�e���V���e�X�g�L�� & �[�x�X�e���V���X�V����
	DISABLE_TEST_AND_ENABLE_WRITE,	// �[�x�X�e���V���e�X�g���� & �[�x�X�e���V���X�V�L��
	DISABLE_TEST_AND_DISABLE_WRITE,	// �[�x�X�e���V���e�X�g���� & �[�x�X�e���V���X�V����
	MAX,							// �[�x�X�e���V���X�e�[�g�̐�
};

/// @enum EPrimitiveTopology
/// @brief �v���~�e�B�u�g�|���W
enum class EPrimitiveTopology : std::uint8_t
{
	UNKNOWN = 0,						// �v���~�e�B�u�g�|���W�s��
	TRIANGLE_LIST,						// �g���C�A���O�����X�g
	TRIANGLE_STRIP,						// �g���C�A���O���X�g���b�v
	POINT_LIST,							// �|�C���g���X�g
	LINE_LIST,							// ���C�����X�g
	LINE_STRIP,							// ���C���X�g���b�v
	TRIANGLE_CONTROL_POINT_PATCHLIST,	// �g���C�A���O���R���g���[���|�C���g�p�b�`���X�g
	MAX,								// �v���~�e�B�u�g�|���W�̐�
};

