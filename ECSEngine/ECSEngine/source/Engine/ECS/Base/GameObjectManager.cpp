/*****************************************************************//**
 * \file   GameObjectManager.h
 * \brief  ゲームオブジェクトマネージャー
 *
 * \author USAMI KOSHI
 * \date   2021/07/13
 *********************************************************************/

#include "GameObjectManager.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>

using namespace ecs;


/// @brief ゲームオブジェクトの生成
/// @param archetype アーキタイプ
/// @return 生成したゲームオブジェクトID
GameObjectID GameObjectManager::createGameObject(std::string_view name, const Archetype& archetype)
{
	auto entity = m_pWorld->getEntityManager()->createEntity(archetype);
	GameObjectID id = NONE_GAME_OBJECT_ID;

	while (true)
	{
		id =  static_cast<GameObjectID>(rand());
		const auto& itr = m_game0bjectMap.find(id);
		if (m_game0bjectMap.end() == itr) break;
	}

	m_game0bjectMap[id] = std::make_unique<GameObject>(id, name, entity);
	m_rootList.push_back(id);
	sortRootList();

	return id;
}

/// @brief ゲームオブジェクトの削除
/// @param gameObjectID 削除するゲームオブジェクトID
void GameObjectManager::destroyGameObject(const GameObjectID& gameObjectID)
{

}


/// @brief 親の設定
/// @param gameObjectID 自身
/// @param parentID 親
void GameObjectManager::SetParent(const GameObjectID& gameObjectID, const GameObjectID& parentID)
{
	// 自身の子に親を設定する場合はできない
	if (searchParent(parentID, gameObjectID) == gameObjectID)
	{
		return;
	}
	// 同じ親だったら更新しない
	if (GetParent(gameObjectID) == parentID)
	{
		return;
	}

	// 自身の親
	auto parent = m_game0bjectMap[gameObjectID]->GetParentID();
	auto transform = getComponentData<Transform>(gameObjectID);
	auto parentTrans = getComponentData<Transform>(parentID);

	auto itr = findRootList(gameObjectID);
	if (itr != m_rootList.end() && *itr == gameObjectID)
	{
		//--- 自身がルートだった場合は削除
		m_rootList.erase(itr);
	}
	else
	{
		//--- 違う場合は親の子から自身を削除
		// 親の子から自身を削除
		m_game0bjectMap[parent]->RemoveChildID(gameObjectID);
		// 親を初期化
		m_game0bjectMap[gameObjectID]->ResetParentID();
		// 行列再計算
		auto global = transform->globalMatrix;
		// 位置
		transform->translation = global.Translation();
		// 回転
		Matrix invSca = Matrix::CreateScale(transform->globalScale);
		invSca = invSca.Invert();
		transform->rotation = Quaternion::CreateFromRotationMatrix(invSca * global);
		// 拡縮
		transform->scale = transform->globalScale;
		// マトリックス更新
		// 拡縮
		transform->localMatrix = Matrix::CreateScale(transform->scale);
		// 回転
		transform->localMatrix *= Matrix::CreateFromQuaternion(transform->rotation);
		// 移動
		transform->localMatrix *= Matrix::CreateTranslation(transform->translation);
	}

	// 自身に親を設定
	m_game0bjectMap[gameObjectID]->SetParentID(parentID);

	// 親の子に自身を設定
	m_game0bjectMap[parentID]->AddChildID(gameObjectID);


	// 親の逆行列を反映
	auto invParent = (parentTrans->globalMatrix).Invert();
	auto invParentScale = Vector3(1,1,1) / parentTrans->globalScale;
	auto localMatrix = transform->localMatrix * invParent;
	// 位置
	transform->translation = localMatrix.Translation();
	// 回転
	Matrix invSca = Matrix::CreateScale(transform->scale * invParentScale);
	invSca = invSca.Invert();
	transform->rotation = Quaternion::CreateFromRotationMatrix(invSca * localMatrix);
	// 拡縮
	transform->scale = transform->scale * invParentScale;
	// マトリックス更新
	// 拡縮
	transform->localMatrix = Matrix::CreateScale(transform->scale);
	// 回転
	transform->localMatrix *= Matrix::CreateFromQuaternion(transform->rotation);
	// 移動
	transform->localMatrix *= Matrix::CreateTranslation(transform->translation);
}
/// @brief 親の取得
/// @param gameObjectID 自身
/// @retrun 親ID
GameObjectID GameObjectManager::GetParent(const GameObjectID& gameObjectID)
{
	return m_game0bjectMap[gameObjectID]->GetParentID();
}
/// @brief 親子関係の削除
/// @param gameObjectID 自身
void GameObjectManager::ResetParent(const GameObjectID& gameObjectID)
{
	// 親の取得
	auto parent = m_game0bjectMap[gameObjectID]->GetParentID();
	if (parent == NONE_GAME_OBJECT_ID) return;

	// 行列再計算
	auto transform = getComponentData<Transform>(gameObjectID);
	auto global = transform->globalMatrix;
	// 位置
	transform->translation = global.Translation();
	// 回転
	Matrix invSca = Matrix::CreateScale(transform->globalScale);
	invSca = invSca.Invert();
	transform->rotation = Quaternion::CreateFromRotationMatrix(invSca * global);
	// 拡縮
	transform->scale = transform->globalScale;
	// マトリックス更新
	// 拡縮
	transform->localMatrix = Matrix::CreateScale(transform->scale);
	// 回転
	transform->localMatrix *= Matrix::CreateFromQuaternion(transform->rotation);
	// 移動
	transform->localMatrix *= Matrix::CreateTranslation(transform->translation);

	// 親の子から自身を削除
	m_game0bjectMap[parent]->RemoveChildID(gameObjectID);
	// 親を初期化
	m_game0bjectMap[gameObjectID]->ResetParentID();
	m_rootList.push_back(gameObjectID);
	sortRootList();
}

/// @brief 子の追加
/// @param gameObjectID 自身
/// @param childID 子
void GameObjectManager::AddChild(const GameObjectID& gameObjectID, const GameObjectID& childID)
{
	SetParent(childID, gameObjectID);
}
/// @brief 子リストの取得
/// @param gameObjectID 自身
/// @return 子リスト
const std::vector<GameObjectID>& GameObjectManager::GetChilds(const GameObjectID& gameObjectID)
{
	return m_game0bjectMap[gameObjectID]->GetChildsID();
}
/// @brief 子の削除
/// @param gameObjectID 自身 
/// @param childID 子
void GameObjectManager::RemoveChild(const GameObjectID& gameObjectID, const GameObjectID& childID)
{
	m_game0bjectMap[gameObjectID]->RemoveChildID(childID);
	m_game0bjectMap[childID]->ResetParentID();
	m_rootList.push_back(childID);
	sortRootList();
}
