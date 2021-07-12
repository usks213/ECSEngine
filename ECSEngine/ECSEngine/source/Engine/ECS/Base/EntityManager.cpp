/*****************************************************************//**
 * \file   EntityManager.cpp
 * \brief  �G���e�B�e�B�}�l�[�W���[
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "EntityManager.h"
using namespace ecs;


/// @brief �G���e�B�e�B�̐���
/// @param archetype �A�[�L�^�C�v
/// @return ���������G���e�B�e�B
Entity EntityManager::createEntity(const Archetype& archetype) const
{
	const auto chunkIndex = getAndCreateChunkIndex(archetype);
	auto entity = m_pWorld->m_ChunkList[chunkIndex].createEntity();
	entity.m_chunkIndex = chunkIndex;

	return entity;
}

/// @brief �G���e�B�e�B�̍폜
/// @param entity �폜����G���e�B�e�B
void EntityManager::destroyEntity(const Entity& entity) const
{
	m_pWorld->m_ChunkList[entity.m_chunkIndex].destroyEntity(entity);
}

/// @brief �w�肵���A�[�L�^�C�v���܂܂�邷�ׂẴ`�����N���擾
/// @param archetype �A�[�L�^�C�v
/// @return �`�����N�̃��X�g
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

/// @brief �w�肵���A�[�L�^�C�v�̃`�����N�𐶐��܂��͎擾
/// @param archetype �A�[�L�^�C�v
/// @return �`�����N�̃C���f�b�N�X
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