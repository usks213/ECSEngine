/*****************************************************************//**
 * \file   EntityManager.h
 * \brief  エンティティマネージャー
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "World.h"


namespace ecs {

	/// @class EntityManager
	/// @brief エンティティマネージャー
	class EntityManager
	{
	public:

		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit EntityManager(World* pWorld) : m_pWorld(pWorld)
		{
		}

		/// @brief エンティティの生成
		/// @param archetype アーキタイプ
		/// @return 生成したエンティティ
		[[nodiscard]] Entity createEntity(const Archetype& archetype) const;

		/// @brief エンティティの生成
		template<typename ...Args>
		[[nodiscard]] Entity createEntity() const
		{
			constexpr auto archetype = Archetype::create<Args...>();
			return createEntity(archetype);
		}

		/// @brief エンティティの削除
		/// @param entity 削除するエンティティ
		void destroyEntity(const Entity& entity) const;

		/// @brief コンポーネントデータの追加
		/// @tparam T コンポーネントデータ型
		/// @param entity 対象のエンティティ
		/// @param data 追加するデータ
		template<typename T>
		void addComponentData(Entity& entity, const T& data)
		{
			auto newArchetype = m_pWorld->m_ChunkList[entity.m_chunkIndex].getArchetype();
			newArchetype.addType<T>();
			const auto newChunkIndex = getAndCreateChunkIndex(newArchetype);

			auto& chunk = m_pWorld->m_ChunkList[newChunkIndex];
			m_pWorld->m_ChunkList[entity.m_chunkIndex].moveEntity(entity, chunk);
			chunk.setComponentData(entity, data);
		}

		/// @brief コンポーネントデータを設定
		/// @tparam T コンポーネントデータ型
		/// @param entity 対象のエンティティ
		/// @param data 追加するデータ
		template<typename T, typename = std::enable_if_t<is_component_data<T>>>
		void setComponentData(const Entity& entity, const T& data)
		{
			m_pWorld->m_ChunkList[entity.m_chunkIndex].setComponentData(entity, data);
		}

		/// @brief コンポーネントデータを設定
		/// @tparam T コンポーネントデータ型
		/// @param entity 対象のエンティティ
		template<typename T, typename = std::enable_if_t<is_component_data<T>>>
		[[nodiscard]] T* getComponentData(const Entity& entity)
		{
			if (hasComponentData<T>(entity))
				return m_pWorld->m_ChunkList[entity.m_chunkIndex].getComponentData<T>(entity.m_index);
			else
				return nullptr;
		}

		/// @brief コンポーネントデータを持っているか判定
		/// @tparam T コンポーネントデータ型
		/// @param entity 対象のエンティティ
		template<typename T, typename = std::enable_if_t<is_component_data<T>>>
		[[nodiscard]] bool hasComponentData(const Entity& entity)
		{
			const auto& archetype = m_pWorld->m_ChunkList[entity.m_chunkIndex].getArchetype();
			for (int i = 0; i < archetype.getArchetypeSize(); ++i)
			{
				if (archetype.getTypeInfo(i) == TypeInfo::create<T>())
					return true;
			}
			return false;
		}

		/// @brief 指定したアーキタイプが含まれるすべてのチャンクを取得
		/// @param archetype アーキタイプ
		/// @return チャンクのリスト
		[[nodiscard]] std::vector<Chunk*> getChunkList(const Archetype& archetype) const;

		/// @brief 指定した型(可変)が含まれるすべてのチャンクを取得
		/// @tparam ...Args コンポーネントデータ型(可変)
		/// @return チャンクのリスト
		template<typename ...Args>
		[[nodiscard]] std::vector<Chunk*> getChunkList() const
		{
			constexpr auto archetype = Archetype::create<Args...>();
			return getChunkList(archetype);
		}

		/// @brief 指定した型(可変)が含まれるすべてのチャンクを取得
		/// @param tagName タグ名
		/// @return チャンクのリスト
		[[nodiscard]] std::vector<Chunk*> getChunkListByTag(std::string_view tagName) const
		{
			Archetype archetype;
			archetype.addTag(tagName);
			return getChunkList(archetype);
		}

		/// @brief 指定した型(可変)が含まれるすべてのチャンクを取得
		/// @param tagHash ハッシュ値
		/// @return チャンクのリスト
		[[nodiscard]] std::vector<Chunk*> getChunkListByTag(std::size_t tagHash) const
		{
			Archetype archetype;
			archetype.addTag(tagHash);
			return getChunkList(archetype);
		}

		/// @brief 指定したアーキタイプのチャンクを生成または取得
		/// @param archetype アーキタイプ
		/// @return チャンクのインデックス
		[[nodiscard]] std::uint32_t getAndCreateChunkIndex(const Archetype& archetype) const;

		/// @brief 指定した型(可変)のチャンクを生成または取得
		/// @tparam ...Args コンポーネントデータ型(可変)
		/// @return チャンクのインデックス
		template<typename ...Args>
		[[nodiscard]] std::uint32_t getAndCreateChunkIndex() const
		{
			constexpr auto archetype = Archetype::create<Args...>();
			return getAndCreateChunkIndex(archetype);
		}

	private:
		/// @brief 所属するワールド
		World* m_pWorld = nullptr;
	};

}