/*****************************************************************//**
 * \file   GameObject.h
 * \brief  ゲームオブジェクト
 * 
 * \author USAMI KOSHI
 * \date   2021/07/13
 *********************************************************************/
#pragma once

#include "IComponentData.h"
#include <vector>
#include <algorithm>


namespace ecs {

	/// @brief ゲームオブジェクトID
	using GameObjectID = std::uint32_t;
	/// @brief 存在しないゲームオブジェクトID
	constexpr GameObjectID NONE_GAME_OBJECT_ID = std::numeric_limits<GameObjectID>::max();

	/// @brief ECSゲームオブジェクトデータ
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

	/// @brief ゲームオブジェクト
	/// @detail エンティティを扱いやすくするラッパークラス
	class GameObject : public Entity
	{
		friend class GameObjectManager;
	public:
		/// @brief コンストラクタ
		/// @param id ゲームオブジェクトID
		/// @param entity エンティティ
		explicit GameObject(const GameObjectID& id, std::string_view name, const Entity& entity) :
			m_id(id), m_name(name), m_parentID(NONE_GAME_OBJECT_ID), Entity(entity)
		{
		}

		/// @brief デストラクタ
		~GameObject() = default;

		/// @brief IDの取得
		/// @return ゲームオブジェクトID
		GameObjectID getID() { return m_id; }

		/// @brief 名前の取得
		/// @return 名前
		std::string_view getName() { return m_name; }

		/// @brief 子の数を取得
		/// @return 子の数
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

		/// @brief 昇順
		static bool UpOrder(const GameObjectID& lhs, const GameObjectID& rhs) {
			return lhs < rhs;
		}

		/// @brief 降順
		static bool DownOrder(const GameObjectID& lhs, const GameObjectID& rhs) {
			return lhs > rhs;
		}
	private:
		/// @brief ゲームオブジェクトID
		GameObjectID	m_id;
		/// @brief 名前
		std::string		m_name;

		/// @brief 親のID
		GameObjectID	m_parentID;
		/// @brief 子のIDリスト
		std::vector<GameObjectID> m_childsID;
	};
}