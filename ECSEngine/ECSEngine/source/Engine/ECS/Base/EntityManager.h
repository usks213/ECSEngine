/*****************************************************************//**
 * \file   EntityManager.h
 * \brief  �G���e�B�e�B�}�l�[�W���[
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "World.h"


namespace ecs {

	/// @class EntityManager
	/// @brief �G���e�B�e�B�}�l�[�W���[
	class EntityManager
	{
	public:

		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit EntityManager(World* pWorld) : m_pWorld(pWorld)
		{
		}

		/// @brief �G���e�B�e�B�̐���
		/// @param archetype �A�[�L�^�C�v
		/// @return ���������G���e�B�e�B
		Entity createEntity(const Archetype& archetype) const;

		/// @brief �G���e�B�e�B�̍폜
		/// @param entity �폜����G���e�B�e�B
		void destroyEntity(const Entity& entity) const;

		/// @brief �R���|�[�l���g�f�[�^�̒ǉ�
		/// @tparam T �R���|�[�l���g�f�[�^�^
		/// @param entity �Ώۂ̃G���e�B�e�B
		/// @param data �ǉ�����f�[�^
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

		/// @brief �R���|�[�l���g�f�[�^��ݒ�
		/// @tparam T �R���|�[�l���g�f�[�^�^
		/// @param entity �Ώۂ̃G���e�B�e�B
		/// @param data �ǉ�����f�[�^
		template<typename T, typename = std::enable_if_t<is_component_data<T>>>
		void setComponentData(const Entity& entity, const T& data)
		{
			m_pWorld->m_ChunkList[entity.m_chunkIndex].setComponentData(entity, data);
		}

		/// @brief �w�肵���A�[�L�^�C�v���܂܂�邷�ׂẴ`�����N���擾
		/// @param archetype �A�[�L�^�C�v
		/// @return �`�����N�̃��X�g
		[[nodiscard]] std::vector<Chunk*> getChunkList(const Archetype& archetype) const;

		/// @brief �w�肵���^(��)���܂܂�邷�ׂẴ`�����N���擾
		/// @tparam ...Args �R���|�[�l���g�f�[�^�^(��)
		/// @return �`�����N�̃��X�g
		template<typename ...Args>
		[[nodiscard]] std::vector<Chunk*> getChunkList() const
		{
			constexpr auto archetype = Archetype::create<Args...>();
			return getChunkList(archetype);
		}

		/// @brief �w�肵���A�[�L�^�C�v�̃`�����N�𐶐��܂��͎擾
		/// @param archetype �A�[�L�^�C�v
		/// @return �`�����N�̃C���f�b�N�X
		[[nodiscard]] std::uint32_t getAndCreateChunkIndex(const Archetype& archetype) const;

		/// @brief �w�肵���^(��)�̃`�����N�𐶐��܂��͎擾
		/// @tparam ...Args �R���|�[�l���g�f�[�^�^(��)
		/// @return �`�����N�̃C���f�b�N�X
		template<typename ...Args>
		[[nodiscard]] std::uint32_t getAndCreateChunkIndex() const
		{
			constexpr auto archetype = Archetype::create<Args...>();
			return getAndCreateChunkIndex(archetype);
		}

	private:
		/// @brief �������郏�[���h
		World* m_pWorld = nullptr;
	};

}