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

#include <Engine/Utility/Input.h>
#include <Engine/Renderer/Base/Geometry.h>


using namespace ecs;

struct VERTEX_3D
{
	Vector3 vtx;		// ���_���W
	Vector3 nor;		// �@���x�N�g��
	Vector4 diffuse;	// �g�U���ˌ�
	Vector2 tex;		// �e�N�X�`�����W
};

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
			rot.value *= Quaternion::CreateFromYawPitchRoll(0.1f, 0, 0);
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
	ShaderID shaderID = renderer->createShader(shaderDesc);

	// �}�e���A���̍쐬
	auto matID = renderer->createMaterial("TestMat", shaderID);
	Material* mat = renderer->getMaterial(matID);
	mat->m_rasterizeState = ERasterizeState::CULL_NONE;

	// ���_���W�̐ݒ�
	VERTEX_3D pVtx[4];
	// ���_���W�̐ݒ�
	pVtx[0].vtx = Vector3(-1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[1].vtx = Vector3(-1.0f / 2, 1.0f / 2, 0.0f);
	pVtx[2].vtx = Vector3(1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[3].vtx = Vector3(1.0f / 2, 1.0f / 2, 0.0f);
	// �@���̐ݒ�
	pVtx[0].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = Vector3(0.0f, 0.0f, -1.0f);
	// ���ˌ��̐ݒ�
	pVtx[0].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = Vector2(0.0f, 1.0f);
	pVtx[1].tex = Vector2(0.0f, 0.0f);
	pVtx[2].tex = Vector2(1.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 0.0f);

	// ���b�V���̐���
	MeshID meshID = renderer->createMesh("TestMesh");
	auto* pMesh = renderer->getMesh(meshID);

	// �X�t�B�A
	Geometry::Sphere(*pMesh, 23, 1.0f, 1.0f);
	//Geometry::Cube(*pMesh);

	//pMesh->m_vertexCount = 4;
	//for (int i = 0; i < 4; ++i)
	//{
	//	pMesh->m_vertexData.positions.push_back(pVtx[i].vtx);
	//	pMesh->m_vertexData.normals.push_back(pVtx[i].nor);
	//	pMesh->m_vertexData.texcoord0s.push_back(pVtx[i].tex);
	//	pMesh->m_vertexData.colors.push_back(pVtx[i].diffuse);
	//}

	// �����_�[�o�b�t�@�̐���
	auto rdID = renderer->createRenderBuffer(shaderID, meshID);

	// �A�[�L�^�C�v
	Archetype archetype = Archetype::create<Position, Rotation, Scale, WorldMatrix, RenderData>();

	// �������f�[�^
	Position pos;
	Scale scale;
	Rotation rot;
	RenderData rd;
	rd.materialID = matID;
	rd.meshID = meshID;

	// ��
	scale.value = Vector3(10, 10, 1);
	rot.value = Quaternion::CreateFromYawPitchRoll(0, -3.141592f / 2, 0);

	auto plane = getEntityManager()->createEntity(archetype);
	getEntityManager()->setComponentData<Position>(plane, pos);
	getEntityManager()->setComponentData<Scale>(plane, scale);
	getEntityManager()->setComponentData<Rotation>(plane, rot);
	getEntityManager()->setComponentData(plane, rd);

	scale.value = Vector3(1, 1, 1);
	rot.value = Quaternion::CreateFromYawPitchRoll(3.14 / 2, 0, 0);
	pos.value.y = 5;
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
	addSystem<ControllSystem>();
	//addSystem<RotationSystem>();
	addSystem<TransformSystem>();
	addSystem<RenderingSystem>();
}

/// @brief �G���h
void DevelopWorld::End()
{

}

