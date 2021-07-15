/*****************************************************************//**
 * \file   GameObject.h
 * \brief  �Q�[���I�u�W�F�N�g
 * 
 * \author USAMI KOSHI
 * \date   2021/07/13
 *********************************************************************/
#pragma once

#include "IComponentData.h"
#include <vector>
#include <algorithm>


namespace ecs {

	/// @brief �Q�[���I�u�W�F�N�gID
	using GameObjectID = std::uint32_t;
	/// @brief ���݂��Ȃ��Q�[���I�u�W�F�N�gID
	constexpr GameObjectID NONE_GAME_OBJECT_ID = std::numeric_limits<GameObjectID>::max();

	/// @brief ECS�Q�[���I�u�W�F�N�g�f�[�^
	struct GameObjectData : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(GameObjectData);
		GameObjectID id;
		GameObjectData(const GameObjectID& id):
			id(id)
		{
		}
	};

	class GameObjectManager;

	/// @brief �Q�[���I�u�W�F�N�g
	/// @detail �G���e�B�e�B�������₷�����郉�b�p�[�N���X
	class GameObject : public Entity
	{
		friend class GameObjectManager;
	public:
		/// @brief �R���X�g���N�^
		/// @param id �Q�[���I�u�W�F�N�gID
		/// @param entity �G���e�B�e�B
		explicit GameObject(const GameObjectID& id, std::string_view name, const Entity& entity) :
			m_id(id), m_name(name), m_parentID(NONE_GAME_OBJECT_ID), Entity(entity)
		{
		}

		/// @brief �f�X�g���N�^
		~GameObject() = default;

		/// @brief ID�̎擾
		/// @return �Q�[���I�u�W�F�N�gID
		GameObjectID getID() { return m_id; }

		/// @brief ���O�̎擾
		/// @return ���O
		std::string_view getName() { return m_name; }

		/// @brief �q�̐����擾
		/// @return �q�̐�
		std::size_t getChildCount() { return m_childsID.size(); }

	protected:


		void SetParentID(const GameObjectID& parentID) { m_parentID = parentID; }
		GameObjectID GetParentID() { return m_parentID; }
		void ResetParentID() { m_parentID = NONE_GAME_OBJECT_ID; }

		void AddChildID(const GameObjectID& childID) {
			m_childsID.push_back(childID);
			std::sort(m_childsID.begin(), m_childsID.end(), UpOrder);
		}
		const std::vector<GameObjectID>& GetChildsID() { return m_childsID; }
		void RemoveChildID(const GameObjectID& childID) {
			const auto& itr = std::lower_bound(m_childsID.begin(), m_childsID.end(), childID);
			if (*itr != childID) return;
			m_childsID.erase(itr);
		}

		/// @brief ����
		static bool UpOrder(const GameObjectID& lhs, const GameObjectID& rhs) {
			return lhs < rhs;
		}

		/// @brief �~��
		static bool DownOrder(const GameObjectID& lhs, const GameObjectID& rhs) {
			return lhs > rhs;
		}
	private:
		/// @brief �Q�[���I�u�W�F�N�gID
		GameObjectID	m_id;
		/// @brief ���O
		std::string		m_name;

		/// @brief �e��ID
		GameObjectID	m_parentID;
		/// @brief �q��ID���X�g
		std::vector<GameObjectID> m_childsID;
	};
}