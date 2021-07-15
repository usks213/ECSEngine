/*****************************************************************//**
 * \file   IComponentData.h
 * \brief  �R���|�[�l���g�f�[�^�E�G���e�B�e�B
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "TypeInfo.h"

/// @brief �R���|�[�l���g�f�[�^�^���t��
#define ECS_DECLARE_COMPONENT_DATA(T)					\
DECLARE_TYPE_INFO( T )


namespace ecs {

	/// @struct IComponentData
	/// @brief �R���|�[�l���g�̃x�[�X
	struct IComponentData
	{
	};

	/// @brief �R���|�[�l���g�̍ő�ێ���
	constexpr auto c_maxComponentSize = 16;

	/// @brief �R���|�[�l���g�ň����ő啶����
	constexpr int MAX_COMPONENT_STRING = 32;


	/// @brief T���R���|�[�l���g�f�[�^������
	/// @tparam T getTypeHash()��ێ����Ă���^
	template<class T>
	constexpr bool is_component_data = std::is_base_of_v<IComponentData, T> && type::has_get_type_hash<T>;
	//std::is_trivially_destructible_v<T> && std::is_trivial_v<T>&&;
	// �����ꂪ����ƃR���|�[�l���f�[�^�ɍ\���̂����ĂȂ�
	//	 ���̐�����O�����ꍇ�A�����s����o�邩��...

	/// @struct Entity
	/// @brief �G���e�B�e�B
	struct Entity
	{
		/// @brief �R���X�g���N�^
		/// @param index chunk���ł̔ԍ�
		explicit Entity(const std::uint32_t index) :
			m_chunkIndex(std::numeric_limits<std::uint32_t>::max()),
			m_index(index)
		{
		}

		/// @brief 
		/// @param chunkIndex chunk�̔ԍ�
		/// @param index chunk���ł̔ԍ�
		Entity(const std::uint32_t chunkIndex, const std::uint32_t index) :
			m_chunkIndex(chunkIndex), m_index(index)
		{
		}

		/// @brief chunk�̔ԍ�
		std::uint32_t m_chunkIndex;
		/// @brief chunk���ł̔ԍ�
		std::uint32_t m_index;
	};
}