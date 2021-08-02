/*****************************************************************//**
 * \file   World.cpp
 * \brief  ���[���h
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "World.h"
#include "EntityManager.h"
#include "GameObjectManager.h"
#include "SystemBase.h"
#include "RenderPipeline.h"
#include <algorithm>

#include <fstream>
#include <iostream>

using namespace ecs;


/// @brief �R���X�g���N�^
World::World(WorldManager* pWorldManager):
	m_pWorldManager(pWorldManager)
{
	m_pEntityManager = std::make_unique<EntityManager>(this);
	m_pGameObjectManager = std::make_unique<GameObjectManager>(this);
	m_pRenderPipeline = std::make_unique<RenderPipeline>(this);
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

/// @brief �Q�[���I�u�W�F�N�g�}�l�[�W���[�̎擾
/// @return �Q�[���I�u�W�F�N�g�}�l�[�W���[
GameObjectManager* World::getGameObjectManager() const
{
	return m_pGameObjectManager.get();
}

/// @brief �����_�[�p�C�v���C���̎擾
/// @return �����_�[�p�C�v���C��
RenderPipeline* World::getRenderPipeline() const
{
	return m_pRenderPipeline.get();
}

/// @brief �V�X�e���̍X�V
void World::update()
{
	// �X�^�[�g�A�b�v
	m_pGameObjectManager->StartUp();

	for (auto&& system : m_SystemList)
	{
		system->onUpdate();
	}

	// �N���[���A�b�v
	m_pGameObjectManager->CleanUp();
}

void World::render()
{
	// �`��
	m_pRenderPipeline->onUpdate();
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

/// @brief ���[���h�̃V���A���C�Y
/// @param path �p�X
void World::serializeWorld(std::string path)
{
	// �`�����N�̏����o��
	BinaryStream bs;
	this->writeBinaryStream(bs);

	std::string chunkPath(path + "chunk.bin");
	std::ofstream ofs(chunkPath, std::ios::binary);
	ofs.write(bs.getBuffer(), bs.getSize());
	ofs.close();

	// �Q�[���I�u�W�F�N�g�̏����o��
	m_pGameObjectManager->Serialize(path);

}

/// @brief ���[���h�̃V���A���C�Y
/// @param path �p�X
bool World::deserializeWorld(std::string path)
{
	// �`�����N�t�@�C���̓ǂݍ���
	std::string chunkPath(path + "chunk.bin");
	std::ifstream ifs(chunkPath, std::ios::binary);
	if (!ifs.is_open()) return false;

	ifs.seekg(0, std::ios_base::end);
	const int fileSize = static_cast<const int>(ifs.tellg());
	ifs.seekg(0, std::ios_base::beg);
	const auto fileBuffer = std::make_unique<char[]>(fileSize);
	ifs.read(fileBuffer.get(), fileSize);
	BinaryStream bs;
	bs.write(fileBuffer.get(), fileSize);
	ifs.close();

	// �����ǂݍ��߂��ꍇ�̂�
	if (m_pGameObjectManager->Deserialize(path))
	{
		this->readBinaryStream(bs);
	}

	// �V�X�e���̏�����
	for (auto&& system : m_SystemList)
	{
		system->onDestroy();
		system->onCreate();
	}

	return true;
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