/*****************************************************************//**
 * \file   RenderPipeline.h
 * \brief  �����_�[�p�C�v���C��
 *
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/

#include "RenderPipeline.h"
#include <Engine/ECS/Base/EntityManager.h>

#include <Engine/Renderer/D3D11/D3D11RendererManager.h>
#include <Engine/Renderer/D3D11/D3D11RenderTarget.h>

#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/System/PhysicsSystem.h>


using namespace ecs;


/// @brief ������
void RenderPipeline::onCreate()
{

}

/// @brief �폜��
void RenderPipeline::onDestroy()
{

}

/// @brief �X�V
void RenderPipeline::onUpdate()
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// �J�����ݒ�
	Camera* mainCamera = nullptr;
	foreach<Camera, Transform>(
		[&mainCamera, &engine](Camera& camera, Transform& transform)
		{
			// �J�����X�V
			updateCamera(camera, transform, engine->getWindowWidth(), engine->getWindowHeight());
			// ���C���J����
			mainCamera = &camera;
		});

	// �J�����Ȃ�
	if (mainCamera == nullptr) return;

	// �f�B���N�V���i�����C�g
	

	// �J�����O
	cullingPass(*mainCamera);

	// �`��
	beginPass(*mainCamera);
	prePass();
	gbufferPass();
	shadowPass();
	opaquePass();
	skyPass();
	transparentPass();
	postPass();
	//endPass(*mainCamera);

	// �o�b�N�o�b�t�@�ɔ��f
	auto* rt = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(mainCamera->renderTargetID));
	renderer->d3dCopyResource(renderer->m_gbuffer.m_diffuseRT.Get(), rt->m_tex.Get());
}

void RenderPipeline::beginPass(Camera& camera)
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// �f�B���N�V���i�����C�g
	DirectionalLightData dirLit;
	dirLit.ambient = Vector4(0.3f, 0.3f, 0.3f, 0.3f);
	dirLit.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	dirLit.direction = Vector4(1.f, -1.5f, -1.f, 0.0f);

	// �V�X�e���o�b�t�@�̐ݒ�
	D3D::SystemBuffer sysBuffer;
	sysBuffer._mView = camera.view.Transpose();
	sysBuffer._mProj = camera.projection.Transpose();
	sysBuffer._viewPos = Vector4(camera.world.Translation());
	sysBuffer._directionalLit = dirLit;
	renderer->setD3DSystemBuffer(sysBuffer);

	// �r���[�|�[�g�w��
	float scale = 1.0f;
	if (camera.viewportScale > 0.0f)
		scale = camera.viewportScale;
	Viewport viewport(
		camera.viewportOffset.x,
		camera.viewportOffset.y,
		camera.width * scale,
		camera.height * scale
	);
	renderer->setViewport(viewport);

	// �����_�[�^�[�Q�b�g�w��
	renderer->setRenderTarget(camera.renderTargetID, camera.depthStencilID);
}

void RenderPipeline::cullingPass(Camera& camera)
{
	m_batchList.clear();
	m_opequeList.clear();
	m_transparentList.clear();
	m_pointLights.clear();
	m_spotLights.clear();

	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = engine->getRendererManager();

	// �J�����t���X�^��
	Frustum cameraFrustum(camera.farZ, camera.view, camera.projection);


	// �o�b�`�f�[�^
	for (const auto& bitchID : renderer->m_batchGroupPool)
	{
		auto mesh = renderer->getMesh(bitchID.second->m_meshID);
		for (auto* chunk : getEntityManager()->getChunkListByTag(bitchID.first))
		{
			auto transform = chunk->getComponentArray<Transform>();
			m_batchList[bitchID.first].resize(transform.Count());
			for (auto i = 0u; i < transform.Count(); ++i)
			{
				AABB aabb;
				AABB::transformAABB(transform[i].globalMatrix, mesh->m_aabb, aabb);
				// �J�����J�����O
				if (cameraFrustum.CheckAABB(aabb))
				{
					m_batchList[bitchID.first][i] = transform[i].globalMatrix;
				}
				// �V���h�E�J�����O

			}
		}
	}

	// �I�u�W�F�N�g�f�[�^
	foreach<RenderData, Transform>(
		[&](RenderData& rd, Transform& transform)
		{
			const auto* mat = renderer->getMaterial(rd.materialID);
			const auto* mesh = renderer->getMesh(rd.meshID);
			const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, rd.meshID);

			AABB aabb;
			AABB::transformAABB(transform.globalMatrix, mesh->m_aabb, aabb);
			// �J�����J�����O
			//if (cameraFrustum.CheckAABB(aabb))
			{
				RenderingData data;
				data.materialID = rd.materialID;
				data.renderBufferID = rdID;
				data.worldMatrix = transform.globalMatrix;
				data.cameraLengthSqr = (camera.world.Translation() - 
					transform.globalMatrix.Translation()).LengthSquared();
				if (mat->m_isTransparent)
				{
					// ������
					m_transparentList.push_back(data);
				}
				else
				{
					// �s����
					m_opequeList.push_back(data);
				}
			}
			// �V���h�E�J�����O

		});


}

void RenderPipeline::prePass()
{

}

void RenderPipeline::gbufferPass()
{

}

void RenderPipeline::shadowPass()
{

}

void RenderPipeline::opaquePass()
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// �o�b�`�`��
	for (auto& bitch : m_batchList)
	{
		auto* pBitch = renderer->getBatchGroup(bitch.first);
		const auto* mat = renderer->getMaterial(pBitch->m_materialID);
		const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, pBitch->m_meshID);

		renderer->setD3D11Material(pBitch->m_materialID);
		renderer->setD3D11RenderBuffer(rdID);

		for (auto& matrix : bitch.second)
		{
			renderer->setD3DTransformBuffer(matrix);
			renderer->d3dRender(rdID);
		}
	}

	// �s�����`��
	for (auto& opeque : m_opequeList)
	{
		renderer->setD3D11Material(opeque.materialID);
		renderer->setD3DTransformBuffer(opeque.worldMatrix);
		renderer->setD3D11RenderBuffer(opeque.renderBufferID);
		renderer->d3dRender(opeque.renderBufferID);
	}
}

void RenderPipeline::skyPass()
{

}

void RenderPipeline::transparentPass()
{

}

void RenderPipeline::postPass()
{

}

void RenderPipeline::endPass(Camera& camera)
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// �o�b�N�o�b�t�@�ɔ��f
	auto* rt = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(camera.renderTargetID));
	renderer->d3dCopyResource(renderer->m_backBufferRT.Get(), rt->m_tex.Get());
}

void renderingPass()
{
	//// �����_���[�}�l�[�W���[
	//auto* engine = m_pWorld->getWorldManager()->getEngine();
	//auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());


	//// �o�b�`�`��
	//for (const auto& bitchID : renderer->m_batchGroupPool)
	//{
	//	auto* pBitch = renderer->getBatchGroup(bitchID.first);
	//	const auto* mat = renderer->getMaterial(pBitch->m_materialID);
	//	const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, pBitch->m_meshID);

	//	renderer->setD3D11Material(pBitch->m_materialID);
	//	renderer->setD3D11RenderBuffer(rdID);


	//	for (auto* chunk : getEntityManager()->getChunkListByTag(bitchID.first))
	//	{
	//		auto transform = chunk->getComponentArray<Transform>();
	//		for (auto i = 0u; i < transform.Count(); ++i)
	//		{
	//			//Matrix scale = Matrix::CreateScale(transform[i].localScale * transform[i].parentScale);
	//			renderer->setD3DTransformBuffer(transform[i].globalMatrix);
	//			renderer->d3dRender(rdID);
	//		}
	//	}
	//}


	//// �I�u�W�F�N�g�`��
	//foreach<RenderData, Transform>(
	//	[&renderer](RenderData& rd, Transform& transform)
	//	{
	//		const auto* mat = renderer->getMaterial(rd.materialID);
	//		const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, rd.meshID);

	//		renderer->setD3D11Material(rd.materialID);

	//		//Matrix scale = Matrix::CreateScale(transform.localScale * transform.parentScale);
	//		renderer->setD3DTransformBuffer(transform.globalMatrix);

	//		renderer->setD3D11RenderBuffer(rdID);

	//		renderer->d3dRender(rdID);
	//	});
}


inline void RenderPipeline::updateCamera(Camera& camera, Transform& transform, float width, float height)
{
	// ���[���h�}�g���b�N�X�X�V
	camera.world = transform.globalMatrix;

	// �r���[�}�g���b�N�X�X�V
	Vector3 pos = camera.world.Translation();
	Vector3 target = pos + camera.world.Forward();
	Vector3 up = camera.world.Up();
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
