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
		foreach<Rotation, ObjectTag>([](Rotation& rot, ObjectTag& tag) {
			rot.value *= Quaternion::CreateFromYawPitchRoll(0.01f, 0, 0);
			//rot.value.x += 0.1f;
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
		foreach<Position, Rotation, LocalToWorld, InputTag>
			([this](Position& pos, Rotation& rot, LocalToWorld& mtx, InputTag& tag)
				{
					POINT* mousePos = GetMousePosition();
					POINT mouseDist = {
						mousePos->x - m_oldMousePos.x,
						mousePos->y - m_oldMousePos.y,
					};
					float SCREEN_WIDTH = m_pWorld->getWorldManager()->getEngine()->getWindowWidth();
					float SCREEN_HEIGHT = m_pWorld->getWorldManager()->getEngine()->getWindowHeight();

					// 向き
					Vector3 vPos = mtx.value.Translation();
					Vector3 right = mtx.value.Right();
					Vector3 up = Vector3(0, 1, 0);
					Vector3 forward = mtx.value.Forward();
					Vector3 vLook = vPos + forward;
					float focus = 0.0f;
					mtx.value.Up(up);

					// 速度
					float moveSpeed = 1.0f / 60.0f * 5;
					float rotSpeed = 3.141592f / 60.0f;

					// 左ボタン(カメラ回り込み
					if (GetKeyPress(VK_RBUTTON))
					{
						// 回転量
						float angleX = 360.f * mouseDist.x / SCREEN_WIDTH * 0.5f;	// 画面一周で360度回転(画面サイズの半分で180度回転)
						float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT * 0.5f;	// 画面一周で180度回転(画面サイズの半分で90度回転)

						rot.value *= Quaternion::CreateFromAxisAngle(up, XMConvertToRadians(-angleX));
						rot.value *= Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-angleY));

						//// 回転量をもとに、各軸の回転行列を計算
						//DirectX::XMVECTOR rotPos = DirectX::XMVectorSubtract(vPos, vLook);
						//DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(angleX));
						//DirectX::XMVECTOR rotAxis = DirectX::XMVector3TransformCoord(right, rotY);
						//DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(angleY));

						//// 注視点を原点として回転
						//rotPos = DirectX::XMVector3TransformCoord(rotPos, rotY);
						//rotPos = DirectX::XMVector3TransformCoord(rotPos, rotX);
						//pos.value = DirectX::XMVectorAdd(rotPos, vLook);
					}
					// 回転
					if (GetKeyPress(VK_RIGHT))
					{
						rot.value *= Quaternion::CreateFromAxisAngle(up, -rotSpeed);
					}
					if (GetKeyPress(VK_LEFT))
					{
						rot.value *= Quaternion::CreateFromAxisAngle(up, rotSpeed);
					}
					if (GetKeyPress(VK_UP))
					{
						rot.value *= Quaternion::CreateFromAxisAngle(right, rotSpeed);
					}
					if (GetKeyPress(VK_DOWN))
					{
						rot.value *= Quaternion::CreateFromAxisAngle(right, -rotSpeed);
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
							pos.value += right * moveSpeed;
						}
						if (GetKeyPress(VK_A))
						{
							pos.value += right * -moveSpeed;
						}
						if (GetKeyPress(VK_W))
						{
							pos.value += forward * moveSpeed;
						}
						if (GetKeyPress(VK_S))
						{
							pos.value += forward * -moveSpeed;
						}
					}
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
	Archetype archetype = Archetype::create<Position, Rotation, Scale, LocalToWorld, RenderData, Name>();
	// 初期化データ
	Position pos;
	Scale scale;
	Rotation rot;
	RenderData rd;
	Name name;
	rd.materialID = matLitID;
	rd.meshID = meshID;
	RenderData rdSky;
	rdSky.materialID = shaderSkyID;
	rdSky.meshID = meshSky;

	// スカイドーム
	scale.value = Vector3(500, 500, 500);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	std::strcpy(name.value, "SkyDome");

	auto sky = getGameObjectManager()->createGameObject("SkyDome", archetype);
	getGameObjectManager()->setComponentData<Position>(sky, pos);
	getGameObjectManager()->setComponentData<Scale>(sky, scale);
	getGameObjectManager()->setComponentData<Rotation>(sky, rot);
	getGameObjectManager()->setComponentData(sky, rdSky);
	getGameObjectManager()->setComponentData(sky, name);

	// 床
	scale.value = Vector3(1, 1, 1);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	std::strcpy(name.value, "Plane");
	RenderData rdPlane;
	rdPlane.materialID = matUnlitID;
	rdPlane.meshID = meshPlane;

	auto plane = getGameObjectManager()->createGameObject("Plane", archetype);
	getGameObjectManager()->setComponentData<Position>(plane, pos);
	getGameObjectManager()->setComponentData<Scale>(plane, scale);
	getGameObjectManager()->setComponentData<Rotation>(plane, rot);
	getGameObjectManager()->setComponentData(plane, rdPlane);
	getGameObjectManager()->setComponentData(plane, name);

	// カメラ生成
	Archetype cameraArchetype = Archetype::create<Position, Rotation, Scale, LocalToWorld, Camera, InputTag, Name>();

	auto entity = getGameObjectManager()->createGameObject("Camera", cameraArchetype);
	Camera cameraData;
	cameraData.isOrthographic = false;
	cameraData.fovY = 45;
	cameraData.nearZ = 1.0f;
	cameraData.farZ = 1000.0f;
	pos.value.x = 0;
	pos.value.z = -17.5f;
	pos.value.y = 17.5f;
	rot.value = Quaternion::CreateFromYawPitchRoll(3.141592f, 3.141592f / -7, 0);
	std::strcpy(name.value, "Camera");

	getGameObjectManager()->setComponentData<Position>(entity, pos);
	getGameObjectManager()->setComponentData<Scale>(entity, scale);
	getGameObjectManager()->setComponentData<Rotation>(entity, rot);
	getGameObjectManager()->setComponentData(entity, cameraData);
	getGameObjectManager()->setComponentData(entity, name);



	// 人
	Archetype archetypeHuman = Archetype::create<Position, Rotation, Scale, LocalToWorld>();
	archetypeHuman.addTag(objBitchID);

	// 腰
	auto Waist = getGameObjectManager()->createGameObject("Waist", archetypeHuman);
	pos.value = Vector3(0, 8, 0);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale.value = Vector3(5, 2, 2);
	getGameObjectManager()->setComponentData<Position>(Waist, pos);
	getGameObjectManager()->setComponentData<Scale>(Waist, scale);
	getGameObjectManager()->setComponentData<Rotation>(Waist, rot);

	// 腿左
	auto ThighLeft = getGameObjectManager()->createGameObject("Thigh Left", archetypeHuman);
	getGameObjectManager()->SetParent(ThighLeft, Waist);
	pos.value = Vector3(-0.25f, -1.25f, 0);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale.value = Vector3(0.3f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Position>(ThighLeft, pos);
	getGameObjectManager()->setComponentData<Scale>(ThighLeft, scale);
	getGameObjectManager()->setComponentData<Rotation>(ThighLeft, rot);

	// 足左
	auto LegLeft = getGameObjectManager()->createGameObject("Leg Left", archetypeHuman);
	getGameObjectManager()->SetParent(LegLeft, ThighLeft);
	pos.value = Vector3(-0.0f, -1.25f, 0);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale.value = Vector3(0.6f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Position>(LegLeft, pos);
	getGameObjectManager()->setComponentData<Scale>(LegLeft, scale);
	getGameObjectManager()->setComponentData<Rotation>(LegLeft, rot);

	// 腿右
	auto ThighRight = getGameObjectManager()->createGameObject("Thigh Right", archetypeHuman);
	getGameObjectManager()->SetParent(ThighRight, Waist);
	pos.value = Vector3(0.25f, -1.25f, 0);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale.value = Vector3(0.3f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Position>(ThighRight, pos);
	getGameObjectManager()->setComponentData<Scale>(ThighRight, scale);
	getGameObjectManager()->setComponentData<Rotation>(ThighRight, rot);

	// 足右
	auto LegRight = getGameObjectManager()->createGameObject("Leg Right", archetypeHuman);
	getGameObjectManager()->SetParent(LegRight, ThighRight);
	pos.value = Vector3(0.0f, -1.25f, 0);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale.value = Vector3(0.6f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Position>(LegRight, pos);
	getGameObjectManager()->setComponentData<Scale>(LegRight, scale);
	getGameObjectManager()->setComponentData<Rotation>(LegRight, rot);

	// 体
	auto Body = getGameObjectManager()->createGameObject("Body", archetypeHuman);
	getGameObjectManager()->SetParent(Body, Waist);
	pos.value = Vector3(0.0f, 2.15f, 0);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale.value = Vector3(0.8f, 3.0f, 0.8f);
	getGameObjectManager()->setComponentData<Position>(Body, pos);
	getGameObjectManager()->setComponentData<Scale>(Body, scale);
	getGameObjectManager()->setComponentData<Rotation>(Body, rot);

	// 頭
	auto Head = getGameObjectManager()->createGameObject("Head", archetypeHuman);
	getGameObjectManager()->SetParent(Head, Body);
	pos.value = Vector3(0.0f, 0.65f, 0);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale.value = Vector3(0.4f, 0.25f, 0.8f);
	getGameObjectManager()->setComponentData<Position>(Head, pos);
	getGameObjectManager()->setComponentData<Scale>(Head, scale);
	getGameObjectManager()->setComponentData<Rotation>(Head, rot);

	// 肩左
	auto ShoulderLeft = getGameObjectManager()->createGameObject("Shoulder Left", archetypeHuman);
	getGameObjectManager()->SetParent(ShoulderLeft, Body);
	pos.value = Vector3(0.65f, 0.4f, 0);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale.value = Vector3(0.2f, 0.2f, 0.7f);
	getGameObjectManager()->setComponentData<Position>(ShoulderLeft, pos);
	getGameObjectManager()->setComponentData<Scale>(ShoulderLeft, scale);
	getGameObjectManager()->setComponentData<Rotation>(ShoulderLeft, rot);



	// システムの追加
	addSystem<ImguiSystem>();
	addSystem<ControllSystem>();
	addSystem<RotationSystem>();
	addSystem<TransformSystem>();
	addSystem<ParentSystem>();
	addSystem<RenderingSystem>();
}

/// @brief エンド
void DevelopWorld::End()
{

}

