/*****************************************************************//**
 * \file   GameObjectManager.h
 * \brief  ゲームオブジェクトマネージャー
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
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit GameObjectManager(World* pWorld) : m_pWorld(pWorld)
		{
		}

		/// @brief デストラクタ
		~GameObjectManager() = default;

		/// @brief ゲームオブジェクトの生成
		/// @param archetype アーキタイプ
		/// @return 生成したゲームオブジェクトID
		[[nodiscard]] GameObjectID createGameObject(std::string_view name, const Archetype& archetype);

		/// @brief ゲームオブジェクトの生成
		/// @tparam コンポーネントデータ
		/// @return 生成したゲームオブジェクトID
		template<typename ...Args>
		[[nodiscard]] GameObjectID createGameObject(std::string_view name)
		{
			constexpr auto archetype = Archetype::create<Args...>();
			return createGameObject(name, archetype);
		}

		/// @brief ゲームオブジェクトの削除
		/// @param gameObjectID 削除するゲームオブジェクトID
		void destroyGameObject(const GameObjectID& gameObjectID);

		/// @brief 生成リストの実行
		void StartUp();

		/// @brief 削除リストの実行
		void CleanUp();

		/// @brief コンポーネントデータの追加
		/// @tparam T コンポーネントデータ型
		/// @param gameObjectID 対象のゲームオブジェクト
		/// @param data 追加するデータ
		template<typename T>
		void addComponentData(const GameObjectID& gameObjectID, const T& data)
		{
			const auto& itr = m_game0bjectMap.find(gameObjectID);
			if (m_game0bjectMap.end() == itr) return;

			m_pWorld->getEntityManager()->addComponentData<T>(itr->second->m_entity, data);
		}

		/// @brief コンポーネントデータを設定
		/// @tparam T コンポーネントデータ型
		/// @param gameObjectID 対象のゲームオブジェクト
		/// @param data 追加するデータ
		template<typename T, typename = std::enable_if_t<is_component_data<T>>>
		void setComponentData(const GameObjectID& gameObjectID, const T& data)
		{
			const auto& itr = m_game0bjectMap.find(gameObjectID);
			if (m_game0bjectMap.end() == itr) return;

			m_pWorld->getEntityManager()->setComponentData<T>(itr->second->m_entity, data);
		}

		/// @brief コンポーネントデータを設定
		/// @tparam T コンポーネントデータ型
		/// @param gameObjectID 対象のゲームオブジェクト
		/// @param data 追加するデータ
		template<typename T, typename = std::enable_if_t<is_component_data<T>>>
		[[nodiscard]] T* getComponentData(const GameObjectID& gameObjectID)
		{
			const auto& itr = m_game0bjectMap.find(gameObjectID);
			if (m_game0bjectMap.end() == itr) return nullptr;

			return m_pWorld->getEntityManager()->getComponentData<T>(itr->second->m_entity);
		}

	public:
		/// @brief ゲームオブジェクトの取得
		/// @param id ゲームオブジェクトID
		/// @return ポインタ
		GameObject* getGameObject(const GameObjectID& id) {
			auto itr = m_game0bjectMap.find(id);
			if (m_game0bjectMap.end() == itr) return nullptr;
			return itr->second.get();
		}

		/// @brief ゲームオブジェクトマップの取得
		/// @return マップ
		std::unordered_map<GameObjectID, std::unique_ptr<GameObject>>& 
			getGameObjectMap() { return m_game0bjectMap; }

		std::vector<GameObjectID>& getRootList() { return m_rootList; }

		/// @brief 親の設定
		/// @param gameObjectID 自身
		/// @param parentID 親
		void SetParent(const GameObjectID& gameObjectID, const GameObjectID& parentID);
		/// @brief 親の取得
		/// @param gameObjectID 自身
		/// @retrun 親ID
		GameObjectID GetParent(const GameObjectID& gameObjectID);
		/// @brief 親子関係の削除
		/// @param gameObjectID 自身
		void ResetParent(const GameObjectID& gameObjectID);

		/// @brief 子の追加
		/// @param gameObjectID 自身
		/// @param childID 子
		void AddChild(const GameObjectID& gameObjectID, const GameObjectID& childID);
		/// @brief 子リストの取得
		/// @param gameObjectID 自身
		/// @return 子リスト
		const std::vector<GameObjectID>& GetChilds(const GameObjectID& gameObjectID);
		/// @brief 子の削除
		/// @param gameObjectID 自身 
		/// @param childID 子
		void RemoveChild(const GameObjectID& gameObjectID, const GameObjectID& childID);

	private:
		
		void cleanUpGameObject(const GameObjectID& gameObjectID);

		/// @brief ルートリストのソート
		void sortRootList() { std::sort(m_rootList.begin(), m_rootList.end(), GameObject::UpOrder); }
		/// @brief 
		std::vector<GameObjectID>::iterator findRootList(GameObjectID id) {
			return std::lower_bound(m_rootList.begin(), m_rootList.end(), id);
		}

		/// @brief 親を辿って一致したIDを返す(一致しない場合はNONE_ID)
		GameObjectID searchParent(GameObjectID parentID, GameObjectID searchID) {
			if (searchID == m_game0bjectMap[parentID]->GetParentID()
				|| m_game0bjectMap[parentID]->GetParentID() == NONE_GAME_OBJECT_ID)
			{
				return m_game0bjectMap[parentID]->GetParentID();
			}
			searchParent(m_game0bjectMap[parentID]->GetParentID(), searchID);
		}

	private:
		/// @brief ワールド
		World* m_pWorld;

		/// @brief ゲームオブジェクトマップ
		std::unordered_map<GameObjectID, std::unique_ptr<GameObject>> m_game0bjectMap;

		/// @brief 生成ゲームオブジェクトリスト
		std::list<GameObjectID> m_createList;

		/// @brief 削除ゲームオブジェクトリスト
		std::list<GameObjectID> m_destroyList;

		/// @brief 親子関係のルートオブジェクトリスト(二分探索)
		std::vector<GameObjectID> m_rootList;
	};
}
