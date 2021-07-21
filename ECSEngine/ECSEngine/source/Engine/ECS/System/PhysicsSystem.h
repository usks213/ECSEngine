/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  物理システム
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/Utility/Mathf.h>

#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/ColliderComponentData.h>
#include <Engine/ECS/ComponentData/RigidbodyComponentData.h>

// Bullet3
#include "BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"
#include "BulletDynamics/Dynamics/btSimulationIslandManagerMt.h"
#include "BulletDynamics/MLCPSolvers/btDantzigSolver.h"
#include "BulletDynamics/MLCPSolvers/btLemkeSolver.h"
#include "BulletDynamics/MLCPSolvers/btMLCPSolver.h"
#include "BulletDynamics/MLCPSolvers/btSolveProjectedGaussSeidel.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btPoolAllocator.h"
#include "btBulletCollisionCommon.h"


namespace ecs {

	class PhysicsSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(PhysicsSystem);

		struct PhysicsData
		{
			std::unique_ptr<btMotionState>		motion;	// トランスフォーム
			std::unique_ptr<btCollisionShape>	shape;	// コライダー
			std::unique_ptr<btCollisionObject>	body;	// リジッドボディ
		};

	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit PhysicsSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~PhysicsSystem() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

		void CreatePhysicsData(const Transform& transform, 
			const Collider& collider, const Rigidbody& rigidbody);

	private:
		/// @brief 重力
		Vector3 m_graviyAcceleration;

		//--- bullet3

		std::unique_ptr<btDefaultCollisionConfiguration>				m_pConfig;
		std::unique_ptr<btCollisionDispatcher>							m_pDispatcher;
		std::unique_ptr<btBroadphaseInterface>							m_pBroadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver>			m_pSolver;
		std::unique_ptr<btDiscreteDynamicsWorld>						m_pWorld;

		std::unordered_map<GameObjectID, std::unique_ptr<PhysicsData>>	m_physicsDatas;
	};
}