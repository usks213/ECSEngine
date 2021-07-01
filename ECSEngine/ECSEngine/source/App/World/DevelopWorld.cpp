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

#include <Engine/ECS/System/TransformSystem.h>
#include <Engine/ECS/System/RenderingSystem.h>

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

class RotationSystem : public ecs::SystemBase {
public:
	explicit RotationSystem(World* pWorld) :
		SystemBase(pWorld)
	{}
	void onUpdate() override {
		foreach<Rotation>([](Rotation& rot) {
			rot.value *= Quaternion::CreateFromYawPitchRoll(0.1f, 0, 0);
			//rot.value.x += 0.1f;
			});
	}
};

/// @brief �X�^�[�g
void DevelopWorld::Start()
{
	
	auto* renderer = getWorldManager()->getEngine()->getRendererManager();

	// �V�F�[�_�ǂݍ���
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Unlit";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;
	ShaderID shaderID = renderer->createShader(shaderDesc);

	// �}�e���A���̍쐬
	auto matID = renderer->createMaterial("TestMat", shaderID);

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

	pMesh->m_vertexCount = 4;
	for (int i = 0; i < 4; ++i)
	{
		pMesh->m_vertexData.positions.push_back(pVtx[i].vtx);
		pMesh->m_vertexData.normals.push_back(pVtx[i].nor);
		pMesh->m_vertexData.texcoord0s.push_back(pVtx[i].tex);
		pMesh->m_vertexData.colors.push_back(pVtx[i].diffuse);
	}

	// �����_�[�o�b�t�@�̐���
	auto rdID = renderer->createRenderBuffer(shaderID, meshID);

	// �A�[�L�^�C�v
	Archetype archetype = Archetype::create<Position, Rotation, Scale, WorldMatrix, RenderData>();

	// �������f�[�^
	Scale scale;
	scale.value = Vector3(1, 3, 1);
	Rotation rot;
	rot.value = Quaternion::CreateFromYawPitchRoll(3.14 / 2, 0, 0);
	RenderData rd;
	rd.materialID = matID;
	rd.meshID = meshID;

	// �I�u�W�F�N�g�̐���
	for (int i = 0; i < 10; ++i)
	{
		auto entity = getEntityManager()->createEntity(archetype);

		Position pos;
		pos.value.x = -5 + i;
		getEntityManager()->setComponentData<Position>(entity, pos);
		getEntityManager()->setComponentData<Scale>(entity, scale);
		getEntityManager()->setComponentData<Rotation>(entity, rot);
		getEntityManager()->setComponentData(entity, rd);
	}

	// �V�X�e���̒ǉ�
	addSystem<RotationSystem>();
	addSystem<TransformSystem>();
	addSystem<RenderingSystem>();
}

/// @brief �G���h
void DevelopWorld::End()
{

}

