/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  �����V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/Utility/Mathf.h>
#include <Engine/Renderer/D3D11/D3D11RendererManager.h>

#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/ColliderComponentData.h>
#include <Engine/ECS/ComponentData/RigidbodyComponentData.h>

// Bullet3
#include "BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
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


class IBulletDebugDrawDX11;

namespace ecs {

	class PhysicsSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(PhysicsSystem);

		struct PhysicsData
		{
			std::unique_ptr<btMotionState>		motion;	// �g�����X�t�H�[��
			std::unique_ptr<btCollisionShape>	shape;	// �R���C�_�[
			std::unique_ptr<btCollisionObject>	body;	// ���W�b�h�{�f�B
		};

	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit PhysicsSystem(World* pWorld) :
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~PhysicsSystem() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
		void onUpdate() override;

		/// @brief �Q�[���I�u�W�F�N�g�X�V�O�Ɉ�x�����Ă΂��R�[���o�b�N
		void onStartGameObject(const GameObjectID& id) override;
		/// @brief �Q�[���I�u�W�F�N�g�폜���R�[���o�b�N
		void onDestroyGameObject(const GameObjectID& id) override;


		/// @brief �f�o�b�O�\��
		void debugDraw() {
			if (m_pDynamicsWorld)
				m_pDynamicsWorld->debugDrawWorld();
		}

	private:
		void CreatePhysicsData(const Transform& transform,
			const Collider& collider, const Rigidbody& rigidbody);
	private:
		/// @brief �d��
		Vector3 m_graviyAcceleration;

		//--- bullet3
		std::unique_ptr<IBulletDebugDrawDX11>							m_pDebugDraw;

		std::unique_ptr<btDefaultCollisionConfiguration>				m_pConfig;
		std::unique_ptr<btCollisionDispatcher>							m_pDispatcher;
		std::unique_ptr<btBroadphaseInterface>							m_pBroadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver>			m_pSolver;
		std::unique_ptr<btDiscreteDynamicsWorld>						m_pDynamicsWorld;

		std::unordered_map<GameObjectID, std::unique_ptr<PhysicsData>>	m_physicsDatas;

	private:

	};
}

	// debugRender
class IBulletDebugDrawDX11 : public btIDebugDraw
{
public:
	IBulletDebugDrawDX11();
	virtual ~IBulletDebugDrawDX11();

	void Setup(D3D11RendererManager* renderer);

public:
	virtual void	setDebugMode(int debugMode) { bitDebugMode = debugMode; }
	virtual int		getDebugMode() const { return bitDebugMode; }

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	virtual void flushLines() { FlushLine(); }
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		drawLine(PointOnB, PointOnB + normalOnB * distance, color);
		btVector3 ncolor(0, 0, 0);
		drawLine(PointOnB, PointOnB + normalOnB * 0.01f, ncolor);
	}

	//���Ή�
	virtual void	reportErrorWarning(const char* warningString) {}
	virtual void	draw3dText(const btVector3& location, const char* textString) {}

private:
	//���C���`����s
	void FlushLine();

	int bitDebugMode = 0;

	D3D11RendererManager* m_renderer;
	ID3D11Device1* m_pDevice;
	ID3D11DeviceContext1* m_pContext;

	D3D11Shader* m_pShader;
	D3D11Material* m_pMaterial;

	struct Vertex
	{
		Vertex() {}
		Vertex(const btVector3& p, const btVector3& c) {
			pos.x = p[0];
			pos.y = p[1];
			pos.z = p[2];
			color.x = c[0];
			color.y = c[1];
			color.z = c[2];
		}
		Vector3 pos;
		Vector3 color;
	};
	struct Line
	{
		Line() {}
		Line(const Vertex& f, const Vertex& t) : from(f), to(t) {}
		Line(const btVector3& f, const btVector3& t, const btVector3& fc, const btVector3& tc)
			: from(f, fc), to(t, tc) {}

		Vertex from, to;
	};
	static const size_t MAX_LINE = 100000;
	std::vector<Line> aLine;
};
