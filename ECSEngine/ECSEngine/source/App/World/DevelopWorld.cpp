/*****************************************************************//**
 * \file   DevelopWorld.cpp
 * \brief  開発用テストワールド
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "DevelopWorld.h"
#include <Engine/Engine.h>

#include <Engine/ECS/Base/WorldManager.h>
#include <Engine/ECS/Base/EntityManager.h>
#include <Engine/ECS/Base/GameObjectManager.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/ComponentTag.h>

#include <Engine/ECS/System/TransformSystem.h>
#include <Engine/ECS/System/ParentSystem.h>
#include <Engine/ECS/System/RenderingSystem.h>
#include <Engine/ECS/System/ImguiSystem.h>

#include <Engine/Utility/Input.h>
#include <Engine/Renderer/Base/Geometry.h>
#include <Engine/Renderer/D3D11/D3D11Utility.h>



using namespace ecs;


struct MaterialComponentData : public IComponentData
{
	ECS_DECLARE_COMPONENT_DATA(MaterialComponentData);
};

struct ObjectTag :public IComponentData
{
	ECS_DECLARE_COMPONENT_DATA(ObjectTag);
};

struct Test :public IComponentData
{
	ECS_DECLARE_COMPONENT_DATA(AAAAA);
};

class RotationSystem : public ecs::SystemBase {
public:
	explicit RotationSystem(World* pWorld) :
		SystemBase(pWorld)
	{}
	void onUpdate() override {
		foreach<Transform, ObjectTag>([](Transform& transform, ObjectTag& tag) {
			transform.rotation *= Quaternion::CreateFromYawPitchRoll(0.1f, 0, 0);
			//rot.x += 0.1f;
			});
	}
};

class ControllSystem : public ecs::SystemBase {
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
					Vector3 vPos = transform.localToWorld.Translation();
					Vector3 right = transform.localToWorld.Right();
					Vector3 up = Vector3(0, 1, 0);
					Vector3 forward = transform.localToWorld.Forward();
					Vector3 vLook = vPos + forward;
					float focus = 0.0f;

					// 速度
					float moveSpeed = 1.0f / 60.0f * 5;
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
					transform.localToWorld = Matrix::CreateScale(transform.scale);
					// 回転
					transform.localToWorld *= Matrix::CreateFromQuaternion(transform.rotation);
					// 移動
					transform.localToWorld *= Matrix::CreateTranslation(transform.translation);
					// グローバルマトリックス更新
					transform.localToWorld.Up(up);

					m_oldMousePos = *mousePos;
				});
	}
};

/// @brief スタート
void DevelopWorld::Start()
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

	MeshID meshSky = renderer->createMesh("SkyDome");
	auto* pSky = renderer->getMesh(meshSky);
	Geometry::SkyDome(*pSky, 36, 1.0f);

	MeshID meshPlane = renderer->createMesh("Plane");
	auto* pPlaneMesh = renderer->getMesh(meshPlane);
	Geometry::Plane(*pPlaneMesh, 20);

	// レンダーバッファの生成
	auto rdID = renderer->createRenderBuffer(shaderLitID, meshID);
	auto rdskyID = renderer->createRenderBuffer(shaderSkyID, meshSky);

	// バッチデータの作成
	auto objBitchID = renderer->creatBatchGroup(matLitID, meshID);


	// アーキタイプ
	Archetype archetype = Archetype::create<Transform, RenderData>();
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
	scale = Vector3(1, 1, 1);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	RenderData rdPlane;
	rdPlane.materialID = matUnlitID;
	rdPlane.meshID = meshPlane;

	auto plane = getGameObjectManager()->createGameObject("Plane", archetype);
	getGameObjectManager()->setComponentData<Transform>(plane, Transform(plane, pos, rot, scale));
	getGameObjectManager()->setComponentData(plane, rdPlane);

	// カメラ生成
	Archetype cameraArchetype = Archetype::create<Transform, Camera, InputTag>();

	auto entity = getGameObjectManager()->createGameObject("Camera", cameraArchetype);
	Camera cameraData;
	cameraData.isOrthographic = false;
	cameraData.fovY = 45;
	cameraData.nearZ = 1.0f;
	cameraData.farZ = 1000.0f;
	pos.x = 0;
	pos.z = -17.5f;
	pos.y = 17.5f;
	rot = Quaternion::CreateFromYawPitchRoll(3.141592f, 3.141592f / -7, 0);

	getGameObjectManager()->setComponentData<Transform>(entity, Transform(entity, pos, rot, scale));
	getGameObjectManager()->setComponentData(entity, cameraData);



	// 人
	Archetype archetypeHuman = Archetype::create<Transform>();
	archetypeHuman.addTag(objBitchID);

	// 腰
	auto Waist = getGameObjectManager()->createGameObject("Waist", archetypeHuman);
	pos = Vector3(0, 8, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(5, 2, 2);
	getGameObjectManager()->setComponentData<Transform>(Waist, Transform(Waist, pos, rot, scale));

	// 腿左
	auto ThighLeft = getGameObjectManager()->createGameObject("Thigh Left", archetypeHuman);
	getGameObjectManager()->SetParent(ThighLeft, Waist);
	pos = Vector3(-1.5f, -2.5f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.3f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ThighLeft, Transform(ThighLeft, pos, rot, scale));

	// 足左
	auto LegLeft = getGameObjectManager()->createGameObject("Leg Left", archetypeHuman);
	getGameObjectManager()->SetParent(LegLeft, ThighLeft);
	pos = Vector3(-0.0f, -3.0f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.6f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(LegLeft, Transform(LegLeft, pos, rot, scale));

	// 腿右
	Archetype a = archetypeHuman;
	a.addType<ObjectTag>();
	auto ThighRight = getGameObjectManager()->createGameObject("Thigh Right", a);
	getGameObjectManager()->SetParent(ThighRight, Waist);
	pos = Vector3(1.5f, -2.5f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.3f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ThighRight, Transform(ThighRight, pos, rot, scale));

	// 足右
	auto LegRight = getGameObjectManager()->createGameObject("Leg Right", archetypeHuman);
	getGameObjectManager()->SetParent(LegRight, ThighRight);
	pos = Vector3(0.0f, -3.0f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.6f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(LegRight, Transform(LegRight, pos, rot, scale));

	// 体
	auto Body = getGameObjectManager()->createGameObject("Body", archetypeHuman);
	getGameObjectManager()->SetParent(Body, Waist);
	pos = Vector3(0.0f, 4.25f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.8f, 3.0f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(Body, Transform(Body, pos, rot, scale));

	// 頭
	auto Head = getGameObjectManager()->createGameObject("Head", archetypeHuman);
	getGameObjectManager()->SetParent(Head, Body);
	pos = Vector3(0.0f, 3.85f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.4f, 0.25f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(Head, Transform(Head, pos, rot, scale));

	// 肩左
	auto ShoulderLeft = getGameObjectManager()->createGameObject("Shoulder Left", archetypeHuman);
	getGameObjectManager()->SetParent(ShoulderLeft, Body);
	pos = Vector3(-2.55f, 2.0f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.25f, 0.25f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ShoulderLeft, Transform(ShoulderLeft, pos, rot, scale));

	// 肩右
	auto ShoulderRight = getGameObjectManager()->createGameObject("Shoulder Right", archetypeHuman);
	getGameObjectManager()->SetParent(ShoulderRight, Body);
	pos = Vector3(2.55f, 2.0f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.25f, 0.25f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ShoulderRight, Transform(ShoulderRight, pos, rot, scale));

	// 腕左
	auto ArmLeft = getGameObjectManager()->createGameObject("Arm Left", archetypeHuman);
	getGameObjectManager()->SetParent(ArmLeft, ShoulderLeft);
	pos = Vector3(-3.1f, 0, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(5.0f, 0.75f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ArmLeft, Transform(ArmLeft, pos, rot, scale));

	// 腕左
	auto ArmRight = getGameObjectManager()->createGameObject("Arm Right", archetypeHuman);
	getGameObjectManager()->SetParent(ArmRight, ShoulderRight);
	pos = Vector3(3.1f, 0, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(5.0f, 0.75f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ArmRight, Transform(ArmRight, pos, rot, scale));

	// 回転ボックス
	archetypeHuman.addType<ObjectTag>();
	auto RotateBox = getGameObjectManager()->createGameObject("Rotate Box", archetypeHuman);
	pos = Vector3(0, 2, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(1, 1, 1);
	getGameObjectManager()->setComponentData<Transform>(RotateBox, Transform(RotateBox, pos, rot, scale));

	// システムの追加
	addSystem<ImguiSystem>();
	addSystem<RotationSystem>();
	addSystem<TransformSystem>();
	addSystem<ControllSystem>();
	addSystem<ParentSystem>();
	addSystem<RenderingSystem>();
}

/// @brief エンド
void DevelopWorld::End()
{

}

