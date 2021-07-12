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
					if (GetMouseButton(MOUSEBUTTON_R))
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
	Geometry::Sphere(*pMesh, 36, 1.0f, 1.0f / 18);
	//Geometry::Cube(*pMesh);

	MeshID meshSky = renderer->createMesh("SkyDome");
	auto* pSky = renderer->getMesh(meshSky);
	Geometry::SkyDome(*pSky, 36, 1.0f);

	MeshID meshPlane = renderer->createMesh("Plane");
	auto* pPlaneMesh = renderer->getMesh(meshPlane);
	Geometry::Plane(*pPlaneMesh);

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

	auto sky = getEntityManager()->createEntity(archetype);
	getEntityManager()->setComponentData<Position>(sky, pos);
	getEntityManager()->setComponentData<Scale>(sky, scale);
	getEntityManager()->setComponentData<Rotation>(sky, rot);
	getEntityManager()->setComponentData(sky, rdSky);
	getEntityManager()->setComponentData(sky, name);

	// 床
	scale.value = Vector3(1, 1, 1);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	std::strcpy(name.value, "Plane");
	RenderData rdPlane;
	rdPlane.materialID = matUnlitID;
	rdPlane.meshID = meshPlane;

	auto plane = getEntityManager()->createEntity(archetype);
	getEntityManager()->setComponentData<Position>(plane, pos);
	getEntityManager()->setComponentData<Scale>(plane, scale);
	getEntityManager()->setComponentData<Rotation>(plane, rot);
	getEntityManager()->setComponentData(plane, rdPlane);
	getEntityManager()->setComponentData(plane, name);


	// オブジェクト
	scale.value = Vector3(1, 1, 1);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	pos.value.y = 2;
	archetype = Archetype::create<Position, Rotation, Scale, LocalToWorld, Name, LocalToParent>();
	archetype.addType<ObjectTag>();
	archetype.addTag(objBitchID);

	std::strcpy(name.value, "Sphere");

	// オブジェクトの生成
	int num = 2;
	for (int x = 0; x < num; ++x)
	{
		for (int y = 0; y < num; ++y)
		{
			for (int z = 0; z < num; ++z)
			{
				auto entity = getEntityManager()->createEntity(archetype);

				pos.value.x = x * 2;
				pos.value.y = y * 2 + 2;
				pos.value.z = z * 2;
				getEntityManager()->setComponentData<Position>(entity, pos);
				getEntityManager()->setComponentData<Scale>(entity, scale);
				getEntityManager()->setComponentData<Rotation>(entity, rot);
				//getEntityManager()->setComponentData(entity, rd);
				getEntityManager()->setComponentData(entity, name);
				getEntityManager()->setComponentData(entity, LocalToParent(plane));
			}
		}
	}

	// カメラ生成
	Archetype cameraArchetype = Archetype::create<Position, Rotation, Scale, LocalToWorld, Camera, InputTag, Name>();

	auto entity = getEntityManager()->createEntity(cameraArchetype);
	Camera cameraData;
	cameraData.isOrthographic = false;
	cameraData.fovY = 45;
	cameraData.nearZ = 1.0f;
	cameraData.farZ = 1000.0f;
	pos.value.x = 0;
	pos.value.z = -10;
	pos.value.y = 5;
	rot.value = Quaternion::CreateFromYawPitchRoll(3.141592f, 0, 0);
	std::strcpy(name.value, "Camera");

	getEntityManager()->setComponentData<Position>(entity, pos);
	getEntityManager()->setComponentData<Scale>(entity, scale);
	getEntityManager()->setComponentData<Rotation>(entity, rot);
	getEntityManager()->setComponentData(entity, cameraData);
	getEntityManager()->setComponentData(entity, name);

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

