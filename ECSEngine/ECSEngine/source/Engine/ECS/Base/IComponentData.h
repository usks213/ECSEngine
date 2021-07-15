/*****************************************************************//**
 * \file   IComponentData.h
 * \brief  コンポーネントデータ・エンティティ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "TypeInfo.h"

/// @brief コンポーネントデータ型情報付加
#define ECS_DECLARE_COMPONENT_DATA(T)					\
DECLARE_TYPE_INFO( T )


namespace ecs {

	/// @struct IComponentData
	/// @brief コンポーネントのベース
	struct IComponentData
	{
	};

	/// @brief コンポーネントの最大保持数
	constexpr auto c_maxComponentSize = 16;

	/// @brief コンポーネントで扱う最大文字数
	constexpr int MAX_COMPONENT_STRING = 32;


	/// @brief Tがコンポーネントデータか判定
	/// @tparam T getTypeHash()を保持している型
	template<class T>
	constexpr bool is_component_data = std::is_base_of_v<IComponentData, T> && type::has_get_type_hash<T>;
	//std::is_trivially_destructible_v<T> && std::is_trivial_v<T>&&;
	// ↑これがあるとコンポーネンデータに構造体が持てない
	//	 この制約を外した場合、何か不具合が出るかも...

	/// @struct Entity
	/// @brief エンティティ
	struct Entity
	{
		/// @brief コンストラクタ
		/// @param index chunk内での番号
		explicit Entity(const std::uint32_t index) :
			m_chunkIndex(std::numeric_limits<std::uint32_t>::max()),
			m_index(index)
		{
		}

		/// @brief 
		/// @param chunkIndex chunkの番号
		/// @param index chunk内での番号
		Entity(const std::uint32_t chunkIndex, const std::uint32_t index) :
			m_chunkIndex(chunkIndex), m_index(index)
		{
		}

		/// @brief chunkの番号
		std::uint32_t m_chunkIndex;
		/// @brief chunk内での番号
		std::uint32_t m_index;
	};
}