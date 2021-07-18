/*****************************************************************//**
 * \file   CollisionSystem.h
 * \brief  当たり判定システム
 *
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/

#include "CollisionSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/PhysicsComponentData.h>
#include <Engine/ECS/System/QuadTreeSystem.h>


using namespace ecs;


 /// @brief 生成時
void CollisionSystem::onCreate()
{

}

/// @brief 削除時
void CollisionSystem::onDestroy()
{

}

/// @brief 更新
void CollisionSystem::onUpdate()
{
	// 四分木システムの取得
	auto* quadTree = m_pWorld->getSystem<QuadTreeSystem>();
	if (quadTree == nullptr) return;

	// 動的オブジェクトと静的オブジェクトを分けて当たり判定

	//--- 動的オブジェクト同士
	// メイン空間の当たり判定
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_dynamicMainList[i]);
		}
	}
	// サブ空間の当たり判定
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_dynamicSubList[i]);
		}
	}

	//--- 動的オブジェクトと静的オブジェクト
	// メイン空間の当たり判定
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_staticMainList[i]);
		}
	}
	// サブ空間の当たり判定
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_staticSubList[i]);
		}
	}
	//--- 静的オブジェクトと動的オブジェクト
		// サブ空間の当たり判定
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_staticMainList[i])
		{
			Collision(entity, quadTree->m_dynamicSubList[i]);
		}
	}
}


void CollisionSystem::Collision(const Entity& main, const std::vector<Entity>& m_subList)
{
	Vector3 min(-0.5f, -0.5f, -0.5f);
	Vector3 max(0.5f, 0.5f, 0.5f);

	// サブループ
	for (const auto& sub : m_subList)
	{
		// 同じだった
		if (main.m_chunkIndex == sub.m_chunkIndex &&
			main.m_index == sub.m_index) continue;

		//--- 当たり判定処理 ---
		auto* mainTrans = getEntityManager()->getComponentData<Transform>(main);
		auto* subTrans = getEntityManager()->getComponentData<Transform>(sub);

		Vector3  min1 = Vector3::Transform(min, mainTrans->globalMatrix);
		Vector3  max1 = Vector3::Transform(max, mainTrans->globalMatrix);
		Vector3  min2 = Vector3::Transform(min, subTrans->globalMatrix);
		Vector3  max2 = Vector3::Transform(max, subTrans->globalMatrix);

		// AABB
		if (min1.x < max2.x && min2.x < max1.x &&
			min1.y < max2.y && min2.y < max1.y &&
			min1.z < max2.z && min2.z < max1.z)
		{
			auto* mainPhysics = getEntityManager()->getComponentData<Physics>(main);
			auto* subPhysics = getEntityManager()->getComponentData<Physics>(sub);
		}
	}
}