/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  �����V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/

#include "PhysicsSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/PhysicsComponentData.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>

using namespace ecs;

#pragma comment(lib, "Bullet2FileLoader.lib")
#pragma comment(lib, "Bullet3Collision.lib")
#pragma comment(lib, "Bullet3Common.lib")
#pragma comment(lib, "Bullet3Dynamics.lib")
#pragma comment(lib, "Bullet3Geometry.lib")
#pragma comment(lib, "Bullet3OpenCL_clew.lib")
#pragma comment(lib, "BulletCollision.lib")
#pragma comment(lib, "BulletDynamics.lib")
#pragma comment(lib, "BulletInverseDynamics.lib")
#pragma comment(lib, "BulletSoftBody.lib")
#pragma comment(lib, "LinearMath.lib")


 /// @brief ������
void PhysicsSystem::onCreate()
{
	m_graviyAcceleration = Vector3(0, -0.8f, 0);

	//--- bullet3������
	m_pConfig = std::make_unique<btDefaultCollisionConfiguration>();
	m_pDispatcher = std::make_unique<btCollisionDispatcher>(m_pConfig.get());
	m_pBroadphase = std::make_unique<btDbvtBroadphase>();

	// constraint solver�i�S�������̉�@�j
	m_pSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
	{

	}

	// ���[���h�쐬
	m_pWorld = std::make_unique<btDiscreteDynamicsWorld>(
		m_pDispatcher.get(), m_pBroadphase.get(), m_pSolver.get(), m_pConfig.get());

	// ���[���h�ݒ�
	m_pWorld->setGravity(btVector3(0, -10, 0));

}

/// @brief �폜��
void PhysicsSystem::onDestroy()
{
	// bullet�֘A�̍폜
	// �폜���Ԃ�����̂Œ���
	if (m_pWorld) m_pWorld->setDebugDrawer(nullptr);

	// btBroadphaseInterface�Ȃǃ��[���h�쐬���Ɏw�肵�����̂�
	// btDiscreteDynamicsWorld�N���X�̃f�X�g���N�^�Ŏg�p���邽�ߌ�ō폜
	m_pWorld.reset();
}

/// @brief �X�V
void PhysicsSystem::onUpdate()
{
	// �f���^�^�C��
	float delta = (1.0f / 60.0f);

	//foreach<Transform, Physics, DynamicType>(
	//	[this, delta](Transform& transform, Physics& physics, DynamicType& type)
	//	{
	//		//===== ��] =====

	//		// �p���x
	//		physics.angularVelocity = Quaternion();
	//		// �g���N���Z
	//		physics.angularVelocity *= physics.torque;
	//		// ��]�̍X�V
	//		transform.rotation *= physics.angularVelocity;
	//		// ��R


	//		//===== �ړ� =====
	//		// �d��
	//		if (physics.gravity)
	//		{
	//			physics.force += m_graviyAcceleration * physics.mass;
	//		}
	//		// ��R
	//		physics.force *= (Vector3(1, 1, 1) - physics.drag);
	//		if (physics.force.Length() < 0.01f)
	//			physics.force = Vector3();
	//		// �����x
	//		physics.acceleration = physics.force / physics.mass;
	//		// ���x�X�V
	//		physics.velocity += physics.acceleration * delta;
	//		// �ʒu�̍X�V
	//		transform.translation += physics.velocity * delta;

	//	});


	// �O����
	foreach<Transform, Collider, Rigidbody>(
		[this, delta](Transform& transform, Collider& collider, Rigidbody& rigidbody)
		{
			// �����ς�
			if (rigidbody.isCreate)
			{
				// �����f�[�^�擾
				auto& physics = m_physicsDatas[transform.id];
				// �g�����X�t�H�[���f�[�^�𔽉f
				btVector3 pos;
				pos[0] = transform.translation.x;
				pos[1] = transform.translation.y;
				pos[2] = transform.translation.z;
				pos[3] = 0.0f;
				btQuaternion rot;
				rot[0] = transform.rotation.x;
				rot[1] = transform.rotation.y;
				rot[2] = transform.rotation.z;
				rot[3] = transform.rotation.w;
				//�g�����X�t�H�[���̍쐬
				btTransform bttransform;
				bttransform.setOrigin(pos);
				bttransform.setRotation(rot);
				// ���f
				physics->body->setWorldTransform(bttransform);
			}
			else
			{
				// �V�K�쐬
				this->CreatePhysicsData(transform, collider, rigidbody);
				rigidbody.isCreate = true;
			}
		});

	// �V�~�����[�V�����J�n
	m_pWorld->stepSimulation(delta);

	// �p�����[�^���f
	foreach<Transform, Collider, Rigidbody>(
		[this, delta](Transform& transform, Collider& collider, Rigidbody& rigidbody)
		{
			// �����f�[�^�擾
			auto& physics = m_physicsDatas[transform.id];

			// �g�����X�t�H�[���擾
			btTransform matrix;
			matrix = physics->body->getWorldTransform();

			transform.translation.x = matrix.getOrigin()[0];
			transform.translation.y = matrix.getOrigin()[1];
			transform.translation.z = matrix.getOrigin()[2];

			transform.rotation.x = matrix.getRotation()[0];
			transform.rotation.y = matrix.getRotation()[1];
			transform.rotation.z = matrix.getRotation()[2];
			transform.rotation.w = matrix.getRotation()[3];
		});
}


void PhysicsSystem::CreatePhysicsData(const Transform& transform,
	const Collider& collider, const Rigidbody& rigidbody)
{
	// �����f�[�^�쐬
	std::unique_ptr<PhysicsData> physics = std::make_unique<PhysicsData>();

	// �p�����[�^
	btVector3 pos;
	pos[0] = transform.translation.x;
	pos[1] = transform.translation.y;
	pos[2] = transform.translation.z;
	pos[3] = 0.0f;
	btQuaternion rot;
	rot[0] = transform.rotation.x;
	rot[1] = transform.rotation.y;
	rot[2] = transform.rotation.z;
	rot[3] = transform.rotation.w;
	btVector3 scale;
	scale[0] = transform.scale.x * 0.5f;
	scale[1] = transform.scale.y * 0.5f;
	scale[2] = transform.scale.z * 0.5f;
	scale[3] = 0.0f;

	// �R���C�_�[�̍쐬(Box)
	switch (collider.colliderType)
	{
	case Collider::ColliderType::BOX:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	case Collider::ColliderType::SPHERE:
		physics->shape = std::make_unique<btSphereShape>(scale.x());
		break;
	case Collider::ColliderType::CYLINDER:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	case Collider::ColliderType::CAPSULE:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	case Collider::ColliderType::CONE:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	case Collider::ColliderType::PLANE:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	case Collider::ColliderType::POLYGON:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	case Collider::ColliderType::COMPOUND:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	default:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	}

	// ���ʂŐÓI�E���I
	float mass = rigidbody.mass;
	bool isDynamic = (mass != 0.0f);
	btVector3 localInertia(0, 0, 0);
	if (isDynamic) {
		physics->shape->calculateLocalInertia(mass, localInertia);
	}

	//�g�����X�t�H�[���̍쐬
	btTransform bttransform;
	bttransform.setOrigin(pos);
	bttransform.setRotation(rot);
	physics->motion = std::make_unique<btDefaultMotionState>(bttransform);

	// ���W�b�h�{�f�B�̍쐬
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		mass, physics->motion.get(), physics->shape.get(), localInertia);
	physics->body = std::make_unique<btRigidBody>(rbInfo);

	// ���[���h�ɓo�^
	m_pWorld->addRigidBody(static_cast<btRigidBody*>(physics->body.get()));

	// �}�b�v�ɓo�^
	m_physicsDatas.emplace(transform.id, std::move(physics));
}
