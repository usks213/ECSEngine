/*****************************************************************//**
 * \file   PhysicsTestWorld.h
 * \brief  物理テストワールド
 *
 * \author USAMI KOSHI
 * \date   2021/07/19
 *********************************************************************/

#include "PhysicsTestWorld.h"
#include <Engine/Engine.h>

#include <Engine/ECS/Base/WorldManager.h>
#include <Engine/ECS/Base/EntityManager.h>
#include <Engine/ECS/Base/GameObjectManager.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/ColliderComponentData.h>
#include <Engine/ECS/ComponentData/RigidbodyComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/ComponentTag.h>

#include <Engine/ECS/System/TransformSystem.h>
#include <Engine/ECS/System/ParentSystem.h>
#include <Engine/ECS/System/RenderingSystem.h>
#include <Engine/ECS/System/QuadTreeSystem.h>
#include <Engine/ECS/System/PhysicsSystem.h>
#include <Engine/ECS/System/CollisionSystem.h>
#include <Engine/ECS/System/ImguiSystem.h>

#include <Engine/Utility/Input.h>
#include <Engine/Renderer/Base/Geometry.h>
#include <Engine/Renderer/D3D11/D3D11Utility.h>



using namespace ecs;

struct ObjectTag :public IComponentData
{
	ECS_DECLARE_COMPONENT_DATA(ObjectTag);
};

class SphereSystem : public ecs::SystemBase {
	ECS_DECLARE_SYSTEM(SphereSystem);
public:
	explicit SphereSystem(World* pWorld) :
		SystemBase(pWorld)
	{}
	void onUpdate() override {
		foreach<Transform, ObjectTag>([](Transform& transform, ObjectTag& tag) {

			if (transform.translation.y < -5)
			{
				transform.translation.x = 10 - rand() % 20;
				transform.translation.y = 30;
				transform.translation.z = 0;
			}

			});
	}
};

class ControllSystem : public ecs::SystemBase {
	ECS_DECLARE_SYSTEM(ControllSystem);
private:
	POINT m_oldMousePos;
public:
	explicit ControllSystem(World* pWorld) :
		SystemBase(pWorld)
	{
		POINT* mousePos = GetMousePosition();
		m_oldMousePos = *mousePos;
	}
	void onUpdate() override {
		foreach<Transform, InputTag>
			([this](Transform& transform, InputTag& tag)
				{
					POINT* mousePos = GetMousePosition();
					POINT mouseDist = {
						mousePos->x - m_oldMousePos.x,
						mousePos->y - m_oldMousePos.y,
					};
					float SCREEN_WIDTH = m_pWorld->getWorldManager()->getEngine()->getWindowWidth();
					float SCREEN_HEIGHT = m_pWorld->getWorldManager()->getEngine()->getWindowHeight();

					// 向き
					Vector3 vPos = transform.localMatrix.Translation();
					Vector3 right = transform.localMatrix.Right();
					Vector3 up = Vector3(0, 1, 0);
					Vector3 forward = transform.localMatrix.Forward();
					Vector3 vLook = vPos + forward;
					float focus = 0.0f;

					// 速度
					float moveSpeed = 1.0f / 60.0f;
					float rotSpeed = 3.141592f / 60.0f;

					// 左ボタン(カメラ回り込み
					if (GetKeyPress(VK_RBUTTON))
					{
						// 回転量
						float angleX = 360.f * mouseDist.x / SCREEN_WIDTH * 0.5f;	// 画面一周で360度回転(画面サイズの半分で180度回転)
						float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT * 0.5f;	// 画面一周で180度回転(画面サイズの半分で90度回転)

						transform.rotation *= Quaternion::CreateFromAxisAngle(up, XMConvertToRadians(-angleX));
						transform.rotation *= Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-angleY));

						//// 回転量をもとに、各軸の回転行列を計算
						//DirectX::XMVECTOR rotPos = DirectX::XMVectorSubtract(vPos, vLook);
						//DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(angleX));
						//DirectX::XMVECTOR rotAxis = DirectX::XMVector3TransformCoord(right, rotY);
						//DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(angleY));

						//// 注視点を原点として回転
						//rotPos = DirectX::XMVector3TransformCoord(rotPos, rotY);
						//rotPos = DirectX::XMVector3TransformCoord(rotPos, rotX);
						//pos = DirectX::XMVectorAdd(rotPos, vLook);
					}
					// 回転
					if (GetKeyPress(VK_RIGHT))
					{
						transform.rotation *= Quaternion::CreateFromAxisAngle(up, -rotSpeed);
					}
					if (GetKeyPress(VK_LEFT))
					{
						transform.rotation *= Quaternion::CreateFromAxisAngle(up, rotSpeed);
					}
					if (GetKeyPress(VK_UP))
					{
						transform.rotation *= Quaternion::CreateFromAxisAngle(right, rotSpeed);
					}
					if (GetKeyPress(VK_DOWN))
					{
						transform.rotation *= Quaternion::CreateFromAxisAngle(right, -rotSpeed);
					}

					// 移動
					if (GetMouseButton(MOUSEBUTTON_R) || GetKeyPress(VK_SHIFT))
					{
						if (GetKeyPress(VK_SHIFT))
						{
							moveSpeed *= 2;
						}
						if (GetKeyPress(VK_D))
						{
							transform.translation += right * moveSpeed;
						}
						if (GetKeyPress(VK_A))
						{
							transform.translation += right * -moveSpeed;
						}
						if (GetKeyPress(VK_W))
						{
							transform.translation += forward * moveSpeed;
						}
						if (GetKeyPress(VK_S))
						{
							transform.translation += forward * -moveSpeed;
						}
					}

					// Matrix更新
					// 拡縮
					transform.localMatrix = Matrix::CreateScale(transform.scale);
					// 回転
					transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
					// 移動
					transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
					// グローバルマトリックス更新
					transform.localMatrix.Up(up);

					m_oldMousePos = *mousePos;
				});
	}
};

/// @brief スタート
void PhysicsTestWorld::Start()
{

	auto* renderer = getWorldManager()->getEngine()->getRendererManager();

	// シェーダ読み込み
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Lit";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;
	ShaderID shaderLitID = renderer->createShader(shaderDesc);

	shaderDesc.m_name = "Unlit";
	ShaderID shaderUnlitID = renderer->createShader(shaderDesc);

	shaderDesc.m_name = "SkyDome";
	ShaderID shaderSkyID = renderer->createShader(shaderDesc);

	// マテリアルの作成
	auto matLitID = renderer->createMaterial("Lit", shaderLitID);
	Material* mat = renderer->getMaterial(matLitID);
	//mat->m_rasterizeState = ERasterizeState::CULL_FRONT;

	auto matUnlitID = renderer->createMaterial("Unlit", shaderUnlitID);
	Material* unlit = renderer->getMaterial(matUnlitID);
	//unlit->m_rasterizeState = ERasterizeState::CULL_FRONT;

	auto matSkyID = renderer->createMaterial("SkyDome", shaderSkyID);
	Material* skyMat = renderer->getMaterial(matSkyID);

	renderer->createTextureFromFile("data/texture/wall000.jpg");
	renderer->createTextureFromFile("data/texture/wall001.jpg");
	renderer->createTextureFromFile("data/texture/wall002.jpg");
	renderer->createTextureFromFile("data/texture/wall003.jpg");
	renderer->createTextureFromFile("data/texture/wall004.jpg");

	renderer->createTextureFromFile("data/texture/field000.jpg");
	renderer->createTextureFromFile("data/texture/field001.jpg");
	renderer->createTextureFromFile("data/texture/field002.jpg");
	renderer->createTextureFromFile("data/texture/field003.jpg");
	renderer->createTextureFromFile("data/texture/field004.jpg");

	// テクスチャの読み込み
	auto texID = renderer->createTextureFromFile("data/texture/wall001.jpg");
	//auto texID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(D3D::SHADER_TEX_SLOT_MAIN, texID, EShaderStage::PS);
	mat->setTexture("_MainTexture", texID);
	unlit->setTexture("_MainTexture", texID);

	auto skytexID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(D3D::SHADER_TEX_SLOT_SKYBOX, skytexID, EShaderStage::PS);
	skyMat->setTexture("_SkyTexture", skytexID);

	// メッシュの生成
	MeshID meshID = renderer->createMesh("TestMesh");
	auto* pMesh = renderer->getMesh(meshID);
	//Geometry::Sphere(*pMesh, 36, 1.0f, 1.0f / 18);
	Geometry::Cube(*pMesh);

	MeshID sphereID = renderer->createMesh("Sphere");
	auto* pSphere = renderer->getMesh(sphereID);
	Geometry::Sphere(*pSphere, 36, 1.0f, 1.0f / 18);


	MeshID meshSky = renderer->createMesh("SkyDome");
	auto* pSky = renderer->getMesh(meshSky);
	Geometry::SkyDome(*pSky, 36, 1.0f);

	MeshID meshPlane = renderer->createMesh("Plane");
	auto* pPlaneMesh = renderer->getMesh(meshPlane);
	Geometry::Plane(*pPlaneMesh, 20, 1.0f / 20);

	// レンダーバッファの生成
	auto rdID = renderer->createRenderBuffer(shaderLitID, meshID);
	auto rdskyID = renderer->createRenderBuffer(shaderSkyID, meshSky);

	// バッチデータの作成
	auto objBitchID = renderer->creatBatchGroup(matLitID, meshID);
	auto sphereBitchID = renderer->creatBatchGroup(matLitID, sphereID);


	// アーキタイプ
	Archetype archetype = Archetype::create<Transform, RenderData, StaticType>();
	// 初期化データ
	Vector3 pos;
	Vector3 scale;
	Quaternion rot;
	RenderData rd;
	rd.materialID = matLitID;
	rd.meshID = meshID;
	RenderData rdSky;
	rdSky.materialID = shaderSkyID;
	rdSky.meshID = meshSky;

	// スカイドーム
	scale = Vector3(500, 500, 500);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);

	auto sky = getGameObjectManager()->createGameObject("SkyDome", archetype);
	getGameObjectManager()->setComponentData<Transform>(sky, Transform(sky, pos, rot, scale));
	getGameObjectManager()->setComponentData(sky, rdSky);

	// 床
	archetype = Archetype::create<Transform, RenderData, StaticType, Collider, Rigidbody>();
	scale = Vector3(20, 1, 20);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	RenderData rdPlane;
	rdPlane.materialID = matUnlitID;
	rdPlane.meshID = meshPlane;

	//auto plane = getGameObjectManager()->createGameObject("Plane", archetype);
	//getGameObjectManager()->setComponentData<Transform>(plane, Transform(plane, pos, rot, scale));
	//getGameObjectManager()->setComponentData(plane, 
	//	Physics(Physics::ColliderType::OBB, false, false));
	//getGameObjectManager()->setComponentData(plane, rdPlane);

	// カメラ生成
	Archetype cameraArchetype = Archetype::create<Transform, Camera, InputTag, DynamicType>();

	auto entity = getGameObjectManager()->createGameObject("Camera", cameraArchetype);
	Camera cameraData;
	cameraData.isOrthographic = false;
	cameraData.fovY = 60;
	cameraData.nearZ = 1.0f;
	cameraData.farZ = 1000.0f;
	pos.x = 0;
	pos.z = -30;
	pos.y = 10;
	rot = Quaternion::CreateFromYawPitchRoll(3.141592f, 0, 0);

	getGameObjectManager()->setComponentData<Transform>(entity, Transform(entity, pos, rot, scale));
	getGameObjectManager()->setComponentData(entity, cameraData);


	// ステージ
	Archetype stageArchetype = Archetype::create<Transform, StaticType, Collider, Rigidbody>();
	stageArchetype.addTag(objBitchID);

	// Stage
	auto Stage = getGameObjectManager()->createGameObject("Stage", stageArchetype);
	pos = Vector3(0, 0, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(1, 1, 1);
	getGameObjectManager()->setComponentData<Transform>(Stage, Transform(Stage, pos, rot, scale));
	getGameObjectManager()->setComponentData(Stage, Rigidbody(0.0f));


	// 杭
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			auto Kui = getGameObjectManager()->createGameObject("Kui", stageArchetype);
			getGameObjectManager()->SetParent(Kui, Stage);
			pos = Vector3(x * 3 - 20 + y, y * 3 - 5, 0);
			rot = Quaternion::CreateFromYawPitchRoll(0, 0, rand() % 180 / 3.1415f);
			scale = Vector3(1, 1, 30);
			getGameObjectManager()->setComponentData<Transform>(Kui, Transform(Kui, pos, rot, scale));
			getGameObjectManager()->setComponentData(Kui, Rigidbody(0.0f));
			getGameObjectManager()->setComponentData(Kui, Collider(Collider::ColliderType::BOX));
		}
	}

	// ボール
	Archetype sphereArch = Archetype::create<Transform, DynamicType, Collider, Rigidbody, ObjectTag>();
	sphereArch.addTag(sphereBitchID);

	for (int i = 0; i < 100; i++)
	{
		auto Ball = getGameObjectManager()->createGameObject("Ball", sphereArch);
		pos = Vector3(8 - rand() % 16, rand() % 20 + 10, 0);
		rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
		scale = Vector3(0.5f, 0.5f, 0.5f);
		getGameObjectManager()->setComponentData<Transform>(Ball, Transform(Ball, pos, rot, scale));
		getGameObjectManager()->setComponentData(Ball,Rigidbody(1.0f));
		getGameObjectManager()->setComponentData(Ball, Collider(Collider::ColliderType::SPHERE, Vector3()));
	}

	// システムの追加
	addSystem<ImguiSystem>();
	addSystem<SphereSystem>();
	addSystem<TransformSystem>();
	addSystem<ControllSystem>();
	addSystem<ParentSystem>();
	addSystem<PhysicsSystem>();
	//addSystem<QuadTreeSystem>();
	//addSystem<CollisionSystem>();
	addSystem<RenderingSystem>();
}

/// @brief エンド
void PhysicsTestWorld::End()
{

}

