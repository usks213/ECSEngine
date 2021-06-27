/*****************************************************************//**
 * \file   BinaryStream.h
 * \brief  �o�C�i���X�g���[��
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <vector>

/// @class BinaryStream
/// @brief �o�C�i���X�g���[��
class BinaryStream
{
public:

	/// @brief �R���X�g���N�^
	/// @param isBigSize �傫���o�b�t�@�T�C�Y���K�v��
	explicit BinaryStream(bool isBigSize = true)
	{
		// �傫���T�C�Y���K�v�ȏꍇ�������̂ł��炩����1MB�m��
		if (isBigSize)
			m_Buffer.reserve(1024 * 1024);
	}

	/// @brief �w�肳�ꂽ�l���o�C�i���o�b�t�@�ɏ����o��(�^�w��)
	/// is_trivially_copyable_v �� std::memcpy()�\�Ȍ^
	/// @tparam T �^
	/// @tparam enable_if_t std::memcpy()�\�Ȍ^
	/// @param value �l
	template<class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	void write(const T& value)
	{
		write(&value, sizeof(T));
	}

	/// @brief �w�肳�ꂽ�l���o�C�i���o�b�t�@�ɏ����o��(�ėp�|�C���g)
	/// @param pBuff �����o�����̔ėp�|�C���g
	/// @param size �����o���T�C�Y
	void write(const void* pBuff, std::size_t size)
	{
		const auto pByte = static_cast<const std::byte*>(pBuff);
		m_Buffer.insert(m_Buffer.end(), pByte, pByte + size);
	}

	/// @brief �^�T�C�Y���o�C�g�o�b�t�@����ǂݎ��
	/// @tparam T �^
	/// @tparam enable_if_t std::memcpy()�\�Ȍ^
	/// @param pValue �R�s�[��̃o�b�t�@
	template<class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	void read(T* pValue)
	{
		read(pValue, sizeof(T));
	}

	/// @brief �w��T�C�Y���o�C�g�o�b�t�@����ǂݎ��
	/// @param pBuff �R�s�[��̃o�b�t�@
	/// @param size �ǂݎ��T�C�Y
	void read(void* pBuff, const std::size_t size)
	{
		std::memcpy(pBuff, m_Buffer.data() + m_Count, size);
		m_Count += size;
	}

	/// @brief (const char*)�o�C�g�o�b�t�@�̎擾
	/// ofstream::write�p
	/// @return �擪�A�h���X
	[[nodiscard]] const char* getBuffer() const
	{
		return reinterpret_cast<const char*>(m_Buffer.data());
	}

	/// @brief �o�C�g�o�b�t�@�̑傫��
	/// @return �o�C�g��
	[[nodiscard]] std::size_t getSize() const
	{
		return m_Buffer.size();
	}

private:
	/// @brief �o�C�g�o�b�t�@
	std::vector<std::byte> m_Buffer;
	/// @brief ���݂̓ǂݎ��ʒu�w���q
	std::size_t m_Count = 0;

};