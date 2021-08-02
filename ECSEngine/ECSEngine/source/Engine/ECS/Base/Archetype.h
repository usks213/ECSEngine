/*****************************************************************//**
 * \file   Archetype.h
 * \brief  アーキタイプ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "IComponentData.h"


namespace ecs {

	/// @struct Archetype
	/// @brief Componentの組み合わせ(データ構造体)を表す
	struct Archetype
	{
		/// @brief アーキタイプの一致
		/// @param other 別のアーキタイプ
		/// @return 一致ならTrue
		constexpr bool operator==(const Archetype& other) const
		{
			if (m_ArchetypeSize != other.m_ArchetypeSize)
				return false;

			for (size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i] != other.m_TypeDataList[i])
					return false;
			}
			return true;
		}

		/// @brief アーキタイプの不一致
		/// @param other 別のアーキタイプ
		/// @return 不一致ならTrue
		constexpr bool operator!=(const Archetype& other) const
		{
			return !(*this == other);
		}

		/// @brief 自身が持っている型情報を相手が全て持っているか判定
		/// @param other 相手のアーキタイプ
		/// @return 持っている場合はTrue
		[[nodiscard]] constexpr bool isIn(const Archetype& other) const
		{
			for (size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				auto isIn = false;
				for (size_t j = 0; j < other.m_ArchetypeSize; j++)
				{
					if (m_TypeDataList[i] == other.m_TypeDataList[j])
					{
						isIn = true;
					}
				}
				if (!isIn) return false;
			}
			return true;
		}

		/// @brief templateでComponentDataからArchetypeを生成
		/// @tparam ...Args 可変引数 ComponentData...
		/// @return 生成したアーキタイプ
		template<typename ...Args>
		static constexpr Archetype create()
		{
			Archetype result;
			result.createImplements<Args...>();

			// バブルソート
			for (auto i = 0; i < result.m_ArchetypeSize - 1; ++i)
			{
				for (auto j = i + 1; j < result.m_ArchetypeSize; ++j)
				{
					if (result.m_TypeDataList[i].getHash() > result.m_TypeDataList[j].getHash())
					{
						const auto work = result.m_TypeDataList[i];
						result.m_TypeDataList[i] = result.m_TypeDataList[j];
						result.m_TypeDataList[j] = work;
					}
				}
			}
			// メモリサイズ計算
			for (auto i = 0; i < result.m_ArchetypeSize; ++i)
			{
				result.m_ArchetypeMemorySize += result.m_TypeDataList[i].getSize();
			}

			return result;
		}

		/// @brief アーキタイプの追加
		/// @tparam T 追加する型
		/// @return 自身の参照
		template<typename T>
		constexpr Archetype& addType()
		{
			constexpr auto newType = TypeInfo::create<T>();
			m_ArchetypeMemorySize += sizeof(T);

			// ソート挿入
			for (std::size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i].getHash() > newType.getHash())
				{
					for (auto j = m_ArchetypeSize; j > i; --j)
					{
						m_TypeDataList[j] = m_TypeDataList[j - 1];
					}
					m_TypeDataList[i] = newType;
					++m_ArchetypeSize;
					return *this;
				}
			}

			m_TypeDataList[m_ArchetypeSize] = newType;
			m_ArchetypeSize++;

			return *this;
		}

		/// @brief 既存のハッシュ値とメモリサイズでアーキタイプの追加
		/// @param typeHash 型のハッシュ値
		/// @param typeSize 型のメモリサイズ
		/// @return 自身の参照
		constexpr Archetype& addType(const std::size_t typeHash, const std::size_t typeSize)//, std::string_view typeName)
		{
			const auto newType = TypeInfo::create(typeHash, typeSize);//, typeName);
			m_ArchetypeMemorySize += typeSize;

			// ソート挿入
			for (std::size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i].getHash() > newType.getHash())
				{
					for (auto j = m_ArchetypeSize; j > i; --j)
					{
						m_TypeDataList[j] = m_TypeDataList[j - 1];
					}
					m_TypeDataList[i] = newType;
					++m_ArchetypeSize;
					return *this;
				}
			}

			m_TypeDataList[m_ArchetypeSize] = newType;
			m_ArchetypeSize++;

			return *this;
		}

		/// @brief 名前からタグを追加
		/// @detail size=0
		/// @param typeName タグ名
		/// @return 自身の参照
		constexpr Archetype& addTag(std::string_view typeName)
		{
			return addType(TypeNameToTypeHash(typeName), 0);//, typeName);
		}

		/// @brief ハッシュ値でタグを追加
		/// @detail size=0, name=typeHash
		/// @param typeHash 型のハッシュ値
		/// @return 自身の参照
		constexpr Archetype& addTag(const std::size_t typeHash)
		{
			return addType(typeHash, 0);//, std::to_string(typeHash));
		}

		/// @brief 指定した型が何番目にあるか
		/// @tparam Type IComponentData型
		/// @return 一致した番号
		template<typename Type, typename = std::enable_if_t<is_component_data<Type>>>
		[[nodiscard]] constexpr std::size_t getIndex() const
		{
			for (auto i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i] == TypeInfo::create<Type>())
					return i;
			}
			return m_ArchetypeSize;
		}

		/// @brief 指定した型までのメモリサイズを取得
		/// @tparam Type IComponentData型
		/// @return Indexまでのバイト数
		template<typename Type, typename = std::enable_if_t<is_component_data<Type>>>
		[[nodiscard]] constexpr std::size_t getOffset() const
		{
			std::size_t result = 0;
			for (auto i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i] == TypeInfo::create<Type>())
					return result;
				result += m_TypeDataList[i].getSize();
			}
			return result;
		}

		///// @brief 指定した型名までのメモリサイズを取得
		///// @param typeName IComponentData型の名前
		///// @return Indexまでのバイト数
		//[[nodiscard]] constexpr std::size_t getOffset(std::string_view typeName) const
		//{
		//	std::size_t result = 0;
		//	for (auto i = 0; i < m_ArchetypeSize; ++i)
		//	{
		//		if (m_TypeDataList[i].getName() == typeName)
		//			return result;
		//		result += m_TypeDataList[i].getSize();
		//	}
		//	return result;
		//}

		/// @brief 指定したハッシュ値までのメモリサイズを取得
		/// @param typeHash タイプハッシュ
		/// @return Indexまでのバイト数
		[[nodiscard]] constexpr std::size_t getOffset(std::size_t typeHash) const
		{
			std::size_t result = 0;
			for (auto i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i].getHash() == typeHash)
					return result;
				result += m_TypeDataList[i].getSize();
			}
			return result;
		}

		/// @brief 指定したインデックスまでのメモリサイズを取得
		/// @param index N番号
		/// @return インデックスまでのバイト数
		[[nodiscard]] constexpr std::size_t getOffsetByIndex(const std::size_t index) const
		{
			std::size_t result = 0;
			for (size_t i = 0; i < index; ++i)
			{
				result += m_TypeDataList[i].getSize();
			}
			return result;
		}

		/// @brief 型情報からインデックスを取得
		/// @param info 型情報
		/// @return 一致したインデックス
		[[nodiscard]] constexpr std::size_t getIndexByTypeInfo(const TypeInfo info) const
		{
			for (size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i] == info)
					return i;
			}
			return m_ArchetypeSize;
		}

		/// @brief 指定した型情報のメモリサイズを取得
		/// @param index 指定値
		/// @return 型のサイズ
		[[nodiscard]] constexpr std::size_t getSize(const std::size_t index) const
		{
			return m_TypeDataList[index].getSize();
		}

		///// @brief 指定した型名のメモリサイズを取得
		///// @param typeName 型名
		///// @return 型のサイズ
		//[[nodiscard]] constexpr std::size_t getSize(std::string_view typeName) const
		//{
		//	std::size_t result = 0;
		//	for (auto i = 0; i < m_ArchetypeSize; ++i)
		//	{
		//		if (m_TypeDataList[i].getName() == typeName)
		//			return m_TypeDataList[i].getSize();
		//	}
		//	return result;
		//}

		/// @brief 指定したハッシュ値のメモリサイズを取得
		/// @param typeHash ハッシュ値
		/// @return 型のサイズ
		[[nodiscard]] constexpr std::size_t getSizeByHash(std::size_t typeHash) const
		{
			std::size_t result = 0;
			for (auto i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i].getHash() == typeHash)
					return m_TypeDataList[i].getSize();
			}
			return result;
		}

		/// @brief 指定値のTypeInfoを取得する
		/// @param index 指定値
		/// @return 型情報
		[[nodiscard]] constexpr TypeInfo getTypeInfo(const std::size_t index) const
		{
			return m_TypeDataList[index];
		}

		/// @brief 格納されているArchetypeの数を取得
		/// @return Archetypeの数
		[[nodiscard]] constexpr std::size_t getArchetypeSize() const
		{
			return m_ArchetypeSize;
		}

		/// @brief 格納されているArchetypeのByte数を取得
		/// @return Archetypeの数
		[[nodiscard]] constexpr std::size_t getArchetypeMemorySize() const
		{
			return m_ArchetypeMemorySize;
		}

	private:
		/// @brief 可変引数から型情報を生成
		/// @tparam Head 先頭型情報
		/// @tparam ...Tails 可変型情報
		/// @tparam  型情報がコンポーネントデータか
		template<typename Head, typename ...Tails, typename = std::enable_if_t<is_component_data<Head>>>
		constexpr void createImplements()
		{
			// 型情報を生成
			m_TypeDataList[m_ArchetypeSize] = TypeInfo::create<Head>();
			m_ArchetypeSize++;

			// 最後まで再帰で繰り返す
			if constexpr (sizeof...(Tails) != 0)
			{
				createImplements<Tails...>();
			}
		}

		/// @brief アーキタイプのメモリサイズ
		std::size_t m_ArchetypeMemorySize = 0;
		/// @brief 格納されている数
		std::size_t m_ArchetypeSize = 0;
		/// @brief 型情報の配列
		TypeInfo m_TypeDataList[c_maxComponentSize];
	};
}