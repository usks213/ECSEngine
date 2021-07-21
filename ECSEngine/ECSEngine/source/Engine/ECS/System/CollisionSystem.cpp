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

void CollisionPhysics(Physics& physics1, Physics& physics2, const Vector3& normal, bool static2);
Vector3 LenOBBToPoint(Transform& obb, Vector3& p);

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
			Collision(entity, quadTree->m_dynamicMainList[i],
				false, false);
		}
	}
	// サブ空間の当たり判定
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_dynamicSubList[i],
				false, false);
		}
	}

	//--- 動的オブジェクトと静的オブジェクト
	// メイン空間の当たり判定
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_staticMainList[i],
				false, true);
		}
	}
	// サブ空間の当たり判定
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_staticSubList[i], 
				false, true);
		}
	}
	////--- 静的オブジェクトと動的オブジェクト
	//	// サブ空間の当たり判定
	//for (int i = 0; i < quadTree->getCellNum(); ++i)
	//{
	//	for (auto& entity : quadTree->m_staticMainList[i])
	//	{
	//		Collision(entity, quadTree->m_dynamicSubList[i],
	//			true, false);
	//	}
	//}
}


void CollisionSystem::Collision(const Entity& main, const std::vector<Entity>& m_subList,
	bool mainStatic, bool subStatic)
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
		auto* mainPhysics = getEntityManager()->getComponentData<Physics>(main);
		auto* subPhysics = getEntityManager()->getComponentData<Physics>(sub);
		if (mainPhysics == nullptr || subPhysics == nullptr) continue;

		auto* mainTrans = getEntityManager()->getComponentData<Transform>(main);
		auto* subTrans = getEntityManager()->getComponentData<Transform>(sub);

		Vector3  min1 = Vector3::Transform(min, mainTrans->globalMatrix);
		Vector3  max1 = Vector3::Transform(max, mainTrans->globalMatrix);
		Vector3  min2 = Vector3::Transform(min, subTrans->globalMatrix);
		Vector3  max2 = Vector3::Transform(max, subTrans->globalMatrix);

		// AABB
		//if (min1.x < max2.x && min2.x < max1.x &&
		//	min1.y < max2.y && min2.y < max1.y &&
		//	min1.z < max2.z && min2.z < max1.z)
		{
			// 詳細判定
			if (mainPhysics->colliderType == Physics::ColliderType::AABB &&
				subPhysics->colliderType == Physics::ColliderType::AABB)
			{
				if (mainStatic)
					AABBvsAABB(*subTrans, *subPhysics, *mainTrans, *mainPhysics, mainStatic);
				else
					AABBvsAABB(*mainTrans, *mainPhysics, *subTrans, *subPhysics, subStatic);
			}
			else if (mainPhysics->colliderType == Physics::ColliderType::AABB &&
				subPhysics->colliderType == Physics::ColliderType::Sphere)
			{
				if (mainStatic)
					SphereVsAABB(*subTrans, *subPhysics, *mainTrans, *mainPhysics, mainStatic);
				else
					AABBvsSphere(*mainTrans, *mainPhysics, *subTrans, *subPhysics, subStatic);
			}
			else if (mainPhysics->colliderType == Physics::ColliderType::Sphere &&
				subPhysics->colliderType == Physics::ColliderType::AABB)
			{
				if (mainStatic)
					AABBvsSphere(*subTrans, *subPhysics, *mainTrans, *mainPhysics, mainStatic);
				else
					SphereVsAABB(*mainTrans, *mainPhysics, *subTrans, *subPhysics, subStatic);
			}
			else if (mainPhysics->colliderType == Physics::ColliderType::Sphere &&
				subPhysics->colliderType == Physics::ColliderType::Sphere)
			{
				if (mainStatic)
					SphereVsSphere(*subTrans, *subPhysics, *mainTrans, *mainPhysics, mainStatic);
				else
					SphereVsSphere(*mainTrans, *mainPhysics, *subTrans, *subPhysics, subStatic);
			}
			else if (mainPhysics->colliderType == Physics::ColliderType::OBB &&
				subPhysics->colliderType == Physics::ColliderType::OBB)
			{
				if (mainStatic)
					OBBvsOBB(*subTrans, *subPhysics, *mainTrans, *mainPhysics, mainStatic);
				else
					OBBvsOBB(*mainTrans, *mainPhysics, *subTrans, *subPhysics, subStatic);
			}
			else if (mainPhysics->colliderType == Physics::ColliderType::OBB &&
				subPhysics->colliderType == Physics::ColliderType::Sphere)
			{
				if (mainStatic)
					SphereVsOBB(*subTrans, *subPhysics, *mainTrans, *mainPhysics, mainStatic);
				else
					OBBvsSphere(*mainTrans, *mainPhysics, *subTrans, *subPhysics, subStatic);
			}
			else if (mainPhysics->colliderType == Physics::ColliderType::Sphere &&
				subPhysics->colliderType == Physics::ColliderType::OBB)
			{
				if (mainStatic)
					OBBvsSphere(*subTrans, *subPhysics, *mainTrans, *mainPhysics, mainStatic);
				else
					SphereVsOBB(*mainTrans, *mainPhysics, *subTrans, *subPhysics, subStatic);
			}
		}
	}
}


bool CollisionSystem::AABBvsAABB(Transform& transform1, Physics& physics1,
	Transform& transform2, Physics& physics2, bool static2)
{
	//--- 判定はAABBで取っている

	// トリガー
	if (physics1.trigger || physics2.trigger) return true;


	//--- 詳細判定
	Vector3 min(-0.5f, -0.5f, -0.5f);
	Vector3 max(0.5f, 0.5f, 0.5f);
	// ボックスの最大最小
	Vector3 boxMax1 = Vector3::Transform(max, transform1.globalMatrix);
	Vector3 boxMin1 = Vector3::Transform(min, transform1.globalMatrix);
	Vector3 boxMax2 = Vector3::Transform(max, transform2.globalMatrix);
	Vector3 boxMin2 = Vector3::Transform(min, transform2.globalMatrix);
	// ハーフサイズ
	Vector3 boxSize1 = max * transform1.globalScale;
	Vector3 boxSize2 = max * transform2.globalScale;
	// 親の逆行列
	Matrix parentInv;
	auto parentID = getGameObjectManager()->GetParent(transform1.id);
	if (parentID != NONE_GAME_OBJECT_ID)
	{
		auto* parentTrans = getGameObjectManager()->getComponentData<Transform>(parentID);
		parentInv = parentTrans->globalMatrix.Invert();
	}

	//====================================================================
	// 距離
	Vector3 maxLen = boxMax2 - boxMin1;
	Vector3	minLen = boxMax1 - boxMin2;

	// 各軸距離
	Vector3 len;
	Vector3 pos;
	Vector3 normal;

	// X軸
	if (maxLen.x < minLen.x)
	{
		len.x = maxLen.x;
		pos.x = boxMax2.x + boxSize1.x;
		normal.x = 1.0f;
	}
	else
	{
		len.x = minLen.x;
		pos.x = boxMin2.x - boxSize1.x;
		normal.x = -1.0f;
	}
	// Y軸
	if (maxLen.y < minLen.y)
	{
		len.y = maxLen.y;
		pos.y = boxMax2.y + boxSize1.y;
		normal.y = 1.0f;
	}
	else
	{
		len.y = minLen.y;
		pos.y = boxMin2.y - boxSize1.y;
		normal.y = -1.0f;
	}
	// Z軸
	if (maxLen.z < minLen.z)
	{
		len.z = maxLen.z;
		pos.z = boxMax2.z + boxSize1.y;
		normal.z = 1.0f;
	}
	else
	{
		len.z = minLen.z;
		pos.z = boxMin2.z - boxSize1.y;
		normal.z = -1.0f;
	}

	// 符号
	len.x = fabsf(len.x);
	len.y = fabsf(len.y);
	len.z = fabsf(len.z);

	//--- 押し出し
	pos = Vector3::Transform(pos, parentInv);
	//--- 最短距離軸を出す
	// X軸
	if (len.x <= len.y && len.x <= len.z)
	{
		// 押し出し
		transform1.translation.x = pos.x;
		// 法線
		normal.y = 0.0f;
		normal.z = 0.0f;
	}
	// Y軸
	else if (len.y <= len.x && len.y <= len.z)
	{
		// 押し出し
		transform1.translation.y = pos.y;
		// 法線
		normal.x = 0.0f;
		normal.z = 0.0f;
	}
	// Z軸
	else if (len.z <= len.x && len.z <= len.y)
	{
		// 押し出し
		transform1.translation.z = pos.z;
		// 法線
		normal.x = 0.0f;
		normal.y = 0.0f;
	}

	// 物理計算		// 相手が静的かで区別
	//CollisionPhysics(r1, rb2, normal, static2);
	CollisionPhysics(physics1, physics2, normal, static2);

	return true;
}

bool CollisionSystem::AABBvsSphere(Transform& transform1, Physics& physics1,
	Transform& transform2, Physics& physics2, bool static2)
{
	return false;

}

bool CollisionSystem::SphereVsAABB(Transform& transform1, Physics& physics1,
	Transform& transform2, Physics& physics2, bool static2)
{

	return false;
}

bool CollisionSystem::SphereVsSphere(Transform& transform1, Physics& physics1,
	Transform& transform2, Physics& physics2, bool static2)
{
	return false;

	//--- 詳細判定
	// 中心座標
	Vector3 pos1 = transform1.globalMatrix.Translation();
	Vector3 pos2 = transform2.globalMatrix.Translation();
	// 半径
	float radius1 = transform1.globalScale.x * 0.5f;
	float radius2 = transform2.globalScale.x * 0.5f;

	// 二点間距離
	Vector3 distance = pos1 - pos2;

	//--- 衝突判定
	if (distance.LengthSquared() >= (radius1 + radius2) * (radius1 + radius2)) return false;


	//--- 物理
	// トリガー
	if (physics1.trigger || physics2.trigger) return true;

	// 物理計算
	Vector3 normal = distance;
	normal.Normalize();
	//CollisionPhysics(physics1, physics2, normal, static2);

	//---  押し出し
	// 二点間と２半径の差
	float len = (radius1 + radius2) - distance.Length();
	// 押し出す方向
	Vector3 vec = normal * len;
	// 押し出し
	Vector3 pos = transform1.globalMatrix.Translation() + vec;
	// 親の逆行列
	Matrix parentInv;
	auto parentID = getGameObjectManager()->GetParent(transform1.id);
	if (parentID != NONE_GAME_OBJECT_ID)
	{
		auto* parentTrans = getGameObjectManager()->getComponentData<Transform>(parentID);
		parentInv = parentTrans->globalMatrix.Invert();
	}

	transform1.translation = Vector3::Transform(pos, parentInv);

	return true;
}

bool CollisionSystem::OBBvsOBB(Transform& transform1, Physics& physics1,
	Transform& transform2, Physics& physics2, bool static2)
{

	return false;
}

bool CollisionSystem::OBBvsSphere(Transform& transform1, Physics& physics1,
	Transform& transform2, Physics& physics2, bool static2)
{
	// 球
	Vector3 center = transform2.globalMatrix.Translation();
	float radius = transform2.globalScale.x * 0.5f;
	// OBB
	auto& obb = transform1;
	// 親の逆行列
	Matrix parentInv;
	auto parentID = getGameObjectManager()->GetParent(transform1.id);
	if (parentID != NONE_GAME_OBJECT_ID)
	{
		auto* parentTrans = getGameObjectManager()->getComponentData<Transform>(parentID);
		parentInv = parentTrans->globalMatrix.Invert();
	}

	// 当たり判定
	Vector3 distance = obb.globalMatrix.Translation() - center;
	Vector3 rA, rB, L, d;
	{
		Vector3 right = obb.globalMatrix.Right() * 0.5f;
		rA.x = right.Length();
		rB.x = radius;
		right.Normalize();
		d.x = distance.Dot(right);
		L.x = fabsf(d.x);

		if (L.x > rA.x + rB.x)
			return false;
	}
	{
		Vector3 up = obb.globalMatrix.Up() * 0.5f;
		rA.y = up.Length();
		rB.y = radius;
		up.Normalize();
		d.y = distance.Dot(up);
		L.y = fabsf(d.y);

		if (L.y > rA.y + rB.y)
			return false;
	}
	{
		Vector3 forward = obb.globalMatrix.Forward() * 0.5f;
		rA.z = forward.Length();
		rB.z = radius;
		forward.Normalize();
		d.z = distance.Dot(forward);
		L.z = fabsf(d.z);

		if (L.z > rA.z + rB.z)
			return false;
	}

	// トリガー
	if (physics1.trigger || physics2.trigger) return true;

	//--- 押し出し 
	Vector3 len = rA + rB - L;
	Vector3 normal;

	//--- 最短距離軸を出す
	// X軸
	if (len.x <= len.y && len.x <= len.z)
	{
		if (d.x >= 0)
		{
			normal = obb.globalMatrix.Right() * 0.5f;
			Vector3 pos = obb.globalMatrix.Translation() + normal * len.x;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
		else
		{
			normal = obb.globalMatrix.Left() * 0.5f;
			Vector3 pos = obb.globalMatrix.Translation() + normal * len.x;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
	}
	// Y軸
	else if (len.y <= len.x && len.y <= len.z)
	{
		if (d.y >= 0)
		{
			normal = obb.globalMatrix.Up() * 0.5f;
			Vector3 pos = obb.globalMatrix.Translation() + normal * len.y;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
		else
		{
			normal = obb.globalMatrix.Down() * 0.5f;
			Vector3 pos = obb.globalMatrix.Translation() + normal * len.y;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
	}
	// Z軸
	else if (len.z <= len.x && len.z <= len.y)
	{
		if (d.z >= 0)
		{
			normal = obb.globalMatrix.Forward() * 0.5f;
			Vector3 pos = obb.globalMatrix.Translation() + normal * len.z;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
		else
		{
			normal = obb.globalMatrix.Backward() * 0.5f;
			Vector3 pos = obb.globalMatrix.Translation() + normal * len.z;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
	}

	// 物理
	CollisionPhysics(physics1, physics2, normal, static2);

	return true;
}

bool CollisionSystem::SphereVsOBB(Transform& transform1, Physics& physics1,
	Transform& transform2, Physics& physics2, bool static2)
{
	// 球
	Vector3 center = transform1.globalMatrix.Translation();
	float radius = transform1.globalScale.x * 0.5f;
	// OBB
	auto& obb = transform2;
	// 親の逆行列
	Matrix parentInv;
	auto parentID = getGameObjectManager()->GetParent(transform1.id);
	if (parentID != NONE_GAME_OBJECT_ID)
	{
		auto* parentTrans = getGameObjectManager()->getComponentData<Transform>(parentID);
		parentInv = parentTrans->globalMatrix.Invert();
	}

	// 当たり判定
	Vector3 distance = obb.globalMatrix.Translation() - center;
	Vector3 rA, rB, L, d;
	{
		Vector3 right = obb.globalMatrix.Right() * 0.5f;
		rA.x = right.Length();
		rB.x = radius;
		right.Normalize();
		d.x = distance.Dot(right);
		L.x = fabsf(d.x);

		if (L.x > rA.x + rB.x)
			return false;
	}
	{
		Vector3 up = obb.globalMatrix.Up() * 0.5f;
		rA.y = up.Length();
		rB.y = radius;
		up.Normalize();
		d.y = distance.Dot(up);
		L.y = fabsf(d.y);

		if (L.y > rA.y + rB.y)
			return false;
	}
	{
		Vector3 forward = obb.globalMatrix.Forward() * 0.5f;
		rA.z = forward.Length();
		rB.z = radius;
		forward.Normalize();
		d.z = distance.Dot(forward);
		L.z = fabsf(d.z);

		if (L.z > rA.z + rB.z)
			return false;
	}

	// トリガー
	if (physics1.trigger || physics2.trigger) return true;

	//--- 押し出し 
	Vector3 len = rA + rB - L;
	Vector3 normal;

	//--- 最短距離軸を出す
	// X軸
	if (len.x <= len.y && len.x <= len.z)
	{
		if (d.x < 0)
		{
			normal = obb.globalMatrix.Right() * 0.5f;
			Vector3 pos = center + normal * len.x;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
		else
		{
			normal = obb.globalMatrix.Left() * 0.5f;
			Vector3 pos = center + normal * len.x;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
	}
	// Y軸
	else if (len.y <= len.x && len.y <= len.z)
	{
		if (d.y < 0)
		{
			normal = obb.globalMatrix.Up() * 0.5f;
			Vector3 pos = center + normal * len.y;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
		else
		{
			normal = obb.globalMatrix.Down() * 0.5f;
			Vector3 pos = center + normal * len.y;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
	}
	// Z軸
	else if (len.z <= len.x && len.z <= len.y)
	{
		if (d.z < 0)
		{
			normal = obb.globalMatrix.Forward() * 0.5f;
			Vector3 pos = center + normal * len.z;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
		else
		{
			normal = obb.globalMatrix.Backward() * 0.5f;
			Vector3 pos = center + normal * len.z;
			pos = Vector3::Transform(pos, parentInv);
			transform1.translation = pos;
		}
	}

	// 物理
	normal.Normalize();
	CollisionPhysics(physics1, physics2, normal, static2);

	return true;
}

// 物理計算
void CollisionPhysics(Physics& physics1, Physics& physics2, const Vector3& normal, bool static2)
{
	// デルタタイム
	float delta = (1.0f / 60.0f);
	// 壁ずりベクトル
	Vector3 scratch = Mathf::WallScratchVector(physics1.velocity, normal);
	// 垂直ベクトル
	Vector3 vertical1 = Mathf::WallVerticalVector(physics1.velocity, normal);
	Vector3 vertical2 = Mathf::WallVerticalVector(physics2.velocity, normal);

	//--- 垂直方向(反発)
	float e = (physics1.e * physics2.e);
	float mass1 = physics1.mass;
	float mass2 = physics2.mass;
	// 反発後の垂直速度
	Vector3 verticalVelocity1 =
		(vertical1 * (mass1 - e * mass2) + vertical2 * (1 + e) * mass2) /
		(mass1 + mass2);
	Vector3 verticalVelocity2 =
		((vertical2 * (mass2 - e * mass1) + vertical1 * (1 + e) * mass1)) /
		(mass1 + mass2);
	// 垂直力
	Vector3 verticalForce1 = verticalVelocity1;
	Vector3 verticalForce2 = verticalVelocity2;


	//--- 水平方向(摩擦)
	// 水平方向の力
	Vector3 horizontalForce1;
	Vector3 horizontalForce2;
	// 垂直抗力
	Vector3 N = verticalForce1;
	// 静止摩擦
	float myu_s = physics1.staticFriction * physics2.staticFriction;
	// 動摩擦
	float myu_d = physics1.dynamicFriction * physics2.dynamicFriction;
	// 動摩擦力
	float F_d = myu_d * N.Length();
	if (F_d > 1.0f) F_d = 1.0f;

	// 最大静止摩擦力より大きいか
	if (scratch.Length() > myu_s * N.Length())
	{
		// 水平方向の力
		horizontalForce1 = scratch - scratch * F_d;
	}
	// 最大静止摩擦力より大きいか(相手側)
	if (F_d > myu_s * vertical2.Length())
	{
		// 水平方向の力
		horizontalForce2 = scratch * F_d;
	}

	//--- ベクトルの合成・力の反映
	if (static2)
	{
		physics1.force = Vector3();
		physics1.acceleration = (verticalForce1 - verticalForce2) / 2 + horizontalForce1;
		physics1.velocity = physics1.acceleration + physics1.acceleration * delta;
	}
	else
	{
		physics1.force = Vector3();
		physics1.acceleration = verticalForce1 / 2 + horizontalForce1;
		physics1.velocity = physics1.acceleration + physics1.acceleration * delta;

		physics2.force = Vector3();
		physics2.acceleration = verticalForce2 / 2 + horizontalForce2;
		physics2.velocity = physics2.acceleration + physics2.acceleration * delta;
	}
}


Vector3 LenOBBToPoint(Transform& obb, Vector3& p)
{
	Vector3 Vec(0, 0, 0);   // 最終的に長さを求めるベクトル

	// 各軸についてはみ出た部分のベクトルを算出

	// X軸
	Vector3 right = obb.globalMatrix.Right();
	FLOAT L = right.Length();
	if (L <= 0) return Vector3();  // L=0は計算できない
	right.Normalize();
	FLOAT s = (p - obb.globalMatrix.Translation()).Dot(right) / L;
	// sの値から、はみ出した部分があればそのベクトルを加算
	s = fabs(s);
	if (s > 0)
		Vec += (1 - s) * L * right;   // はみ出した部分のベクトル算出
	// Y軸
	Vector3 up = obb.globalMatrix.Up();
	L = up.Length();
	if (L <= 0) return Vector3();  // L=0は計算できない
	up.Normalize();
	s = (p - obb.globalMatrix.Translation()).Dot(up) / L;
	// sの値から、はみ出した部分があればそのベクトルを加算
	s = fabs(s);
	if (s > 0)
		Vec += (1 - s) * L * up;   // はみ出した部分のベクトル算出
	// Z軸
	Vector3 forward = obb.globalMatrix.Forward();
	L = forward.Length();
	if (L <= 0) return Vector3();  // L=0は計算できない
	forward.Normalize();
	s = (p - obb.globalMatrix.Translation()).Dot(forward) / L;
	// sの値から、はみ出した部分があればそのベクトルを加算
	s = fabs(s);
	if (s > 0)
		Vec += (1 - s) * L * forward;   // はみ出した部分のベクトル算出

	return Vec;   // 長さを出力
}
