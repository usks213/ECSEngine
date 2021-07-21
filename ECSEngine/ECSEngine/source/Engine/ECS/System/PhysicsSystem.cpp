/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  物理システム
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


 /// @brief 生成時
void PhysicsSystem::onCreate()
{
	m_graviyAcceleration = Vector3(0, -0.8f, 0);

	//--- bullet3初期化
	m_pConfig = std::make_unique<btDefaultCollisionConfiguration>();
	m_pDispatcher = std::make_unique<btCollisionDispatcher>(m_pConfig.get());
	m_pBroadphase = std::make_unique<btDbvtBroadphase>();

	// constraint solver（拘束条件の解法）
	m_pSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
	{

	}

	// ワールド作成
	m_pWorld = std::make_unique<btDiscreteDynamicsWorld>(
		m_pDispatcher.get(), m_pBroadphase.get(), m_pSolver.get(), m_pConfig.get());

	// ワールド設定
	m_pWorld->setGravity(btVector3(0, -10, 0));

}

/// @brief 削除時
void PhysicsSystem::onDestroy()
{
	// bullet関連の削除
	// 削除順番があるので注意
	if (m_pWorld) m_pWorld->setDebugDrawer(nullptr);

	// btBroadphaseInterfaceなどワールド作成時に指定したものは
	// btDiscreteDynamicsWorldクラスのデストラクタで使用するため後で削除
	m_pWorld.reset();
}

/// @brief 更新
void PhysicsSystem::onUpdate()
{
	// デルタタイム
	float delta = (1.0f / 60.0f);

	//foreach<Transform, Physics, DynamicType>(
	//	[this, delta](Transform& transform, Physics& physics, DynamicType& type)
	//	{
	//		//===== 回転 =====

	//		// 角速度
	//		physics.angularVelocity = Quaternion();
	//		// トルク加算
	//		physics.angularVelocity *= physics.torque;
	//		// 回転の更新
	//		transform.rotation *= physics.angularVelocity;
	//		// 抵抗


	//		//===== 移動 =====
	//		// 重力
	//		if (physics.gravity)
	//		{
	//			physics.force += m_graviyAcceleration * physics.mass;
	//		}
	//		// 抵抗
	//		physics.force *= (Vector3(1, 1, 1) - physics.drag);
	//		if (physics.force.Length() < 0.01f)
	//			physics.force = Vector3();
	//		// 加速度
	//		physics.acceleration = physics.force / physics.mass;
	//		// 速度更新
	//		physics.velocity += physics.acceleration * delta;
	//		// 位置の更新
	//		transform.translation += physics.velocity * delta;

	//	});


	// 前処理
	foreach<Transform, Collider, Rigidbody>(
		[this, delta](Transform& transform, Collider& collider, Rigidbody& rigidbody)
		{
			// 生成済み
			if (rigidbody.isCreate)
			{
				// 物理データ取得
				auto& physics = m_physicsDatas[transform.id];
				// トランスフォームデータを反映
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
				//トランスフォームの作成
				btTransform bttransform;
				bttransform.setOrigin(pos);
				bttransform.setRotation(rot);
				// 反映
				physics->body->setWorldTransform(bttransform);
			}
			else
			{
				// 新規作成
				this->CreatePhysicsData(transform, collider, rigidbody);
				rigidbody.isCreate = true;
			}
		});

	// シミュレーション開始
	m_pWorld->stepSimulation(delta);

	// パラメータ反映
	foreach<Transform, Collider, Rigidbody>(
		[this, delta](Transform& transform, Collider& collider, Rigidbody& rigidbody)
		{
			// 物理データ取得
			auto& physics = m_physicsDatas[transform.id];

			// トランスフォーム取得
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
	// 物理データ作成
	std::unique_ptr<PhysicsData> physics = std::make_unique<PhysicsData>();

	// パラメータ
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

	// コライダーの作成(Box)
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

	// 質量で静的・動的
	float mass = rigidbody.mass;
	bool isDynamic = (mass != 0.0f);
	btVector3 localInertia(0, 0, 0);
	if (isDynamic) {
		physics->shape->calculateLocalInertia(mass, localInertia);
	}

	//トランスフォームの作成
	btTransform bttransform;
	bttransform.setOrigin(pos);
	bttransform.setRotation(rot);
	physics->motion = std::make_unique<btDefaultMotionState>(bttransform);

	// リジッドボディの作成
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		mass, physics->motion.get(), physics->shape.get(), localInertia);
	physics->body = std::make_unique<btRigidBody>(rbInfo);

	// ワールドに登録
	m_pWorld->addRigidBody(static_cast<btRigidBody*>(physics->body.get()));

	// マップに登録
	m_physicsDatas.emplace(transform.id, std::move(physics));
}
