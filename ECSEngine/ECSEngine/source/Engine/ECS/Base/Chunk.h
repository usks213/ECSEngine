/*****************************************************************//**
 * \file   Chunk.h
 * \brief  チャンク
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "Archetype.h"
#include "BinaryStream.h"
#include "ComponentArray.h"



namespace ecs {

	/// @class Chunk
	/// @brief チャンク
	class Chunk
	{
	public:

		/// @brief 型情報(ICompoenetData)から生成
		/// @tparam ...Args 型情報(IComponentData)
		/// @param capacity チャンクの容量
		/// @return チャンク
		template<typename ...Args>
		static Chunk create(const std::uint32_t capacity = 1)
		{
			constexpr auto archetype = Archetype::create<Args...>();
			return create(archetype, capacity);
		}

		/// @brief アーキタイプから生成
		/// @param archetype  アーキタイプ
		/// @param capacity チャンクの容量
		/// @return チャンク
		static Chunk create(const Archetype& archetype, std::uint32_t capacity = 1);

		/// @brief チャンク一致
		/// @param other 別のチャンク
		/// @return 一致ならTure
		bool operator==(const Chunk& other) const;

		/// @brief チャンク不一致
		/// @param other 別のチャンク
		/// @return 不一致ならTure
		bool operator!=(const Chunk& other) const;

		/// @brief 現在のサイズでCapacityを切り詰め
		void fit();

		/// @brief エンティティを新規作成し、
		/// チャンクにコンポーネントデータを追加
		/// ※書き方によってはメモリが足らず危険性
		/// @tparam ...Args アーキタイプにある型情報
		/// @param ...value アーキタイプにあるデータ
		/// @return 作成したEntity
		template<typename ...Args>
		Entity addComponentData(const Args&... value)
		{
			// いっぱいの場合は再確保
			if (m_Capacity == m_Size)
			{
				resetMemory(m_Capacity * 2);
			}
			const auto entity = Entity(m_Size);

			addComponentDataImplements(value...);
			++m_Size;
			return entity;
		}

		/// @brief  エンティティを新規作成し、
		/// チャンクにコンポーネントデータを追加
		/// ※作成したデータは未定義
		/// @return 作成したEntity
		Entity createEntity();

		/// @brief Entityを他のチャンクに移動
		/// ※Archetypeが違う、相手がいっぱいの場合はオーバーフローするかも
		/// @param entity 移動させるEntity
		/// @param other 移動先Chunk
		void moveEntity(Entity& entity, Chunk& other);

		/// @brief Entityを削除
		/// @param entity 削除するEntity
		void destroyEntity(const Entity& entity);

		/// @brief 既存のEntityにデータ設定
		/// @tparam T アーキタイプにあるいずれかの型情報
		/// @param entity 設定するEntity
		/// @param data 設定するデータの値
		template<typename T>
		void setComponentData(const Entity& entity, const T& data)
		{
			// 異常終了 チャンクサイズ外 チャンクが間違ってるかも？
			if (entity.m_index >= m_Size)
				std::abort();

			using TType = std::remove_const_t<std::remove_reference_t<T>>;
			const auto offset = m_Archetype.getOffset<TType>() * m_Capacity;
			const auto currentIndex = sizeof(TType) * entity.m_index;
			// データをコピー
			std::memcpy(m_pBegin.get() + offset + currentIndex, &data, sizeof(TType));
		}

		/// @brief コンポーネントの配列を取得
		/// @tparam T アーキタイプにあるいずれかの型
		/// @return T型の配列
		template<class T>
		[[nodiscard]] ComponentArray<T> getComponentArray()
		{
			using TType = std::remove_const_t<std::remove_reference_t<T>>;
			auto offset = m_Archetype.getOffset<TType>() * m_Capacity;
			return ComponentArray<T>(reinterpret_cast<TType*>(m_pBegin.get() + offset), m_Size);
		}

		/// @brief 指定した型のデータを取得
		/// @tparam コンポーネントデータ型
		/// @param index インデックス
		/// @return 型のポインタ
		template<class T>
		[[nodiscard]] T* getComponentData(std::uint32_t index)
		{
			using TType = std::remove_const_t<std::remove_reference_t<T>>;
			auto offset = m_Archetype.getOffset<TType>() * m_Capacity;
			return reinterpret_cast<T*>(m_pBegin.get() + offset + m_Archetype.getSize(m_Archetype.getIndex<TType>()) * index);
		}

		/// @brief 指定した型名のデータを取得
		/// @param typeName 型名
		/// @param index インデックス
		/// @return 汎用ポインタ
		[[nodiscard]] void* getComponentData(std::string_view typeName, std::uint32_t index)
		{
			auto offset = m_Archetype.getOffset(typeName) * m_Capacity;
			return m_pBegin.get() + offset + m_Archetype.getSize(typeName) * index;
		}

		/// @brief 指定したハッシュ値のデータを取得
		/// @param typeHash ハッシュ値
		/// @param index インデックス
		/// @return 汎用ポインタ
		[[nodiscard]] void* getComponentData(std::size_t typeHash, std::uint32_t index)
		{
			auto offset = m_Archetype.getOffset(typeHash) * m_Capacity;
			return m_pBegin.get() + offset + m_Archetype.getSizeByHash(typeHash) * index;
		}

		/// @brief Archetypeを取得
		/// @return archetype
		[[nodiscard]] const Archetype& getArchetype() const
		{
			return m_Archetype;
		}

		/// @brief Chunkに登録されているデータの数を取得
		/// @return dataSize
		[[nodiscard]] std::uint32_t getSize() const
		{
			return m_Size;
		}

		/// @brief 他のチャンクを自身のチャンクに結合する
		/// @param other マージ元
		void marge(Chunk&& other);

		/// @brief ChunkのデータをBinaryStreamに出力
		/// @param output 出力先
		void writeBinaryStream(BinaryStream& output);

		/// @brief ChunkのデータをBinaryStreamから構築
		/// @param input 入力元
		/// @return 読み込んだチャンク
		static Chunk readBinaryStream(BinaryStream& input);

	private:
		/// @brief メモリを再確保
		/// @param capacity 新しいメモリサイズ
		void resetMemory(std::uint32_t capacity);

		/// @brief コンポーネントデータの追加 
		/// @tparam Head 先頭型情報
		/// @tparam ...Types 型情報(可変)
		/// @param head 先頭型情報
		/// @param ...type 型情報(可変)
		template<typename Head, typename ...Types>
		void addComponentDataImplements(Head&& head, Types&&... type)
		{
			using HeadType = std::remove_const_t<std::remove_reference_t<Head>>;
			const auto offset = m_Archetype.getOffset<HeadType>() * m_Capacity;
			const auto currentIndex = sizeof(HeadType) * m_Size;
			// メモリコピー
			std::memcpy(m_pBegin.get() + offset + currentIndex, &head, sizeof(HeadType));
			if constexpr (sizeof...(Types) > 0)
				addComponentDataImplements(type...);
		}

		/// @brief アーキタイプ
		Archetype m_Archetype;
		/// @brief チャンクの配列
		std::unique_ptr<std::byte[]> m_pBegin = nullptr;
		/// @brief 現在のサイズ
		std::uint32_t m_Size = 0;
		/// @brief 最大サイズ
		std::uint32_t m_Capacity = 1;
	};
}