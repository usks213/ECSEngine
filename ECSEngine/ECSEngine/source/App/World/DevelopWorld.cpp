/*****************************************************************//**
 * \file   DevelopWorld.cpp
 * \brief  �J���p�e�X�g���[���h
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

					// ����
					Vector3 vPos = transform.localToWorld.Translation();
					Vector3 right = transform.localToWorld.Right();
					Vector3 up = Vector3(0, 1, 0);
					Vector3 forward = transform.localToWorld.Forward();
					Vector3 vLook = vPos + forward;
					float focus = 0.0f;

					// ���x
					float moveSpeed = 1.0f / 60.0f * 5;
					float rotSpeed = 3.141592f / 60.0f;

					// ���{�^��(�J������荞��
					if (GetKeyPress(VK_RBUTTON))
					{
						// ��]��
						float angleX = 360.f * mouseDist.x / SCREEN_WIDTH * 0.5f;	// ��ʈ����360�x��](��ʃT�C�Y�̔�����180�x��])
						float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT * 0.5f;	// ��ʈ����180�x��](��ʃT�C�Y�̔�����90�x��])

						transform.rotation *= Quaternion::CreateFromAxisAngle(up, XMConvertToRadians(-angleX));
						transform.rotation *= Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-angleY));

						//// ��]�ʂ����ƂɁA�e���̉�]�s����v�Z
						//DirectX::XMVECTOR rotPos = DirectX::XMVectorSubtract(vPos, vLook);
						//DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(angleX));
						//DirectX::XMVECTOR rotAxis = DirectX::XMVector3TransformCoord(right, rotY);
						//DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationAxis(rotAxis, DirectX::XMConvertToRadians(angleY));

						//// �����_�����_�Ƃ��ĉ�]
						//rotPos = DirectX::XMVector3TransformCoord(rotPos, rotY);
						//rotPos = DirectX::XMVector3TransformCoord(rotPos, rotX);
						//pos = DirectX::XMVectorAdd(rotPos, vLook);
					}
					// ��]
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

					// �ړ�
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

					// Matrix�X�V
					// �g�k
					transform.localToWorld = Matrix::CreateScale(transform.scale);
					// ��]
					transform.localToWorld *= Matrix::CreateFromQuaternion(transform.rotation);
					// �ړ�
					transform.localToWorld *= Matrix::CreateTranslation(transform.translation);
					// �O���[�o���}�g���b�N�X�X�V
					transform.localToWorld.Up(up);

					m_oldMousePos = *mousePos;
				});
	}
};

/// @brief �X�^�[�g
void DevelopWorld::Start()
{

	auto* renderer = getWorldManager()->getEngine()->getRendererManager();

	// �V�F�[�_�ǂݍ���
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Lit";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;
	ShaderID shaderLitID = renderer->createShader(shaderDesc);

	shaderDesc.m_name = "Unlit";
	ShaderID shaderUnlitID = renderer->createShader(shaderDesc);

	shaderDesc.m_name = "SkyDome";
	ShaderID shaderSkyID = renderer->createShader(shaderDesc);

	// �}�e���A���̍쐬
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

	// �e�N�X�`���̓ǂݍ���
	auto texID = renderer->createTextureFromFile("data/texture/wall001.jpg");
	//auto texID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(D3D::SHADER_TEX_SLOT_MAIN, texID, EShaderStage::PS);
	mat->setTexture("_MainTexture", texID);
	unlit->setTexture("_MainTexture", texID);

	auto skytexID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(D3D::SHADER_TEX_SLOT_SKYBOX, skytexID, EShaderStage::PS);
	skyMat->setTexture("_SkyTexture", skytexID);

	// ���b�V���̐���
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

	// �����_�[�o�b�t�@�̐���
	auto rdID = renderer->createRenderBuffer(shaderLitID, meshID);
	auto rdskyID = renderer->createRenderBuffer(shaderSkyID, meshSky);

	// �o�b�`�f�[�^�̍쐬
	auto objBitchID = renderer->creatBatchGroup(matLitID, meshID);


	// �A�[�L�^�C�v
	Archetype archetype = Archetype::create<Transform, RenderData>();
	// �������f�[�^
	Vector3 pos;
	Vector3 scale;
	Quaternion rot;
	RenderData rd;
	rd.materialID = matLitID;
	rd.meshID = meshID;
	RenderData rdSky;
	rdSky.materialID = shaderSkyID;
	rdSky.meshID = meshSky;

	// �X�J�C�h�[��
	scale = Vector3(500, 500, 500);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);

	auto sky = getGameObjectManager()->createGameObject("SkyDome", archetype);
	getGameObjectManager()->setComponentData<Transform>(sky, Transform(sky, pos, rot, scale));
	getGameObjectManager()->setComponentData(sky, rdSky);

	// ��
	scale = Vector3(1, 1, 1);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	RenderData rdPlane;
	rdPlane.materialID = matUnlitID;
	rdPlane.meshID = meshPlane;

	auto plane = getGameObjectManager()->createGameObject("Plane", archetype);
	getGameObjectManager()->setComponentData<Transform>(plane, Transform(plane, pos, rot, scale));
	getGameObjectManager()->setComponentData(plane, rdPlane);

	// �J��������
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



	// �l
	Archetype archetypeHuman = Archetype::create<Transform>();
	archetypeHuman.addTag(objBitchID);

	// ��
	auto Waist = getGameObjectManager()->createGameObject("Waist", archetypeHuman);
	pos = Vector3(0, 8, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(5, 2, 2);
	getGameObjectManager()->setComponentData<Transform>(Waist, Transform(Waist, pos, rot, scale));

	// �ڍ�
	auto ThighLeft = getGameObjectManager()->createGameObject("Thigh Left", archetypeHuman);
	getGameObjectManager()->SetParent(ThighLeft, Waist);
	pos = Vector3(-1.5f, -2.5f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.3f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ThighLeft, Transform(ThighLeft, pos, rot, scale));

	// ����
	auto LegLeft = getGameObjectManager()->createGameObject("Leg Left", archetypeHuman);
	getGameObjectManager()->SetParent(LegLeft, ThighLeft);
	pos = Vector3(-0.0f, -3.0f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.6f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(LegLeft, Transform(LegLeft, pos, rot, scale));

	// �ډE
	Archetype a = archetypeHuman;
	a.addType<ObjectTag>();
	auto ThighRight = getGameObjectManager()->createGameObject("Thigh Right", a);
	getGameObjectManager()->SetParent(ThighRight, Waist);
	pos = Vector3(1.5f, -2.5f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.3f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ThighRight, Transform(ThighRight, pos, rot, scale));

	// ���E
	auto LegRight = getGameObjectManager()->createGameObject("Leg Right", archetypeHuman);
	getGameObjectManager()->SetParent(LegRight, ThighRight);
	pos = Vector3(0.0f, -3.0f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.6f, 1.2f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(LegRight, Transform(LegRight, pos, rot, scale));

	// ��
	auto Body = getGameObjectManager()->createGameObject("Body", archetypeHuman);
	getGameObjectManager()->SetParent(Body, Waist);
	pos = Vector3(0.0f, 4.25f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.8f, 3.0f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(Body, Transform(Body, pos, rot, scale));

	// ��
	auto Head = getGameObjectManager()->createGameObject("Head", archetypeHuman);
	getGameObjectManager()->SetParent(Head, Body);
	pos = Vector3(0.0f, 3.85f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.4f, 0.25f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(Head, Transform(Head, pos, rot, scale));

	// ����
	auto ShoulderLeft = getGameObjectManager()->createGameObject("Shoulder Left", archetypeHuman);
	getGameObjectManager()->SetParent(ShoulderLeft, Body);
	pos = Vector3(-2.55f, 2.0f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.25f, 0.25f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ShoulderLeft, Transform(ShoulderLeft, pos, rot, scale));

	// ���E
	auto ShoulderRight = getGameObjectManager()->createGameObject("Shoulder Right", archetypeHuman);
	getGameObjectManager()->SetParent(ShoulderRight, Body);
	pos = Vector3(2.55f, 2.0f, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(0.25f, 0.25f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ShoulderRight, Transform(ShoulderRight, pos, rot, scale));

	// �r��
	auto ArmLeft = getGameObjectManager()->createGameObject("Arm Left", archetypeHuman);
	getGameObjectManager()->SetParent(ArmLeft, ShoulderLeft);
	pos = Vector3(-3.1f, 0, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(5.0f, 0.75f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ArmLeft, Transform(ArmLeft, pos, rot, scale));

	// �r��
	auto ArmRight = getGameObjectManager()->createGameObject("Arm Right", archetypeHuman);
	getGameObjectManager()->SetParent(ArmRight, ShoulderRight);
	pos = Vector3(3.1f, 0, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(5.0f, 0.75f, 0.8f);
	getGameObjectManager()->setComponentData<Transform>(ArmRight, Transform(ArmRight, pos, rot, scale));

	// ��]�{�b�N�X
	archetypeHuman.addType<ObjectTag>();
	auto RotateBox = getGameObjectManager()->createGameObject("Rotate Box", archetypeHuman);
	pos = Vector3(0, 2, 0);
	rot = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	scale = Vector3(1, 1, 1);
	getGameObjectManager()->setComponentData<Transform>(RotateBox, Transform(RotateBox, pos, rot, scale));

	// �V�X�e���̒ǉ�
	addSystem<ImguiSystem>();
	addSystem<RotationSystem>();
	addSystem<TransformSystem>();
	addSystem<ControllSystem>();
	addSystem<ParentSystem>();
	addSystem<RenderingSystem>();
}

/// @brief �G���h
void DevelopWorld::End()
{

}

