/*****************************************************************//**
 * \file   SystemBase.h
 * \brief  �V�X�e���x�[�X
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "EntityManager.h"
#include "GameObjectManager.h"


namespace ecs {

	/// @class SystemBase
	/// @brief �V�X�e���x�[�X
	class SystemBase
	{
	public:
		/// @brief �R�s�[�R���X�g���N�^�폜
		SystemBase(SystemBase&) = delete;
		/// @brief ���[�u�R���X�g���N�^�폜
		SystemBase(SystemBase&&) = delete;

		/// @brief �R���X�g���N�^
		explicit SystemBase(World* pWorld);
		/// @brief �f�X�g���N�^
		virtual ~SystemBase() = default;

		/// @brief ������
		virtual void onCreate();
		/// @brief �폜��
		virtual void onDestroy();
		/// @brief �X�V
		virtual void onUpdate() = 0;

		/// @brief �X�V���ԍ��擾
		[[nodiscard]] int getExecutionOrder() const;

	protected:

		/// @brief �G���e�B�e�B�}�l�[�W���[�擾
		/// @return �G���e�B�e�B�}�l�[�W���[
		[[nodiscard]] EntityManager* getEntityManager() const;

		/// @brief �Q�[���I�u�W�F�N�g�}�l�[�W���[�擾
		/// @return �Q�[���I�u�W�F�N�g�}�l�[�W���[
		[[nodiscard]] GameObjectManager* getGameObjectManager() const;

		/// @brief �X�V���ݒ�
		/// @param �X�V�ԍ�
		void setExecutionOrder(const int executionOrder);

		/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
		/// @tparam T1 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam Func �֐��I�u�W�F�N�g
		/// @param func �����̊֐��I�u�W�F�N�g
		template<class T1, typename Func>
		void foreach(Func&& func);

		/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
		/// @tparam T1 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam T2 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam Func �֐��I�u�W�F�N�g
		/// @param func �����̊֐��I�u�W�F�N�g
		template<class T1, class T2, typename Func>
		void foreach(Func&& func);

		/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
		/// @tparam T1 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam T2 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam T3 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam Func �֐��I�u�W�F�N�g
		/// @param func �����̊֐��I�u�W�F�N�g
		template<class T1, class T2, class T3, typename Func>
		void foreach(Func&& func);

		/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
		/// @tparam T1 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam T2 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam T3 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam T4 �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @tparam Func �֐��I�u�W�F�N�g
		/// @param func �����̊֐��I�u�W�F�N�g
		template<class T1, class T2, class T3, class T4, typename Func>
		void foreach(Func&& func);

	protected:

		/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
		/// @tparam Func �֐��I�u�W�F�N�g
		/// @tparam ...Args �t�F�b�`����R���|�[�l���g�f�[�^�^
		/// @param pChunk �`�����N
		/// @param func �����̊֐��I�u�W�F�N�g
		/// @param ...args �t�F�b�`����R���|�[�l���g�f�[�^
		template<typename Func, class... Args>
		static void foreachImplements(Chunk* pChunk, Func&& func, Args... args);

		/// @brief �������郏�[���h
		World* m_pWorld = nullptr;
		/// @brief
		int m_nExecutionOrder = 0;
	};

	/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
	/// @tparam T1 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam Func �֐��I�u�W�F�N�g
	/// @param func �����̊֐��I�u�W�F�N�g
	template<class T1, typename Func>
	void SystemBase::foreach(Func&& func)
	{
		auto pChunkList = getEntityManager()->getChunkList<T1>();
		for (auto&& pChunk : pChunkList)
		{
			auto arg1 = pChunk->template getComponentArray<T1>();
			foreachImplements(pChunk, func, arg1);
		}
	}

	/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
	/// @tparam T1 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam T2 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam Func �֐��I�u�W�F�N�g
	/// @param func �����̊֐��I�u�W�F�N�g
	template<class T1, class T2, typename Func>
	void SystemBase::foreach(Func&& func)
	{
		auto pChunkList = getEntityManager()->getChunkList<T1, T2>();
		for (auto&& pChunk : pChunkList)
		{
			auto arg1 = pChunk->template getComponentArray<T1>();
			auto arg2 = pChunk->template getComponentArray<T2>();
			foreachImplements(pChunk, func, arg1, arg2);
		}
	}

	/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
	/// @tparam T1 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam T2 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam T3 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam Func �֐��I�u�W�F�N�g
	/// @param func �����̊֐��I�u�W�F�N�g
	template<class T1, class T2, class T3, typename Func>
	void SystemBase::foreach(Func&& func)
	{
		auto pChunkList = getEntityManager()->getChunkList<T1, T2, T3>();
		for (auto&& pChunk : pChunkList)
		{
			auto arg1 = pChunk->template getComponentArray<T1>();
			auto arg2 = pChunk->template getComponentArray<T2>();
			auto arg3 = pChunk->template getComponentArray<T3>();
			foreachImplements(pChunk, func, arg1, arg2, arg3);
		}
	}

	/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
	/// @tparam T1 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam T2 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam T3 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam T4 �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @tparam Func �֐��I�u�W�F�N�g
	/// @param func �����̊֐��I�u�W�F�N�g
	template<class T1, class T2, class T3, class T4, typename Func>
	void SystemBase::foreach(Func&& func)
	{
		auto pChunkList = getEntityManager()->getChunkList<T1, T2, T3, T4>();
		for (auto&& pChunk : pChunkList)
		{
			auto arg1 = pChunk->template getComponentArray<T1>();
			auto arg2 = pChunk->template getComponentArray<T2>();
			auto arg3 = pChunk->template getComponentArray<T3>();
			auto arg4 = pChunk->template getComponentArray<T4>();
			foreachImplements(pChunk, func, arg1, arg2, arg3, arg4);
		}
	}

	/// @brief �w�肵���R���|�[�l���g�f�[�^���t�F�b�`���ď�������
	/// @tparam Func �֐��I�u�W�F�N�g
	/// @tparam ...Args �t�F�b�`����R���|�[�l���g�f�[�^�^
	/// @param pChunk �`�����N
	/// @param func �����̊֐��I�u�W�F�N�g
	/// @param ...args �t�F�b�`����R���|�[�l���g�f�[�^
	template<typename Func, class... Args>
	void SystemBase::foreachImplements(Chunk* pChunk, Func&& func, Args... args)
	{
		for (std::uint32_t i = 0; i < pChunk->getSize(); ++i)
		{
			func(args[i]...);
		}
	}
}