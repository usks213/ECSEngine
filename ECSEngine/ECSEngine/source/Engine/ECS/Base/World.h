/*****************************************************************//**
 * \file   World.h
 * \brief  ���[���h
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include"Chunk.h"


namespace ecs {
	class EntityManager;
	class SystemBase;

	/// @class World
	/// @brief ���[���h
	class World
	{
		friend class EntityManager;
	public:
		/// @brief �R���X�g���N�^
		World();
		/// @brief �f�X�g���N�^
		~World();

		/// @brief �G���e�B�e�B�}�l�[�W���[�̎擾
		/// @return �G���e�B�e�B�}�l�[�W���[
		[[nodiscard]] EntityManager* getEntityManager() const;

		/// @brief �V�X�e���̍X�V
		void update();

		/// @brief  �V�X�e���̒ǉ�
		/// @tparam �V�X�e���̌^
		template<class T>
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
	};
}