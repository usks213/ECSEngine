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
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/ComponentTag.h>

#include <Engine/ECS/System/TransformSystem.h>
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
public:
	explicit ControllSystem(World* pWorld) :
		SystemBase(pWorld)
	{}
	void onUpdate() override {
		foreach<Position, Rotation, WorldMatrix, InputTag>
			([](Position& pos, Rotation& rot, WorldMatrix& mtx, InputTag& tag)
				{
					// ����
					Vector3 right = mtx.value.Right();
					Vector3 up = Vector3(0, 1, 0);
					Vector3 forward = mtx.value.Forward();

					// ���x
					float moveSpeed = 1.0f / 60.0f * 5;
					float rotSpeed = 3.141592f / 60.0f;

					// ��]
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

					// �ړ�
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

	// �}�e���A���̍쐬
	auto matLitID = renderer->createMaterial("Lit", shaderLitID);
	Material* mat = renderer->getMaterial(matLitID);
	//mat->m_rasterizeState = ERasterizeState::CULL_FRONT;

	auto matUnlitID = renderer->createMaterial("Unlit", shaderUnlitID);
	Material* unlit = renderer->getMaterial(matUnlitID);
	//unlit->m_rasterizeState = ERasterizeState::CULL_FRONT;


	// �e�N�X�`���̓ǂݍ���
	auto texID = renderer->createTextureFromFile("data/texture/wall001.jpg");
	//auto texID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(D3D::SHADER_TEX_SLOT_MAIN, texID, EShaderStage::PS);

	auto skytexID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(D3D::SHADER_TEX_SLOT_SKYBOX, skytexID, EShaderStage::PS);

	// ���b�V���̐���
	MeshID meshID = renderer->createMesh("TestMesh");
	auto* pMesh = renderer->getMesh(meshID);
	Geometry::Sphere(*pMesh, 36, 1.0f, 1.0f / 18);
	//Geometry::Cube(*pMesh);

	MeshID meshSky = renderer->createMesh("SkyDome");
	auto* pSky = renderer->getMesh(meshSky);
	Geometry::SkyDome(*pSky, 36, 1.0f);


	// �����_�[�o�b�t�@�̐���
	auto rdID = renderer->createRenderBuffer(shaderLitID, meshID);
	auto rdskyID = renderer->createRenderBuffer(shaderUnlitID, meshSky);

	// �A�[�L�^�C�v
	Archetype archetype = Archetype::create<Position, Rotation, Scale, WorldMatrix, RenderData>();

	// �������f�[�^
	Position pos;
	Scale scale;
	Rotation rot;
	RenderData rd;
	rd.materialID = matLitID;
	rd.meshID = meshID;
	RenderData rdSky;
	rdSky.materialID = matUnlitID;
	rdSky.meshID = meshSky;

	// �X�J�C�h�[��
	scale.value = Vector3(500, 500, 500);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);

	auto plane = getEntityManager()->createEntity(archetype);
	getEntityManager()->setComponentData<Position>(plane, pos);
	getEntityManager()->setComponentData<Scale>(plane, scale);
	getEntityManager()->setComponentData<Rotation>(plane, rot);
	getEntityManager()->setComponentData(plane, rdSky);

	scale.value = Vector3(1, 1, 1);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	pos.value.y = 0;
	archetype.addType<ObjectTag>();
	// �I�u�W�F�N�g�̐���
	for (int i = 0; i < 10; ++i)
	{
		auto entity = getEntityManager()->createEntity(archetype);

		pos.value.x = -5 + i*3;
		getEntityManager()->setComponentData<Position>(entity, pos);
		getEntityManager()->setComponentData<Scale>(entity, scale);
		getEntityManager()->setComponentData<Rotation>(entity, rot);
		getEntityManager()->setComponentData(entity, rd);
	}

	// �J��������
	Archetype cameraArchetype = Archetype::create<Position, Rotation, Scale, WorldMatrix, Camera, InputTag>();

	auto entity = getEntityManager()->createEntity(cameraArchetype);
	Camera cameraData;
	cameraData.isOrthographic = false;
	cameraData.fovY = 45;
	cameraData.nearZ = 1.0f;
	cameraData.farZ = 1000.0f;
	pos.value.x = 0;
	pos.value.z = 5;
	rot.value = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	getEntityManager()->setComponentData<Position>(entity, pos);
	getEntityManager()->setComponentData<Scale>(entity, scale);
	getEntityManager()->setComponentData<Rotation>(entity, rot);
	getEntityManager()->setComponentData(entity, cameraData);

	// �V�X�e���̒ǉ�
	addSystem<ImguiSystem>();
	addSystem<ControllSystem>();
	addSystem<RotationSystem>();
	addSystem<TransformSystem>();
	addSystem<RenderingSystem>();
}

/// @brief �G���h
void DevelopWorld::End()
{

}

