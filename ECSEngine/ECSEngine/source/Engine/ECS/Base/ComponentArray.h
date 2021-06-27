/*****************************************************************//**
 * \file   ComponentArray.h
 * \brief  �R���|�[�l���g�z��
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

namespace ecs {

	/// @class ComponentArray
	/// @brief �w�肵���^���`�����N����R���|�[�l���g�z��Ƃ��Ĉ���
	/// @tparam T �^
	template<class T>
	class ComponentArray
	{
	public:
		/// @brief �R���X�g���N�^
		/// @param pBegin �`�����N���̌^�̐擪�A�h���X
		/// @param size �z��̃T�C�Y
		explicit ComponentArray(T* pBegin, const std::uint32_t size) :
			m_pBegin(pBegin), m_Size(size)
		{
		}

		/// @brief �Y�����A�N�Z�X
		/// @param index �C���f�b�N�X(index < n_Size)
		/// @return T�^�f�[�^�̎Q��
		T& operator[](const int index) { return m_pBegin[index]; }

		/// @brief �z��̐擪�A�h���X
		/// @return T�^ �擪�A�h���X
		T* begin() { return m_pBegin; }

		/// @brief �z��̖����A�h���X
		/// @return T�^ �����A�h���X
		T* end() { return m_pBegin + sizeof(T) * m_Size; }

		/// @brief �z��̗v�f��
		/// @return �v�f��
		std::uint32_t Count() { return m_Size; }

	private:
		T* m_pBegin = nullptr;
		std::uint32_t m_Size = 0;
	};
}