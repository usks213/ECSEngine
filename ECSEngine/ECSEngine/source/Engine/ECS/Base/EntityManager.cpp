/*****************************************************************//**
 * \file   EntityManager.cpp
 * \brief  エンティティマネージャー
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "EntityManager.h"
using namespace ecs;


/// @brief エンティティの生成
/// @param archetype アーキタイプ
/// @return 生成したエンティティ
Entity EntityManager::createEntity(const Archetype& archetype) const
{
	const auto chunkIndex = getAndCreateChunkIndex(archetype);
	auto entity = m_pWorld->m_ChunkList[chunkIndex].createEntity();
	entity.m_chunkIndex = chunkIndex;

	return entity;
}

/// @brief エンティティの削除
/// @param entity 削除するエンティティ
void EntityManager::destroyEntity(const Entity& entity) const
{
	m_pWorld->m_ChunkList[entity.m_chunkIndex].destroyEntity(entity);
}

/// @brief 指定したアーキタイプが含まれるすべてのチャンクを取得
/// @param archetype アーキタイプ
/// @return チャンクのリスト
std::vector<Chunk*> EntityManager::getChunkList(const Archetype& archetype) const
{
	std::vector<Chunk*> result;
	result.reserve(4);
	for (auto&& chunk : m_pWorld->m_ChunkList)
	{
		if (archetype.isIn(chunk.getArchetype()))
			result.push_back(&chunk);
	}
	return result;
}

/// @brief 指定したアーキタイプのチャンクを生成または取得
/// @param archetype アーキタイプ
/// @return チャンクのインデックス
std::uint32_t EntityManager::getAndCreateChunkIndex(const Archetype& archetype) const
{
	auto chunkIndex = 0;
	for (auto&& chunk : m_pWorld->m_ChunkList)
	{
		if (chunk.getArchetype() == archetype)
			return chunkIndex;
		++chunkIndex;
	}

	m_pWorld->m_ChunkList.push_back(Chunk::create(archetype));
	return chunkIndex;
}