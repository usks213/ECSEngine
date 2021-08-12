/*****************************************************************//**
 * \file   RenderingSystem.h
 * \brief  �����_�����O�V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "RenderingSystem.h"
#include <Engine/ECS/Base/EntityManager.h>

#include <Engine/Engine.h>
#include <Engine/Renderer/D3D11/D3D11RendererManager.h>

#include <Engine/ECS/System/PhysicsSystem.h>


using namespace ecs;


/// @brief ������
void RenderingSystem::onCreate()
{

}

/// @brief �폜��
void RenderingSystem::onDestroy()
{

}

/// @brief �X�V
void RenderingSystem::onUpdate()
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());


	// �J�����ݒ�
	Camera* mainCamera = nullptr;
	Vector3 cameraPos;
	foreach<Camera, Transform>(
		[&mainCamera, &cameraPos, &engine](Camera& camera, Transform& transform)
		{
			// �r���[�}�g���b�N�X�X�V
			Matrix mtxWorld = transform.globalMatrix;
			Vector3 pos = mtxWorld.Translation();
			Vector3 target = pos + mtxWorld.Forward();
			Vector3 up = mtxWorld.Up();
			camera.view = Matrix::CreateLookAt(pos, target, up);

			// �E�B���h�E�T�C�Y
			camera.width = engine->getWindowWidth();
			camera.height = engine->getWindowHeight();
			camera.aspect = camera.width / camera.height;

			// �v���W�F�N�V�����}�g���b�N�X�X�V
			if (camera.isOrthographic)
			{
				// ���s���e
				camera.projection = Matrix::CreateOrthographic(
					camera.width, camera.height, camera.nearZ, camera.farZ);
			}
			else
			{
				// �������e
				camera.projection = Matrix::CreatePerspectiveFieldOfView(
					XMConvertToRadians(camera.fovY), camera.aspect, camera.nearZ, camera.farZ);
			}

			// ���C���J����
			mainCamera = &camera;
			cameraPos = pos;
		});

	// �J�����Ȃ�
	if (mainCamera == nullptr) return;

	// �V�X�e���o�b�t�@�̐ݒ�
	//XMMATRIX mtxView, mtxProj;
	//mtxView = XMMatrixLookAtLH(XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	//float asoect = (float)engine->getWindowWidth() / engine->getWindowHeight();
	//mtxProj = XMMatrixPerspectiveFovLH(
	//	XMConvertToRadians(45), asoect, 1.0f, 1000.0f);

	// �f�B���N�V���i�����C�g
	DirectionalLightData dirLit;
	dirLit.ambient = Vector4(0.3f, 0.3f, 0.3f, 0.3f);
	dirLit.direction = Vector4(1.f, -1.5f, -1.f, 0.0f);


	// �V�X�e���o�b�t�@�̐ݒ�
	D3D::SystemBuffer sysBuffer;
	sysBuffer._mView = mainCamera->view.Transpose();
	sysBuffer._mProj = mainCamera->projection.Transpose();
	sysBuffer._viewPos = Vector4(cameraPos);
	sysBuffer._directionalLit = dirLit;
	renderer->setD3DSystemBuffer(sysBuffer);


	// �o�b�`�`��
	for (const auto& bitchID : renderer->m_batchGroupPool)
	{
		auto* pBitch = renderer->getBatchGroup(bitchID.first);
		const auto* mat = renderer->getMaterial(pBitch->m_materialID);
		const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, pBitch->m_meshID);

		renderer->setD3D11Material(pBitch->m_materialID);
		renderer->setD3D11RenderBuffer(rdID);

		
		for (auto* chunk : getEntityManager()->getChunkListByTag(bitchID.first))
		{
			auto transform = chunk->getComponentArray<Transform>();
			for (auto i = 0u; i < transform.Count(); ++i)
			{
				//Matrix scale = Matrix::CreateScale(transform[i].localScale * transform[i].parentScale);
				renderer->setD3DTransformBuffer(transform[i].globalMatrix);
				renderer->d3dRender(rdID);
			}
		}
	}


	// �I�u�W�F�N�g�`��
	foreach<RenderData, Transform>(
		[&renderer](RenderData& rd, Transform& transform)
		{
			const auto* mat = renderer->getMaterial(rd.materialID);
			const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, rd.meshID);

			renderer->setD3D11Material(rd.materialID);

			//Matrix scale = Matrix::CreateScale(transform.localScale * transform.parentScale);
			renderer->setD3DTransformBuffer(transform.globalMatrix);

			renderer->setD3D11RenderBuffer(rdID);

			renderer->d3dRender(rdID);
		});


	// BulletDebugDraw
	auto* physicsSytem = m_pWorld->getSystem<PhysicsSystem>();
	if (physicsSytem)
	{
		physicsSytem->debugDraw();
	}
}


inline void RenderingSystem::updateTransform(Transform& transform)
{
	// �g�k
	transform.localMatrix = Matrix::CreateScale(transform.scale);
	// ��]
	transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
	// �ړ�
	transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
}

inline void RenderingSystem::updateCamera(Camera& camera, Transform& transform, float width, float height)
{
	// �r���[�}�g���b�N�X�X�V
	Matrix mtxWorld = transform.globalMatrix;
	Vector3 pos = mtxWorld.Translation();
	Vector3 target = pos + mtxWorld.Forward();
	Vector3 up = mtxWorld.Up();
	camera.view = Matrix::CreateLookAt(pos, target, up);

	// �E�B���h�E�T�C�Y
	camera.width = width;
	camera.height = height;
	camera.aspect = camera.width / camera.height;

	// �v���W�F�N�V�����}�g���b�N�X�X�V
	if (camera.isOrthographic)
	{
		// ���s���e
		camera.projection = Matrix::CreateOrthographic(
			camera.width, camera.height, camera.nearZ, camera.farZ);
	}
	else
	{
		// �������e
		camera.projection = Matrix::CreatePerspectiveFieldOfView(
			XMConvertToRadians(camera.fovY), camera.aspect, camera.nearZ, camera.farZ);
	}
}
