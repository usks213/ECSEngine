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
#include <Engine/ECS/Base/RenderPipeline.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/ColliderComponentData.h>
#include <Engine/ECS/ComponentData/RigidbodyComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/ComponentTag.h>

#include <Engine/ECS/System/AnimationSystem.h>
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

#include "../FBXModel.h"
#include "../System/DevelopSystem.h"

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
	{
		m_nCnt = 0;
	}
	void onUpdate() override {
		m_nCnt++;

		foreach<Transform, ObjectTag>([this](Transform& transform, ObjectTag& tag) {

			Vector3 vec1 = transform.translation;
			float len = vec1.Length();
			vec1.Normalize();
			Vector3 vec2 = Vector3(0, 1, 0);
			Vector3 dir = vec1.Cross(vec2);
			dir.Normalize();
			dir.y = sinf(sinf(m_nCnt * 0.05f)) * 0.00f;
			vec1.y = 0.0f;

			dir -= vec1 * fabsf(sinf(m_nCnt * 0.005f)) * 0.41f;
			dir += vec1 * 0.20f;

			transform.translation += dir * 0.75f;

			});


		foreach<Transform, InputTag>
			([this](Transform& transform, InputTag& tag)
				{
					// カメラの回転
					Vector3 pos = Mathf::RotationY(Vector3(0, 0, -1), m_nCnt) * (30 + cosf(m_nCnt * 0.01f) * 30);
					pos.y = 12;
					transform.translation = pos;

					Vector3 dir = pos;
					dir.Normalize();
					Matrix rotMat = Matrix::CreateLookAt(pos, Vector3(0,0,0), Vector3(0, 1, 0));
					transform.rotation = Quaternion::CreateFromRotationMatrix(rotMat.Invert());

				});

		return;
	}
private:
	int m_nCnt;
	float m_lengthRot;
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
	// システムの追加
	addSystem<DevelopSystem>();
	addSystem<SphereSystem>();
	//addSystem<ControllSystem>();
	addSystem<AnimationSystem>();
	addSystem<TransformSystem>();
	addSystem<PhysicsSystem>();


	auto* renderer = getWorldManager()->getEngine()->getRendererManager();

	// シェーダ読み込み
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Lit";
	shaderDesc.m_name = "Refract";
	//shaderDesc.m_name = "GBuffer";
	//shaderDesc.m_name = "AnimationGBuffer";
	shaderDesc.m_stages = ShaderStageFlags::VS | ShaderStageFlags::PS;
	ShaderID shaderLitID = renderer->createShader(shaderDesc);
	auto* pS = renderer->getShader(shaderLitID);
	//pS->m_type = ShaderType::Deferred;

	shaderDesc.m_name = "Unlit";
	ShaderID shaderUnlitID = renderer->createShader(shaderDesc);

	shaderDesc.m_name = "SkyDome";
	ShaderID shaderSkyID = renderer->createShader(shaderDesc);

	shaderDesc.m_name = "Wave";
	ShaderID shaderWaveID = renderer->createShader(shaderDesc);

	// マテリアルの作成
	auto matLitID = renderer->createMaterial("Lit", shaderLitID);
	Material* mat = renderer->getMaterial(matLitID);
	//mat->m_rasterizeState = RasterizeState::CULL_FRONT;
	mat->m_isTransparent = true;
	mat->m_blendState = BlendState::ALPHA;
	mat->m_materialBuffer._Flg ^= (UINT)SHADER::Material_Flg::TEXTURE;

	auto matUnlitID = renderer->createMaterial("Unlit", shaderUnlitID);
	Material* unlit = renderer->getMaterial(matUnlitID);
	//unlit->m_rasterizeState = RasterizeState::CULL_FRONT;

	auto matSkyID = renderer->createMaterial("SkyDome", shaderSkyID);
	Material* skyMat = renderer->getMaterial(matSkyID);

	auto matWaveID = renderer->createMaterial("Wave", shaderWaveID);
	Material* waveMat = renderer->getMaterial(matWaveID);
	waveMat->setFloat("_metallic", 0.0f);
	waveMat->setFloat("_roughness", 0.2f);
	waveMat->m_materialBuffer._Color.z = 0.5f;

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
	renderer->createTextureFromFile("data/texture/field005.jpg");

	renderer->createTextureFromFile("data/model/Sword And Shield Idle.fbm/Paladin_diffuse.png");

	// テクスチャの読み込み
	auto texID = renderer->createTextureFromFile("data/texture/field005.jpg");
	//auto texID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(SHADER::SHADER_SRV_SLOT_MAINTEX, texID, ShaderStage::PS);
	mat->setTexture("_MainTexture", texID);
	unlit->setTexture("_MainTexture", texID);
	waveMat->setTexture("_MainTexture", texID);

	auto skytexID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(SHADER::SHADER_SRV_SLOT_SKYDOOM, skytexID, ShaderStage::PS);
	skyMat->setTexture("_SkyTexture", skytexID);

	// メッシュの生成
	MeshID meshID = renderer->createMesh("TestMesh");
	auto* pMesh = renderer->getMesh(meshID);
	//Geometry::Sphere(*pMesh, 36, 1.0f, 1.0f / 18);
	Geometry::Cube(*pMesh);

	MeshID sphereID = renderer->createMesh("Sphere");
	auto* pSphere = renderer->getMesh(sphereID);
	Geometry::Sphere(*pSphere, 36, 1.0f, 1.0f / 36);

	MeshID meshSky = renderer->createMesh("SkyDome");
	auto* pSky = renderer->getMesh(meshSky);
	Geometry::SkyDome(*pSky, 36, 1.0f);

	MeshID meshPlane = renderer->createMesh("Plane");
	auto* pPlaneMesh = renderer->getMesh(meshPlane);
	Geometry::Plane(*pPlaneMesh, 1000, 0.1f, 1.0f / 100);

	// FBX読み込み
	FBXModel::FBXModelData fbxData;
	FBXModel::LoadFBXModel("data/model/terrain.fbx", fbxData);

	// レンダーバッファの生成
	auto rdID = renderer->createRenderBuffer(shaderLitID, meshID);
	auto rdskyID = renderer->createRenderBuffer(shaderSkyID, meshSky);

	// バッチデータの作成
	auto objBitchID = renderer->creatBatchGroup(matLitID, meshID);
	auto sphereBitchID = renderer->creatBatchGroup(matLitID, sphereID);
	auto planeBitchID = renderer->creatBatchGroup(matWaveID, meshPlane);


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
	archetype = Archetype::create<Transform, StaticType>();
	archetype.addTag(planeBitchID);
	pos = Vector3(0, 0, 0);
	scale = Vector3(1.4f, 1.0f, 1.4f);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);

	auto plane = getGameObjectManager()->createGameObject("Plane", archetype);
	getGameObjectManager()->setComponentData<Transform>(plane, Transform(plane, pos, rot, scale));
	//getGameObjectManager()->setComponentData(plane,Collider(Collider::ColliderType::POLYGON, meshPlane));
	//getGameObjectManager()->setComponentData(plane,Rigidbody(0.0f));

	// カメラ生成
	Archetype cameraArchetype = Archetype::create<Transform, Camera, InputTag, DynamicType>();

	auto entity = getGameObjectManager()->createGameObject("Camera", cameraArchetype);
	Camera cameraData;
	cameraData.isOrthographic = false;
	cameraData.fovY = 60;
	cameraData.nearZ = 1.0f;
	cameraData.farZ = 1000.0f;
	cameraData.renderTargetID = renderer->createRenderTarget("Camera1");
	cameraData.depthStencilID = renderer->createDepthStencil("Camera1");
	pos.x = 0;
	pos.z = -30;
	pos.y = 12;
	rot = Quaternion::CreateFromYawPitchRoll(3.141592f, -3.141592f * 0.05f, 0);
	scale = Vector3(10, 10, 10);

	getGameObjectManager()->setComponentData<Transform>(entity, Transform(entity, pos, rot, scale));
	getGameObjectManager()->setComponentData(entity, cameraData);


	//// ステージ
	//Archetype stageArchetype = Archetype::create<Transform, StaticType, Collider, Rigidbody>();
	//stageArchetype.addTag(objBitchID);

	//// Stage
	//auto Stage = getGameObjectManager()->createGameObject("Stage", stageArchetype);
	//pos = Vector3(0, 0, 0);
	//rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	//scale = Vector3(1, 1, 1);
	//getGameObjectManager()->setComponentData<Transform>(Stage, Transform(Stage, pos, rot, scale));
	//getGameObjectManager()->setComponentData(Stage, Collider(Collider::ColliderType::BOX));
	//getGameObjectManager()->setComponentData(Stage, Rigidbody(0.0f));
	///*RenderData fbxRD;
	//fbxRD.meshID = fbxData.meshID;
	//fbxRD.materialID = matLitID;
	//getGameObjectManager()->setComponentData(Stage, fbxRD);*/


	//// 杭
	//for (int y = 0; y < 10; y++)
	//{
	//	for (int x = 0; x < 10; x++)
	//	{
	//		auto Kui = getGameObjectManager()->createGameObject("Kui", stageArchetype);
	//		getGameObjectManager()->SetParent(Kui, Stage);
	//		pos = Vector3(x * 3 - 20 + y, y * 3 - 5, 0);
	//		rot = Quaternion::CreateFromYawPitchRoll(0, 0, rand() % 180 / 3.1415f);
	//		scale = Vector3(1, 1, 30);
	//		getGameObjectManager()->setComponentData<Transform>(Kui, Transform(Kui, pos, rot, scale));
	//		getGameObjectManager()->setComponentData(Kui, Rigidbody(0.0f));
	//		getGameObjectManager()->setComponentData(Kui, Collider(Collider::ColliderType::BOX));
	//	}
	//}

	// ボール
	{
		Archetype tarnsformArc = Archetype::create<Transform>();
		GameObjectID BallHost = getGameObjectManager()->createGameObject("BallHost", tarnsformArc);
		getGameObjectManager()->setComponentData<Transform>(BallHost, Transform(BallHost, Vector3(0, 0, 0)));
		Archetype sphereArch = Archetype::create<Transform, DynamicType, ObjectTag>();
		sphereArch.addTag(sphereBitchID);

		int numX = 16;
		for (int i = 0; i < numX; i++)
		{
			for (int j = 0; j < numX; ++j)
			{
				auto Ball = getGameObjectManager()->createGameObject("Ball", sphereArch);
				getGameObjectManager()->SetParent(Ball, BallHost);
				pos = Mathf::RotationY(Vector3(1, 0, 0), j * 360.0f / numX) * 10;
				pos.y = i * 2;
				rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
				scale = Vector3(1.0f, 1.0f, 1.0f);
				getGameObjectManager()->setComponentData<Transform>(Ball, Transform(Ball, pos, rot, scale));
				//getGameObjectManager()->setComponentData(Ball,Rigidbody(1.0f));
				//getGameObjectManager()->setComponentData(Ball, Collider(Collider::ColliderType::SPHERE));
				PointLight light;
				light.data.color = Vector4(rand() % 100, rand() % 100, rand() % 100, 1.0f);
				light.data.color.Normalize();
				light.data.color.w = 1.0f;
				light.data.range = 3.0f;
				//getGameObjectManager()->setComponentData(Ball, light);
			}
		}
	}


	// ボックス
	//{
	//	Archetype tarnsformArc = Archetype::create<Transform>();
	//	GameObjectID BoxHost = getGameObjectManager()->createGameObject("BoxHost", tarnsformArc);
	//	getGameObjectManager()->setComponentData<Transform>(BoxHost, Transform(BoxHost, Vector3(0, 0, 0)));
	//	Archetype sphereArch = Archetype::create<Transform, DynamicType, ObjectTag>();
	//	sphereArch.addTag(objBitchID);

	//	for (int i = 0; i < 0; i++)
	//	{
	//		auto Box = getGameObjectManager()->createGameObject("Box", sphereArch);
	//		getGameObjectManager()->SetParent(Box, BoxHost);
	//		pos = Vector3(rand() % 20 - 10, rand() % 20 + 5, rand() % 20 - 10);
	//		rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	//		scale = Vector3(1.0f, 1.0f, 1.0f);
	//		getGameObjectManager()->setComponentData<Transform>(Box, Transform(Box, pos, rot, scale));
	//		//getGameObjectManager()->setComponentData(Ball,Rigidbody(1.0f));
	//		//getGameObjectManager()->setComponentData(Ball, Collider(Collider::ColliderType::SPHERE));
	//		PointLight light;
	//		light.data.color = Vector4(rand() % 100, rand() % 100, rand() % 100, 1.0f);
	//		light.data.color.Normalize();
	//		light.data.color.w = 1.0f;
	//		light.data.range = 3.0f;
	//		//getGameObjectManager()->setComponentData(Ball, light);
	//	}
	//}

	// ライト
	Archetype pointArc = Archetype::create<Transform, DynamicType, PointLight, ObjectTag>();

	for (int i = 0; i < 16; i++)
	{
		auto point = getGameObjectManager()->createGameObject("PointLit", pointArc);
		pos = Mathf::RotationY(Vector3(1, 0, 0), i * 360.0f / 16) * 10;
		pos.y = 10;
		rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
		scale = Vector3(1.5f, 1.5f, 1.5f);
		getGameObjectManager()->setComponentData<Transform>(point, Transform(point, pos, rot, scale));
		PointLight light;
		light.data.color = Vector4(rand() % 100 - 50, rand() % 100, rand() % 100, 1.0f);
		light.data.color.Normalize();
		light.data.color.w = 5.0f;
		light.data.range = 50.0f;
		getGameObjectManager()->setComponentData(point, light);
	}

	// メインライト
	Archetype DirLit = Archetype::create<Transform, DynamicType, DirectionalLight>();
	auto mainLit = getGameObjectManager()->createGameObject("MainLight", DirLit);
	pos = Vector3(0, 10, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	getGameObjectManager()->setComponentData<Transform>(mainLit, Transform(mainLit, pos, rot, scale));
	getGameObjectManager()->setComponentData(mainLit, DirectionalLight());
}

/// @brief エンド
void PhysicsTestWorld::End()
{

}
