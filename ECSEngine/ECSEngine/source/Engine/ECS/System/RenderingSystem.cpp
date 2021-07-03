/*****************************************************************//**
 * \file   RenderingSystem.h
 * \brief  �����_�����O�V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "RenderingSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>

#include <Engine/Engine.h>
#include <Engine/Renderer/D3D11/D3D11RendererManager.h>

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
	foreach<Camera, WorldMatrix>(
		[&mainCamera, &cameraPos, &engine](Camera& camera, WorldMatrix& mtxWorld)
		{
			// �r���[�}�g���b�N�X�X�V
			Vector3 pos = mtxWorld.value.Translation();
			Vector3 target = pos + mtxWorld.value.Forward();
			Vector3 up = mtxWorld.value.Up();
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
					camera.fovY, camera.aspect, camera.nearZ, camera.farZ);
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
	dirLit.ambient = Vector4(0.1f, 0.1f, 0.1f, 0.1f);
	dirLit.diffuse = Vector4(2.0f, 1.0f, 1.0f, 1.0f);
	dirLit.specular = Vector4(0.1f, 0.1f, 0.1f, 0.2f);
	dirLit.direction = Vector4(1.f, -1.5f, 1.f, 0.0f);


	// �V�X�e���o�b�t�@�̐ݒ�
	D3D::SystemBuffer sysBuffer;
	sysBuffer._mView = mainCamera->view.Transpose();
	sysBuffer._mProj = mainCamera->projection.Transpose();
	sysBuffer._viewPos = Vector4(cameraPos);
	sysBuffer._directionalLit = dirLit;
	renderer->setD3DSystemBuffer(sysBuffer);


	// �I�u�W�F�N�g�`��
	foreach<RenderData, WorldMatrix>(
		[&renderer](RenderData& rd, WorldMatrix& mtxWorld)
		{
			renderer->setD3D11Material(rd.materialID);

			renderer->setD3DTransformBuffer(mtxWorld.value);

			renderer->render(rd.materialID, rd.meshID);
		});
}


