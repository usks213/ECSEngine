/*****************************************************************//**
 * \file   GameObjectManager.h
 * \brief  �Q�[���I�u�W�F�N�g�}�l�[�W���[
 * 
 * \author USAMI KOSHI
 * \date   2021/07/13
 *********************************************************************/
#pragma once

#include "GameObject.h"
#include "EntityManager.h"
#include <unordered_map>


namespace ecs {

	class GameObjectManager
	{
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit GameObjectManager(World* pWorld) : m_pWorld(pWorld)
		{
		}

		/// @brief �f�X�g���N�^
		~GameObjectManager() = default;

		/// @brief �Q�[���I�u�W�F�N�g�̐���
		/// @param archetype �A�[�L�^�C�v
		/// @return ���������Q�[���I�u�W�F�N�gID
		[[nodiscard]] GameObjectID createGameObject(std::string_view name, const Archetype& archetype);

		/// @brief �Q�[���I�u�W�F�N�g�̐���
		/// @tparam �R���|�[�l���g�f�[�^
		/// @return ���������Q�[���I�u�W�F�N�gID
		template<typename ...Args>
		[[nodiscard]] GameObjectID createGameObject(std::string_view name)
		{
			constexpr auto archetype = Archetype::create<Args...>();
			return createGameObject(name, archetype);
		}

		/// @brief �Q�[���I�u�W�F�N�g�̍폜
		/// @param gameObjectID �폜����Q�[���I�u�W�F�N�gID
		void destroyGameObject(const GameObjectID& gameObjectID);

		/// @brief �������X�g�̎��s
		void StartUp();

		/// @brief �폜���X�g�̎��s
		void CleanUp();

		/// @brief �R���|�[�l���g�f�[�^�̒ǉ�
		/// @tparam T �R���|�[�l���g�f�[�^�^
		/// @param gameObjectID �Ώۂ̃Q�[���I�u�W�F�N�g
		/// @param data �ǉ�����f�[�^
		template<typename T>
		void addComponentData(const GameObjectID& gameObjectID, const T& data)
		{
			const auto& itr = m_game0bjectMap.find(gameObjectID);
			if (m_game0bjectMap.end() == itr) return;

			m_pWorld->getEntityManager()->addComponentData<T>(itr->second->m_entity, data);
		}

		/// @brief �R���|�[�l���g�f�[�^��ݒ�
		/// @tparam T �R���|�[�l���g�f�[�^�^
		/// @param gameObjectID �Ώۂ̃Q�[���I�u�W�F�N�g
		/// @param data �ǉ�����f�[�^
		template<typename T, typename = std::enable_if_t<is_component_data<T>>>
		void setComponentData(const GameObjectID& gameObjectID, const T& data)
		{
			const auto& itr = m_game0bjectMap.find(gameObjectID);
			if (m_game0bjectMap.end() == itr) return;

			m_pWorld->getEntityManager()->setComponentData<T>(itr->second->m_entity, data);
		}

		/// @brief �R���|�[�l���g�f�[�^��ݒ�
		/// @tparam T �R���|�[�l���g�f�[�^�^
		/// @param gameObjectID �Ώۂ̃Q�[���I�u�W�F�N�g
		/// @param data �ǉ�����f�[�^
		template<typename T, typename = std::enable_if_t<is_component_data<T>>>
		[[nodiscard]] T* getComponentData(const GameObjectID& gameObjectID)
		{
			const auto& itr = m_game0bjectMap.find(gameObjectID);
			if (m_game0bjectMap.end() == itr) return nullptr;

			return m_pWorld->getEntityManager()->getComponentData<T>(itr->second->m_entity);
		}

	public:
		/// @brief �Q�[���I�u�W�F�N�g�̎擾
		/// @param id �Q�[���I�u�W�F�N�gID
		/// @return �|�C���^
		GameObject* getGameObject(const GameObjectID& id) {
			auto itr = m_game0bjectMap.find(id);
			if (m_game0bjectMap.end() == itr) return nullptr;
			return itr->second.get();
		}

		/// @brief �Q�[���I�u�W�F�N�g�}�b�v�̎擾
		/// @return �}�b�v
		std::unordered_map<GameObjectID, std::unique_ptr<GameObject>>& 
			getGameObjectMap() { return m_game0bjectMap; }

		std::vector<GameObjectID>& getRootList() { return m_rootList; }

		/// @brief �e�̐ݒ�
		/// @param gameObjectID ���g
		/// @param parentID �e
		void SetParent(const GameObjectID& gameObjectID, const GameObjectID& parentID);
		/// @brief �e�̎擾
		/// @param gameObjectID ���g
		/// @retrun �eID
		GameObjectID GetParent(const GameObjectID& gameObjectID);
		/// @brief �e�q�֌W�̍폜
		/// @param gameObjectID ���g
		void ResetParent(const GameObjectID& gameObjectID);

		/// @brief �q�̒ǉ�
		/// @param gameObjectID ���g
		/// @param childID �q
		void AddChild(const GameObjectID& gameObjectID, const GameObjectID& childID);
		/// @brief �q���X�g�̎擾
		/// @param gameObjectID ���g
		/// @return �q���X�g
		const std::vector<GameObjectID>& GetChilds(const GameObjectID& gameObjectID);
		/// @brief �q�̍폜
		/// @param gameObjectID ���g 
		/// @param childID �q
		void RemoveChild(const GameObjectID& gameObjectID, const GameObjectID& childID);

	private:
		
		void cleanUpGameObject(const GameObjectID& gameObjectID);

		/// @brief ���[�g���X�g�̃\�[�g
		void sortRootList() { std::sort(m_rootList.begin(), m_rootList.end(), GameObject::UpOrder); }
		/// @brief 
		std::vector<GameObjectID>::iterator findRootList(GameObjectID id) {
			return std::lower_bound(m_rootList.begin(), m_rootList.end(), id);
		}

		/// @brief �e��H���Ĉ�v����ID��Ԃ�(��v���Ȃ��ꍇ��NONE_ID)
		GameObjectID searchParent(GameObjectID parentID, GameObjectID searchID) {
			if (searchID == m_game0bjectMap[parentID]->GetParentID()
				|| m_game0bjectMap[parentID]->GetParentID() == NONE_GAME_OBJECT_ID)
			{
				return m_game0bjectMap[parentID]->GetParentID();
			}
			searchParent(m_game0bjectMap[parentID]->GetParentID(), searchID);
		}

	private:
		/// @brief ���[���h
		World* m_pWorld;

		/// @brief �Q�[���I�u�W�F�N�g�}�b�v
		std::unordered_map<GameObjectID, std::unique_ptr<GameObject>> m_game0bjectMap;

		/// @brief �����Q�[���I�u�W�F�N�g���X�g
		std::list<GameObjectID> m_createList;

		/// @brief �폜�Q�[���I�u�W�F�N�g���X�g
		std::list<GameObjectID> m_destroyList;

		/// @brief �e�q�֌W�̃��[�g�I�u�W�F�N�g���X�g(�񕪒T��)
		std::vector<GameObjectID> m_rootList;
	};
}
