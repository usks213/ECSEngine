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

	// �V�X�e���o�b�t�@�̐ݒ�
	XMMATRIX mtxView, mtxProj;
	mtxView = XMMatrixLookAtLH(XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	float asoect = (float)engine->getWindowWidth() / engine->getWindowHeight();
	mtxProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45), asoect, 1.0f, 1000.0f);
	D3D::SystemBuffer sysBuffer;
	sysBuffer._mView = XMMatrixTranspose(mtxView);
	sysBuffer._mProj = XMMatrixTranspose(mtxProj);
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


