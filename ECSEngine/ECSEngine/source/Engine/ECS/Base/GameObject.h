/*****************************************************************//**
 * \file   GameObject.h
 * \brief  ゲームオブジェクト
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

	/// @brief ゲームオブジェクトID
	using GameObjectID = InstanceID;
	/// @brief 存在しないゲームオブジェクトID
	constexpr GameObjectID NONE_GAME_OBJECT_ID = std::numeric_limits<GameObjectID>::max();


	class GameObjectManager;

	/// @brief ゲームオブジェクト
	/// @detail エンティティを扱いやすくするラッパークラス
	class GameObject : public Object
	{
		friend class GameObjectManager;
		DECLARE_TYPE_ID(GameObject);
	public:
		/// @brief コンストラクタ
		/// @param id ゲームオブジェクトID
		/// @param entity エンティティ
		explicit GameObject(const GameObjectID& id, std::string_view name, const Entity& entity) :
			Object(id, name), m_parentID(NONE_GAME_OBJECT_ID), m_entity(entity)
		{
		}
		explicit GameObject() :
			Object(-1, "none"), m_parentID(NONE_GAME_OBJECT_ID), m_entity(-1,-1)
		{
		}

		/// @brief デストラクタ
		~GameObject() = default;


		/// @brief 子の数を取得
		/// @return 子の数
		std::size_t getChildCount() { return m_childsID.size(); }

		/// @brief イベントの格納
		/// @param  イベント関数(name:イベント名, value:値)
		void registerEvent(std::function<void(std::string_view name, void* value)> event);

		/// @brief イベントの呼び出し
		/// @param name イベント名
		/// @param value 値
		void invokeEvent(std::string_view name, void* value);

		/// @brief シリアライズ化
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

		/// @brief 昇順
		static bool UpOrder(const GameObjectID& lhs, const GameObjectID& rhs) {
			return lhs < rhs;
		}

		/// @brief 降順
		static bool DownOrder(const GameObjectID& lhs, const GameObjectID& rhs) {
			return lhs > rhs;
		}

	public:
		/// @brief エンティティ
		Entity m_entity;

	private:
		/// @brief 親のID
		GameObjectID	m_parentID;
		/// @brief 子のIDリスト
		std::vector<GameObjectID> m_childsID;

		/// @brief イベントリスト
		std::list<std::function<void(std::string_view name, void* value)>>	m_eventList;
	};
}