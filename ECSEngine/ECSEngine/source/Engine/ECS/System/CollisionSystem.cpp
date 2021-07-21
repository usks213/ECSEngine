/*****************************************************************//**
 * \file   CollisionSystem.h
 * \brief  �����蔻��V�X�e��
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

 /// @brief ������
void CollisionSystem::onCreate()
{

}

/// @brief �폜��
void CollisionSystem::onDestroy()
{

}

/// @brief �X�V
void CollisionSystem::onUpdate()
{
	// �l���؃V�X�e���̎擾
	auto* quadTree = m_pWorld->getSystem<QuadTreeSystem>();
	if (quadTree == nullptr) return;

	// ���I�I�u�W�F�N�g�ƐÓI�I�u�W�F�N�g�𕪂��ē����蔻��

	//--- ���I�I�u�W�F�N�g���m
	// ���C����Ԃ̓����蔻��
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_dynamicMainList[i],
				false, false);
		}
	}
	// �T�u��Ԃ̓����蔻��
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_dynamicSubList[i],
				false, false);
		}
	}

	//--- ���I�I�u�W�F�N�g�ƐÓI�I�u�W�F�N�g
	// ���C����Ԃ̓����蔻��
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_staticMainList[i],
				false, true);
		}
	}
	// �T�u��Ԃ̓����蔻��
	for (int i = 0; i < quadTree->getCellNum(); ++i)
	{
		for (auto& entity : quadTree->m_dynamicMainList[i])
		{
			Collision(entity, quadTree->m_staticSubList[i], 
				false, true);
		}
	}
	////--- �ÓI�I�u�W�F�N�g�Ɠ��I�I�u�W�F�N�g
	//	// �T�u��Ԃ̓����蔻��
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

	// �T�u���[�v
	for (const auto& sub : m_subList)
	{
		// ����������
		if (main.m_chunkIndex == sub.m_chunkIndex &&
			main.m_index == sub.m_index) continue;

		//--- �����蔻�菈�� ---
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
			// �ڍה���
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
	//--- �����AABB�Ŏ���Ă���

	// �g���K�[
	if (physics1.trigger || physics2.trigger) return true;


	//--- �ڍה���
	Vector3 min(-0.5f, -0.5f, -0.5f);
	Vector3 max(0.5f, 0.5f, 0.5f);
	// �{�b�N�X�̍ő�ŏ�
	Vector3 boxMax1 = Vector3::Transform(max, transform1.globalMatrix);
	Vector3 boxMin1 = Vector3::Transform(min, transform1.globalMatrix);
	Vector3 boxMax2 = Vector3::Transform(max, transform2.globalMatrix);
	Vector3 boxMin2 = Vector3::Transform(min, transform2.globalMatrix);
	// �n�[�t�T�C�Y
	Vector3 boxSize1 = max * transform1.globalScale;
	Vector3 boxSize2 = max * transform2.globalScale;
	// �e�̋t�s��
	Matrix parentInv;
	auto parentID = getGameObjectManager()->GetParent(transform1.id);
	if (parentID != NONE_GAME_OBJECT_ID)
	{
		auto* parentTrans = getGameObjectManager()->getComponentData<Transform>(parentID);
		parentInv = parentTrans->globalMatrix.Invert();
	}

	//====================================================================
	// ����
	Vector3 maxLen = boxMax2 - boxMin1;
	Vector3	minLen = boxMax1 - boxMin2;

	// �e������
	Vector3 len;
	Vector3 pos;
	Vector3 normal;

	// X��
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
	// Y��
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
	// Z��
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

	// ����
	len.x = fabsf(len.x);
	len.y = fabsf(len.y);
	len.z = fabsf(len.z);

	//--- �����o��
	pos = Vector3::Transform(pos, parentInv);
	//--- �ŒZ���������o��
	// X��
	if (len.x <= len.y && len.x <= len.z)
	{
		// �����o��
		transform1.translation.x = pos.x;
		// �@��
		normal.y = 0.0f;
		normal.z = 0.0f;
	}
	// Y��
	else if (len.y <= len.x && len.y <= len.z)
	{
		// �����o��
		transform1.translation.y = pos.y;
		// �@��
		normal.x = 0.0f;
		normal.z = 0.0f;
	}
	// Z��
	else if (len.z <= len.x && len.z <= len.y)
	{
		// �����o��
		transform1.translation.z = pos.z;
		// �@��
		normal.x = 0.0f;
		normal.y = 0.0f;
	}

	// �����v�Z		// ���肪�ÓI���ŋ��
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

	//--- �ڍה���
	// ���S���W
	Vector3 pos1 = transform1.globalMatrix.Translation();
	Vector3 pos2 = transform2.globalMatrix.Translation();
	// ���a
	float radius1 = transform1.globalScale.x * 0.5f;
	float radius2 = transform2.globalScale.x * 0.5f;

	// ��_�ԋ���
	Vector3 distance = pos1 - pos2;

	//--- �Փ˔���
	if (distance.LengthSquared() >= (radius1 + radius2) * (radius1 + radius2)) return false;


	//--- ����
	// �g���K�[
	if (physics1.trigger || physics2.trigger) return true;

	// �����v�Z
	Vector3 normal = distance;
	normal.Normalize();
	//CollisionPhysics(physics1, physics2, normal, static2);

	//---  �����o��
	// ��_�ԂƂQ���a�̍�
	float len = (radius1 + radius2) - distance.Length();
	// �����o������
	Vector3 vec = normal * len;
	// �����o��
	Vector3 pos = transform1.globalMatrix.Translation() + vec;
	// �e�̋t�s��
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
	// ��
	Vector3 center = transform2.globalMatrix.Translation();
	float radius = transform2.globalScale.x * 0.5f;
	// OBB
	auto& obb = transform1;
	// �e�̋t�s��
	Matrix parentInv;
	auto parentID = getGameObjectManager()->GetParent(transform1.id);
	if (parentID != NONE_GAME_OBJECT_ID)
	{
		auto* parentTrans = getGameObjectManager()->getComponentData<Transform>(parentID);
		parentInv = parentTrans->globalMatrix.Invert();
	}

	// �����蔻��
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

	// �g���K�[
	if (physics1.trigger || physics2.trigger) return true;

	//--- �����o�� 
	Vector3 len = rA + rB - L;
	Vector3 normal;

	//--- �ŒZ���������o��
	// X��
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
	// Y��
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
	// Z��
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

	// ����
	CollisionPhysics(physics1, physics2, normal, static2);

	return true;
}

bool CollisionSystem::SphereVsOBB(Transform& transform1, Physics& physics1,
	Transform& transform2, Physics& physics2, bool static2)
{
	// ��
	Vector3 center = transform1.globalMatrix.Translation();
	float radius = transform1.globalScale.x * 0.5f;
	// OBB
	auto& obb = transform2;
	// �e�̋t�s��
	Matrix parentInv;
	auto parentID = getGameObjectManager()->GetParent(transform1.id);
	if (parentID != NONE_GAME_OBJECT_ID)
	{
		auto* parentTrans = getGameObjectManager()->getComponentData<Transform>(parentID);
		parentInv = parentTrans->globalMatrix.Invert();
	}

	// �����蔻��
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

	// �g���K�[
	if (physics1.trigger || physics2.trigger) return true;

	//--- �����o�� 
	Vector3 len = rA + rB - L;
	Vector3 normal;

	//--- �ŒZ���������o��
	// X��
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
	// Y��
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
	// Z��
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

	// ����
	normal.Normalize();
	CollisionPhysics(physics1, physics2, normal, static2);

	return true;
}

// �����v�Z
void CollisionPhysics(Physics& physics1, Physics& physics2, const Vector3& normal, bool static2)
{
	// �f���^�^�C��
	float delta = (1.0f / 60.0f);
	// �ǂ���x�N�g��
	Vector3 scratch = Mathf::WallScratchVector(physics1.velocity, normal);
	// �����x�N�g��
	Vector3 vertical1 = Mathf::WallVerticalVector(physics1.velocity, normal);
	Vector3 vertical2 = Mathf::WallVerticalVector(physics2.velocity, normal);

	//--- ��������(����)
	float e = (physics1.e * physics2.e);
	float mass1 = physics1.mass;
	float mass2 = physics2.mass;
	// ������̐������x
	Vector3 verticalVelocity1 =
		(vertical1 * (mass1 - e * mass2) + vertical2 * (1 + e) * mass2) /
		(mass1 + mass2);
	Vector3 verticalVelocity2 =
		((vertical2 * (mass2 - e * mass1) + vertical1 * (1 + e) * mass1)) /
		(mass1 + mass2);
	// ������
	Vector3 verticalForce1 = verticalVelocity1;
	Vector3 verticalForce2 = verticalVelocity2;


	//--- ��������(���C)
	// ���������̗�
	Vector3 horizontalForce1;
	Vector3 horizontalForce2;
	// �����R��
	Vector3 N = verticalForce1;
	// �Î~���C
	float myu_s = physics1.staticFriction * physics2.staticFriction;
	// �����C
	float myu_d = physics1.dynamicFriction * physics2.dynamicFriction;
	// �����C��
	float F_d = myu_d * N.Length();
	if (F_d > 1.0f) F_d = 1.0f;

	// �ő�Î~���C�͂��傫����
	if (scratch.Length() > myu_s * N.Length())
	{
		// ���������̗�
		horizontalForce1 = scratch - scratch * F_d;
	}
	// �ő�Î~���C�͂��傫����(���葤)
	if (F_d > myu_s * vertical2.Length())
	{
		// ���������̗�
		horizontalForce2 = scratch * F_d;
	}

	//--- �x�N�g���̍����E�͂̔��f
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
	Vector3 Vec(0, 0, 0);   // �ŏI�I�ɒ��������߂�x�N�g��

	// �e���ɂ��Ă͂ݏo�������̃x�N�g�����Z�o

	// X��
	Vector3 right = obb.globalMatrix.Right();
	FLOAT L = right.Length();
	if (L <= 0) return Vector3();  // L=0�͌v�Z�ł��Ȃ�
	right.Normalize();
	FLOAT s = (p - obb.globalMatrix.Translation()).Dot(right) / L;
	// s�̒l����A�͂ݏo��������������΂��̃x�N�g�������Z
	s = fabs(s);
	if (s > 0)
		Vec += (1 - s) * L * right;   // �͂ݏo���������̃x�N�g���Z�o
	// Y��
	Vector3 up = obb.globalMatrix.Up();
	L = up.Length();
	if (L <= 0) return Vector3();  // L=0�͌v�Z�ł��Ȃ�
	up.Normalize();
	s = (p - obb.globalMatrix.Translation()).Dot(up) / L;
	// s�̒l����A�͂ݏo��������������΂��̃x�N�g�������Z
	s = fabs(s);
	if (s > 0)
		Vec += (1 - s) * L * up;   // �͂ݏo���������̃x�N�g���Z�o
	// Z��
	Vector3 forward = obb.globalMatrix.Forward();
	L = forward.Length();
	if (L <= 0) return Vector3();  // L=0�͌v�Z�ł��Ȃ�
	forward.Normalize();
	s = (p - obb.globalMatrix.Translation()).Dot(forward) / L;
	// s�̒l����A�͂ݏo��������������΂��̃x�N�g�������Z
	s = fabs(s);
	if (s > 0)
		Vec += (1 - s) * L * forward;   // �͂ݏo���������̃x�N�g���Z�o

	return Vec;   // �������o��
}
