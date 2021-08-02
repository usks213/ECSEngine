/*****************************************************************//**
 * \file   GameObject.h
 * \brief  �Q�[���I�u�W�F�N�g
 * 
 * \author USAMI KOSHI
 * \date   2021/07/13
 *********************************************************************/
#pragma once

#include "IComponentData.h"
#include <Engine/Object.h>
#include <vector>
#include <algorithm>
#include <functional>


namespace ecs {

	/// @brief �Q�[���I�u�W�F�N�gID
	using GameObjectID = InstanceID;
	/// @brief ���݂��Ȃ��Q�[���I�u�W�F�N�gID
	constexpr GameObjectID NONE_GAME_OBJECT_ID = std::numeric_limits<GameObjectID>::max();


	class GameObjectManager;

	/// @brief �Q�[���I�u�W�F�N�g
	/// @detail �G���e�B�e�B�������₷�����郉�b�p�[�N���X
	class GameObject : public Object
	{
		friend class GameObjectManager;
		DECLARE_TYPE_ID(GameObject);
	public:
		/// @brief �R���X�g���N�^
		/// @param id �Q�[���I�u�W�F�N�gID
		/// @param entity �G���e�B�e�B
		explicit GameObject(const GameObjectID& id, std::string_view name, const Entity& entity) :
			Object(id, name), m_parentID(NONE_GAME_OBJECT_ID), m_entity(entity)
		{
		}
		explicit GameObject() :
			Object(-1, "none"), m_parentID(NONE_GAME_OBJECT_ID), m_entity(-1,-1)
		{
		}

		/// @brief �f�X�g���N�^
		~GameObject() = default;


		/// @brief �q�̐����擾
		/// @return �q�̐�
		std::size_t getChildCount() { return m_childsID.size(); }

		/// @brief �C�x���g�̊i�[
		/// @param  �C�x���g�֐�(name:�C�x���g��, value:�l)
		void registerEvent(std::function<void(std::string_view name, void* value)> event);

		/// @brief �C�x���g�̌Ăяo��
		/// @param name �C�x���g��
		/// @param value �l
		void invokeEvent(std::string_view name, void* value);

		/// @brief �V���A���C�Y��
		template<class T>
		void serialize(T& archive)
		{
			Object::serialize(archive);
			archive(
				CEREAL_NVP(m_entity),
				CEREAL_NVP(m_parentID),
				CEREAL_NVP(m_childsID)
			);
		}

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

	public:
		/// @brief �G���e�B�e�B
		Entity m_entity;

	private:
		/// @brief �e��ID
		GameObjectID	m_parentID;
		/// @brief �q��ID���X�g
		std::vector<GameObjectID> m_childsID;

		/// @brief �C�x���g���X�g
		std::list<std::function<void(std::string_view name, void* value)>>	m_eventList;
	};
}