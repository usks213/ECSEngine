/*****************************************************************//**
 * \file   SubResource.h
 * \brief  �T�u���\�[�X
 * 
 * \author USAMI KOSHI
 * \date   2021/07/28
 *********************************************************************/
#pragma once

#include <cstdint>


/// @class SubResource
/// @brief �T�u���\�[�X
class SubResource
{
public:
	/// @brief �R���X�g���N�^
	SubResource() :
		pData(nullptr), ByteWidth(0),
		RowPitch(0), DepthPitch(0)
	{}

	/// @brief �f�X�g���N�^
	virtual ~SubResource() = default;

	/// @brief �f�[�^
	void* getData() { return pData; }
	/// @brief �P�f�[�^�̃T�C�Y
	std::uint32_t getByteSize() { return ByteWidth; }
	/// @brief �f�[�^�z��̕�
	std::uint32_t getWidth() { return RowPitch / getByteSize(); }
	/// @brief �f�[�^�z��̍���
	std::uint32_t getHeight() { return DepthPitch / getWidth(); }

public:
	void* pData;				// �f�[�^
	std::uint32_t ByteWidth;	// 1�f�[�^�̃T�C�Y(�o�C�g)
	std::uint32_t RowPitch;		// �f�[�^�̕�(�o�C�g)
	std::uint32_t DepthPitch;	// �f�[�^�̍���(�o�C�g)
};