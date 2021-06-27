/*****************************************************************//**
 * \file   World.cpp
 * \brief  ���[���h
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "World.h"
#include "EntityManager.h"
#include "SystemBase.h"
#include <algorithm>

using namespace ecs;


/// @brief �R���X�g���N�^
World::World(WorldManager* pWorldManager):
	m_pWorldManager(pWorldManager)
{
	m_pEntityManager.reset(new EntityManager(this));
}

/// @brief �f�X�g���N�^
World::~World()
{
	for (auto&& system : m_SystemList)
	{
		system->onDestroy();
	}
}

/// @brief �G���e�B�e�B�}�l�[�W���[�̎擾
/// @return �G���e�B�e�B�}�l�[�W���[
EntityManager* World::getEntityManager() const
{
	return m_pEntityManager.get();
}

/// @brief �V�X�e���̍X�V
void World::update()
{
	for (auto&& system : m_SystemList)
	{
		system->onUpdate();
	}
}

/// @brief ���̃��[���h�̃f�[�^����������
/// SystemList�͌�������Ȃ�
/// @param other �������郏�[���h
void World::mage(World&& other)
{
	for (auto& otherChunk : m_ChunkList)
	{
		auto findItr = std::find(m_ChunkList.begin(), m_ChunkList.end(), otherChunk);
		// ������Ȃ�����
		if (m_ChunkList.end() == findItr)
		{
			m_ChunkList.push_back(std::move(otherChunk));
		}
		// ���ɂ���
		else
		{
			findItr->marge(std::move(otherChunk));
		}
	}

	other.m_ChunkList.clear();		// �������ĂȂ����烁�������[�N���邩���H�H
	other.m_SystemList.clear();
}

/// @brief �`�����N���X�g���o�C�i���X�g���[���ɏ����o��
/// @param output �����o����̃X�g���[��
void World::writeBinaryStream(BinaryStream& output)
{
	const auto chunkSize = m_ChunkList.size();
	// �`�����N���̏����o��
	output.write(chunkSize);
	// �f�[�^�̏����o��
	for (auto&& chunk : m_ChunkList)
	{
		chunk.writeBinaryStream(output);
	}
}

/// @brief �`�����N���X�g���o�C�i���X�g���[������ǂݍ���
/// @param output �ǂݍ��ݐ�̃X�g���[��
void World::readBinaryStream(BinaryStream& input)
{
	// �`�����N���̎擾
	std::size_t chunkSize;
	input.read(&chunkSize);
	// �`�����N���Ń��������m��
	m_ChunkList.resize(chunkSize);
	// �`�����N�f�[�^�̓ǂݍ���
	for (size_t i = 0; i < chunkSize; ++i)
	{
		m_ChunkList[i] = Chunk::readBinaryStream(input);
	}
}

/// @brief �V�X�e���̍X�V������
void World::sortSystem()
{
	std::sort(m_SystemList.begin(), m_SystemList.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return lhs->getExecutionOrder() < rhs->getExecutionOrder();
		});
}

/// @brief �V�X�e���������̃R�[���o�b�N
void World::callDoCreate(SystemBase* pSystem)
{
	pSystem->onCreate();
}