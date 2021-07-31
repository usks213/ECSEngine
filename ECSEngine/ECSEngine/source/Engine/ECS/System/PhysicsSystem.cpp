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
#include <Engine/ECS/System/TransformSystem.h>

#include <Engine/Engine.h>
#include <Engine/Renderer/Base/RendererManager.h>

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

	// デバッグ表示
	m_pDebugDraw = std::make_unique<IBulletDebugDrawDX11>();
	m_pDebugDraw->Setup(static_cast<D3D11RendererManager*>(
		m_pWorld->getWorldManager()->getEngine()->getRendererManager()));
	m_pDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

	// ワールド作成
	m_pDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
		m_pDispatcher.get(), m_pBroadphase.get(), m_pSolver.get(), m_pConfig.get());

	// ワールド設定
	m_pDynamicsWorld->setGravity(btVector3(0, -10, 0));
	m_pDynamicsWorld->setDebugDrawer(m_pDebugDraw.get());
}

/// @brief 削除時
void PhysicsSystem::onDestroy()
{
	// bullet関連の削除
	// 削除順番があるので注意
	if (m_pDynamicsWorld) m_pDynamicsWorld->setDebugDrawer(nullptr);

	// btBroadphaseInterfaceなどワールド作成時に指定したものは
	// btDiscreteDynamicsWorldクラスのデストラクタで使用するため後で削除
	m_pDynamicsWorld.reset();
}

/// @brief ゲームオブジェクト生成時コールバック
void PhysicsSystem::onStartGameObject(const GameObjectID& id)
{
	auto* mgr = getGameObjectManager();
	auto* transform = mgr->getComponentData<Transform>(id);
	auto* collider = mgr->getComponentData<Collider>(id);
	auto* rigidbody = mgr->getComponentData<Rigidbody>(id);
	if (transform && collider && rigidbody)
	{
		if (!rigidbody->isCreate)
		{
			// 新規作成
			this->CreatePhysicsData(*transform, *collider, *rigidbody);
			rigidbody->isCreate = true;
		}
	}
}

/// @brief ゲームオブジェクト削除時コールバック
void PhysicsSystem::onDestroyGameObject(const GameObjectID& id)
{
	// 検索
	auto itr = m_physicsDatas.find(id);
	if (m_physicsDatas.end() == itr) return;

	// bulletWorldから削除
	m_pDynamicsWorld->removeCollisionObject(itr->second->body.get());

	// リストから削除
	m_physicsDatas.erase(itr);
}

/// @brief 更新
void PhysicsSystem::onUpdate()
{
	// デルタタイム
	float delta = (1.0f / 60.0f);

	// 前処理
	foreach<Transform, Collider, Rigidbody>(
		[this, delta](Transform& transform, Collider& collider, Rigidbody& rigidbody)
		{
			// 生成済み
			if (rigidbody.isCreate)
			{
				// 物理データ取得
				auto& physics = m_physicsDatas[transform.id];
				Matrix globalMatrix = Matrix::CreateScale(
					transform.globalScale).Invert() * transform.globalMatrix;
				Vector3 globalPos = globalMatrix.Translation();
				Quaternion globalRot = Quaternion::CreateFromRotationMatrix(globalMatrix);
				globalPos += Vector3::Transform(collider.center, Matrix::CreateFromQuaternion(globalRot));

				// トランスフォームデータを反映
				btVector3 pos;
				pos[0] = globalPos.x;
				pos[1] = globalPos.y;
				pos[2] = globalPos.z;
				pos[3] = 0.0f;
				btQuaternion rot;
				rot[0] = globalRot.x;
				rot[1] = globalRot.y;
				rot[2] = globalRot.z;
				rot[3] = globalRot.w;
				//トランスフォームの作成
				btTransform bttransform;
				bttransform.setOrigin(pos);
				bttransform.setRotation(rot);
				// 反映
				physics->body->setWorldTransform(bttransform);

				// コライダー
				btVector3 scale;
				scale[0] = transform.globalScale.x * collider.size.x;
				scale[1] = transform.globalScale.y * collider.size.y;
				scale[2] = transform.globalScale.z * collider.size.z;
				scale[3] = 0.0f;
				physics->shape->setLocalScaling(scale);
			}
			else
			{
				// 新規作成
				this->CreatePhysicsData(transform, collider, rigidbody);
				rigidbody.isCreate = true;
			}
		});

	// シミュレーション開始
	m_pDynamicsWorld->stepSimulation(delta);

	std::vector<GameObjectID> updateList;
	updateList.reserve(m_pDynamicsWorld->getNumCollisionObjects());

	// パラメータ反映
	foreach<Transform, Collider, Rigidbody, DynamicType>(
		[this, delta, &updateList](Transform& transform, Collider& collider, Rigidbody& rigidbody, DynamicType& type)
		{
			if (rigidbody.mass == 0.0f) return;

			// 物理データ取得
			auto& physics = m_physicsDatas[transform.id];

			// トランスフォーム取得
			btTransform matrix;
			matrix = physics->body->getWorldTransform();
			Vector3 globalPos;
			Quaternion globalRot;

			globalPos.x = matrix.getOrigin()[0];
			globalPos.y = matrix.getOrigin()[1];
			globalPos.z = matrix.getOrigin()[2];

			globalRot.x = matrix.getRotation()[0];
			globalRot.y = matrix.getRotation()[1];
			globalRot.z = matrix.getRotation()[2];
			globalRot.w = matrix.getRotation()[3];

			globalPos -= Vector3::Transform(collider.center, Matrix::CreateFromQuaternion(globalRot));

			Matrix globalMatrix = Matrix::CreateFromQuaternion(globalRot);
			globalMatrix *= Matrix::CreateTranslation(globalPos);

			transform.translation = globalMatrix.Translation();
			transform.rotation = Quaternion::CreateFromRotationMatrix(globalMatrix);

			// マトリックス更新
			TransformSystem::updateTransform(transform);

			// 更新リストに追加
			updateList.push_back(transform.id);
		});


	// 親子関係更新
	auto* mgr = getGameObjectManager();
	TransformSystem::updateHierarchy(mgr, updateList);
}

void PhysicsSystem::CreatePhysicsData(const Transform& transform,
	const Collider& collider, const Rigidbody& rigidbody)
{
	// 物理データ作成
	std::unique_ptr<PhysicsData> physics = std::make_unique<PhysicsData>();

	// パラメータ
	Matrix globalMatrix = Matrix::CreateScale(
		transform.globalScale).Invert() * transform.globalMatrix;
	Vector3 globalPos = globalMatrix.Translation();
	Quaternion globalRot = Quaternion::CreateFromRotationMatrix(globalMatrix);
	globalPos += Vector3::Transform(collider.center, Matrix::CreateFromQuaternion(globalRot));

	// 反映
	btVector3 pos;
	pos[0] = globalPos.x;
	pos[1] = globalPos.y;
	pos[2] = globalPos.z;
	pos[3] = 0.0f;
	btQuaternion rot;
	rot[0] = globalRot.x;
	rot[1] = globalRot.y;
	rot[2] = globalRot.z;
	rot[3] = globalRot.w;
	btVector3 scale;
	scale[0] = 1.0f;
	scale[1] = 1.0f;
	scale[2] = 1.0f;
	scale[3] = 0.0f;
	btVector3 normal;
	Vector3 up = transform.globalMatrix.Up();
	normal[0] = up.x;
	normal[1] = up.y;
	normal[2] = up.z;

	// コライダーの作成(Box)
	switch (collider.colliderType)
	{
	case Collider::ColliderType::BOX:
		physics->shape = std::make_unique<btBoxShape>(scale * 0.5f);
		break;
	case Collider::ColliderType::SPHERE:
		physics->shape = std::make_unique<btSphereShape>(scale.x());
		break;
	case Collider::ColliderType::CYLINDER:
		physics->shape = std::make_unique<btCylinderShape>(scale);
		break;
	case Collider::ColliderType::CAPSULE:
		physics->shape = std::make_unique<btCapsuleShape>(scale.x(), scale.y());
		break;
	case Collider::ColliderType::CONE:
		physics->shape = std::make_unique<btConeShape>(scale.x(), scale.y());
		break;
	case Collider::ColliderType::PLANE:
		physics->shape = std::make_unique<btStaticPlaneShape>(normal, 0.0f);
		break;
	case Collider::ColliderType::POLYGON:
	{
		auto* renderer = m_pWorld->getWorldManager()->getEngine()->getRendererManager();
		auto* pMesh = renderer->getMesh(collider.mesh);
		if (pMesh == nullptr)
		{
			physics->shape = std::make_unique<btBoxShape>(scale);
			break;
		}

		std::unique_ptr<btTriangleMesh> triangles = std::make_unique<btTriangleMesh>();
		for (std::uint32_t i = 0; i < pMesh->m_indexCount - pMesh->m_indexCount % 3; i += 3) {
			btVector3 v0, v1, v2;
			v0 = btVector3(
				pMesh->m_vertexData.positions[pMesh->m_indexData[i + 0]].x,
				pMesh->m_vertexData.positions[pMesh->m_indexData[i + 0]].y,
				pMesh->m_vertexData.positions[pMesh->m_indexData[i + 0]].z);
			v1 = btVector3(
				pMesh->m_vertexData.positions[pMesh->m_indexData[i + 1]].x,
				pMesh->m_vertexData.positions[pMesh->m_indexData[i + 1]].y,
				pMesh->m_vertexData.positions[pMesh->m_indexData[i + 1]].z);
			v2 = btVector3(
				pMesh->m_vertexData.positions[pMesh->m_indexData[i + 2]].x,
				pMesh->m_vertexData.positions[pMesh->m_indexData[i + 2]].y,
				pMesh->m_vertexData.positions[pMesh->m_indexData[i + 2]].z);
			triangles->addTriangle(v0, v1, v2);
		}
		std::unique_ptr<btConvexShape> convex = std::make_unique<btConvexTriangleMeshShape>(triangles.get());
		std::unique_ptr<btShapeHull>   hull = std::make_unique<btShapeHull>(convex.get());
		hull->buildHull(convex->getMargin());
		physics->shape = std::make_unique<btConvexHullShape>();
		btConvexHullShape* convexHullShape = static_cast<btConvexHullShape*>(physics->shape.get());
		for (int i = 0; i < hull->numVertices(); ++i) {
			convexHullShape->addPoint(hull->getVertexPointer()[i]);
		}
	}
		break;
	case Collider::ColliderType::COMPOUND:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	case Collider::ColliderType::TERRAIN:
	{
		auto* renderer = m_pWorld->getWorldManager()->getEngine()->getRendererManager();
		auto* pMesh = renderer->getMesh(collider.mesh);
		if (pMesh == nullptr)
		{
			physics->shape = std::make_unique<btBoxShape>(scale);
			break;
		}
		btScalar worldMin(-10);
		btScalar worldMax(10);
		physics->shape = std::make_unique<btHeightfieldTerrainShape>
			(pMesh->m_heightWidth, pMesh->m_heightWidth, pMesh->m_heightData.data(), worldMin, worldMax, 1, false);
	}
		break;
	default:
		physics->shape = std::make_unique<btBoxShape>(scale);
		break;
	}

	// スケール
	physics->shape->setLocalScaling(scale);

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
	m_pDynamicsWorld->addRigidBody(static_cast<btRigidBody*>(physics->body.get()));

	// マップに登録
	m_physicsDatas.emplace(transform.id, std::move(physics));
}


IBulletDebugDrawDX11::IBulletDebugDrawDX11()
{
}

IBulletDebugDrawDX11::~IBulletDebugDrawDX11()
{
}

void IBulletDebugDrawDX11::Setup(D3D11RendererManager* renderer)
{
	// 描画データの作成
	m_renderer = renderer;
	m_pDevice = renderer->m_d3dDevice.Get();
	m_pContext = renderer->m_d3dContext.Get();

	// シェーダー
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "BtLine";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;
	auto shaderID = renderer->createShader(shaderDesc);
	m_pShader = static_cast<D3D11Shader*>(renderer->getShader(shaderID));

	// マテリアル
	auto materialID = renderer->createMaterial("BtLine", shaderID);
	m_pMaterial = static_cast<D3D11Material*>(renderer->getMaterial(materialID));

}


// ライン描画実行
void IBulletDebugDrawDX11::FlushLine()
{
	if (aLine.size() == 0)return;

	// マテリアル更新
	m_renderer->setD3D11Material(m_pMaterial->m_id);

	//頂点データ構造体
	struct VertexData
	{
		XMFLOAT3 pos;
		XMFLOAT3 col;
	};
	ComPtr<ID3D11Buffer> vbuff, idxbuff;

	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Line) * aLine.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = aLine.data();
	hr = m_pDevice->CreateBuffer(&bd, &InitData, &vbuff);
	if (FAILED(hr)) {
		return;
	}

	// 入力レイアウト
	m_pContext->IASetInputLayout(m_pShader->m_inputLayout.Get());

	// 頂点バッファ
	UINT stride = sizeof(VertexData);
	UINT offset = 0;
	ID3D11Buffer* vb[1] = { vbuff.Get() };
	m_pContext->IASetVertexBuffers(0, 1, vb, &stride, &offset);

	// プリミティブ形状
	m_renderer->setD3D11PrimitiveTopology(EPrimitiveTopology::LINE_LIST);

	//ポリゴン描画
	m_pContext->Draw(aLine.size() * 2, 0);

	/*cbuff.Diffuse = XMVectorSet(1, 1, 1, 1);
	m_pContext->UpdateSubresource(pCBuff.Get(), 0, NULL, &cbuff, 0, 0);
	m_pContext->OMSetDepthStencilState(pDsTestOn.Get(), 0);
	m_pContext->Draw(aLine.size() * 2, 0);*/

	aLine.clear();
}


void IBulletDebugDrawDX11::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
	// バッファにまとめて、あとから一括描画
	if (aLine.size() >= MAX_LINE)return;
	aLine.push_back(Line(from, to, fromColor, toColor));
}

void IBulletDebugDrawDX11::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	drawLine(from, to, color, color);
}

