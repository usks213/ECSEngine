/*****************************************************************//**
 * \file   World.h
 * \brief  ���[���h
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include"Chunk.h"


class WorldManager;

namespace ecs {
	class EntityManager;
	class SystemBase;

	/// @class World
	/// @brief ���[���h
	class World
	{
		friend class WorldManager;
		friend class EntityManager;
		friend class SystemBase;
	public:
		/// @brief �R���X�g���N�^
		World(WorldManager* pWorldManager);
		/// @brief �f�X�g���N�^
		virtual ~World();

		/// @brief �X�^�[�g
		virtual void Start() = 0;
		/// @brief �G���h
		virtual void End() = 0;

		/// @brief �V�X�e���̍X�V
		void update();

		/// @brief  �V�X�e���̒ǉ�
		/// @tparam �V�X�e���̌^
		template<class T, typename = std::enable_if_t<std::is_base_of_v<SystemBase,T>>>
		void addSystem()
		{
			callDoCreate(m_SystemList.emplace_back(new T(this)).get());
			sortSystem();
		}

		/// @brief ���̃��[���h�̃f�[�^����������
		/// SystemList�͌�������Ȃ�
		/// @param other �������郏�[���h
		void mage(World&& other);

		/// @brief �`�����N���X�g���o�C�i���X�g���[���ɏ����o��
		/// @param output �����o����̃X�g���[��
		void writeBinaryStream(BinaryStream& output);

		/// @brief �`�����N���X�g���o�C�i���X�g���[������ǂݍ���
		/// @param output �ǂݍ��ݐ�̃X�g���[��
		void readBinaryStream(BinaryStream& input);

	public:
		/// @brief �G���e�B�e�B�}�l�[�W���[�̎擾
		/// @return �G���e�B�e�B�}�l�[�W���[
		[[nodiscard]] EntityManager* getEntityManager() const;

		/// @brief ���[���h�}�l�[�W���[�擾
		/// @return ���[���h�}�l�[�W���[
		[[nodiscard]] WorldManager* getWorldManager() const { return m_pWorldManager; }

		/// @brief �`�����N���X�g�̎擾
		/// @return �`�����N���X�g
		std::vector<Chunk>& getChunkList() { return m_ChunkList; }

		/// @brief �V�X�e���V�X�e���̎擾
		/// @return �V�X�e�����X�g
		std::vector<std::unique_ptr<SystemBase>>& getSystemList() { return m_SystemList; }

	private:

		/// @brief �V�X�e���̍X�V������
		void sortSystem();

		/// @brief �V�X�e���������̃R�[���o�b�N
		static void callDoCreate(SystemBase* pSystem);

		/// @brief �`�����N���X�g
		std::vector<Chunk> m_ChunkList;
		/// @brief �V�X�e�����X�g
		std::vector<std::unique_ptr<SystemBase>> m_SystemList;
		/// @brief �G���e�B�e�B�}�l�[�W���[
		std::unique_ptr<EntityManager> m_pEntityManager;
		/// @brief ���[���h�}�l�[�W���[
		WorldManager* m_pWorldManager;
	};
}