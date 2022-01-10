/*****************************************************************//**
 * \file   RenderPipeline.h
 * \brief  �����_�[�p�C�v���C��
 *
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/

#include "RenderPipeline.h"
#include <Engine/ECS/Base/EntityManager.h>

#include <Engine/Renderer/Base/Geometry.h>
#include <Engine/Renderer/D3D11/D3D11RendererManager.h>
#include <Engine/Renderer/D3D11/D3D11RenderTarget.h>
#include <Engine/Renderer/D3D11/D3D11DepthStencil.h>

#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/System/PhysicsSystem.h>

#include "imgui.h"


using namespace ecs;


/// @brief ������
void RenderPipeline::onCreate()
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	m_renderTarget = renderer->createRenderTarget("RenderPipeline");

	ShaderDesc desc;
	desc.m_name = "DeferredLit";
	desc.m_stages = ShaderStageFlags::VS | ShaderStageFlags::PS;
	auto defferdLitShader = renderer->createShader(desc);
	m_defferdLitMat = renderer->createMaterial("DeferredLit", defferdLitShader);
	auto mat = renderer->getMaterial(m_defferdLitMat);
	//mat->m_rasterizeState = RasterizeState::CULL_NONE;
	mat->m_depthStencilState = DepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE;

	auto quadMesh = renderer->createMesh("Quad");
	auto* pQuad = renderer->getMesh(quadMesh);
	Geometry::Quad(*pQuad);
	m_quadRb = renderer->createRenderBuffer(defferdLitShader ,quadMesh);

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
	prePass(*mainCamera);
	gbufferPass(*mainCamera);
	shadowPass(*mainCamera);
	opaquePass(*mainCamera);
	skyPass(*mainCamera);
	transparentPass(*mainCamera);
	postPass(*mainCamera);
	//endPass(*mainCamera);

	renderer->copyRenderTarget(m_renderTarget, mainCamera->renderTargetID);

	//// �o�b�N�o�b�t�@�ɔ��f
	//auto* rt = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(mainCamera->renderTargetID));
	//renderer->d3dCopyResource(renderer->m_gbuffer.m_diffuseRT.Get(), rt->m_tex.Get());
}

void RenderPipeline::cullingPass(Camera& camera)
{
	m_batchList.clear();
	m_opequeList.clear();
	m_transparentList.clear();
	m_deferredList.clear();
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
		auto mat = renderer->getMaterial(bitchID.second->m_materialID);
		for (auto* chunk : getEntityManager()->getChunkListByTag(bitchID.first))
		{
			auto transform = chunk->getComponentArray<Transform>();
			m_batchList[bitchID.first].reserve(transform.Count());
			for (auto i = 0u; i < transform.Count(); ++i)
			{
				AABB aabb;
				AABB::transformAABB(transform[i].globalMatrix, mesh->m_aabb, aabb);
				// �J�����J�����O
				//if (cameraFrustum.CheckAABB(aabb))
				{
					if (mat->m_shaderType == ShaderType::Forward)
					{
						RenderingData data;
						data.materialID = bitchID.second->m_materialID;
						data.renderBufferID = renderer->createRenderBuffer(mat->m_shaderID, bitchID.second->m_meshID);
						data.worldMatrix = transform[i].globalMatrix.Transpose();
						data.cameraLengthSqr = (camera.world.Translation() -
							transform[i].globalMatrix.Translation()).LengthSquared();
						// �t�H���[�h
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
					else if(mat->m_shaderType == ShaderType::Deferred)
					{
						// �f�t�@�[�h
						m_batchList[bitchID.first].push_back(transform[i].globalMatrix.Transpose());
					}
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
			if (!mat || !mesh) return;
			const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, rd.meshID);

			AABB aabb;
			AABB::transformAABB(transform.globalMatrix, mesh->m_aabb, aabb);
			// �J�����J�����O
			//if (cameraFrustum.CheckAABB(aabb))
			{
				RenderingData data;
				data.materialID = rd.materialID;
				data.renderBufferID = rdID;
				data.worldMatrix = transform.globalMatrix.Transpose();
				data.cameraLengthSqr = (camera.world.Translation() -
					transform.globalMatrix.Translation()).LengthSquared();
				data.isBone = false;
				data.meshID = rd.meshID;
				data.rootID = NONE_GAME_OBJECT_ID;

				if (mat->m_shaderType == ShaderType::Forward)
				{
					// �t�H���[�h
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
				else if (mat->m_shaderType == ShaderType::Deferred)
				{
					// �s����(�f�t�@�[�h)
					m_deferredList.push_back(data);
				}
			}
			// �V���h�E�J�����O

		});

	// �X�L�����b�V��
	foreach<SkinnedMeshRenderer, Transform>(
		[&](SkinnedMeshRenderer& rd, Transform& transform)
		{
			const auto* mat = renderer->getMaterial(rd.materialID);
			const auto* mesh = renderer->getMesh(rd.meshID);
			if (!mat || !mesh) return;
			const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, rd.meshID);

			AABB aabb;
			AABB::transformAABB(transform.globalMatrix, mesh->m_aabb, aabb);
			// �J�����J�����O
			//if (cameraFrustum.CheckAABB(aabb))
			{
				RenderingData data;
				data.materialID = rd.materialID;
				data.renderBufferID = rdID;
				data.worldMatrix = transform.globalMatrix.Transpose();
				data.cameraLengthSqr = (camera.world.Translation() -
					transform.globalMatrix.Translation()).LengthSquared();
				data.isBone = true;
				data.meshID = rd.meshID;
				data.rootID = rd.rootObjectID;

				if (mat->m_shaderType == ShaderType::Forward)
				{
					// �t�H���[�h
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
				else if (mat->m_shaderType == ShaderType::Deferred)
				{
					// �s����(�f�t�@�[�h)
					m_deferredList.push_back(data);
				}
			}
			// �V���h�E�J�����O

		});


	// �s�����̃\�[�g �J��������߂���(����)
	std::sort(m_opequeList.begin(), m_opequeList.end(), [](RenderingData& l, RenderingData& r) {
		return l.cameraLengthSqr < r.cameraLengthSqr;
		});
	std::sort(m_deferredList.begin(), m_deferredList.end(), [](RenderingData& l, RenderingData& r) {
		return l.cameraLengthSqr < r.cameraLengthSqr;
		});

	// �������̃\�[�g �J�������牓����(�~��)
	std::sort(m_transparentList.begin(), m_transparentList.end(), [](RenderingData& l, RenderingData& r) {
		return l.cameraLengthSqr > r.cameraLengthSqr;
		});


	//----- ���C�g�J�����O

	// �|�C���g���C�g
	foreach<PointLight, Transform>(
		[&](PointLight& point, Transform& transform)
		{
			// ���W�f�[�^
			point.data.position = transform.globalMatrix.Translation();

			// �J�����J�����O
			//if (cameraFrustum.CheckSphere(aabb))
			{
				m_pointLights.push_back(point.data);
			}
		});

	// �X�|�b�g���C�g
	foreach<SpotLight, Transform>(
		[&](SpotLight& spot, Transform& transform)
		{
			// ���W�E����
			spot.data.position = transform.globalMatrix.Translation();
			spot.data.direction = transform.globalMatrix.Forward();

			// �J�����J�����O
			//if (cameraFrustum.Check(aabb))
			{
				m_spotLights.push_back(spot.data);
			}
		});

}

void RenderPipeline::beginPass(Camera& camera)
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// �f�B���N�V���i�����C�g
		// �J�����ݒ�
	DirectionalLight* mainLit = nullptr;
	foreach<DirectionalLight, Transform>(
		[&mainLit, &engine](DirectionalLight& lit, Transform& transform)
		{
			// �����̍X�V
			lit.data.direction = Vector4(transform.globalMatrix.Forward());
			// �V���h�E�J�����X�V
			//updateCamera(camera, transform, engine->getWindowWidth(), engine->getWindowHeight());
			// ���C�����C�g
			mainLit = &lit;
		});
	DirectionalLightData dirLit;
	if (mainLit) {
		dirLit = mainLit->data;
	}
	else {
		dirLit.ambient = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
		dirLit.color = Vector4();
		dirLit.direction = Vector4();
	}

	// ���C�g�o�b�t�@�̐ݒ�
	renderer->setD3DLightBuffer(m_pointLights, m_spotLights);

	// �V�X�e���o�b�t�@�̐ݒ�
	SHADER::SystemBuffer sysBuffer;
	sysBuffer._mView = camera.view.Transpose();
	if (camera.isOrthographic)
		sysBuffer._mProj = camera.projection2d.Transpose();
	else
		sysBuffer._mProj = camera.projection.Transpose();
	sysBuffer._mProj2D = camera.projection2d.Transpose();
	sysBuffer._mViewInv = camera.view.Invert().Transpose();
	sysBuffer._mProjInv = camera.projection.Invert().Transpose();
	sysBuffer._viewPos = Vector4(camera.world.Translation());
	sysBuffer._directionalLit = dirLit;
	sysBuffer._pointLightNum = m_pointLights.size();
	sysBuffer._spotLightNum = m_spotLights.size();
	sysBuffer._time = engine->getCurrentTime();
	sysBuffer._frame = engine->getFrameCount();
	renderer->setD3DSystemBuffer(sysBuffer);

	// �����_�[�^�[�Q�b�g�N���A
	renderer->clearRenderTarget(camera.renderTargetID);
	renderer->clearDepthStencil(camera.depthStencilID);

	// �O���u�e�N�X�`���Z�b�g
	auto pGrab = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(m_renderTarget));
	renderer->m_d3dContext->PSSetShaderResources(SHADER::SHADER_SRV_SLOT_GRABTEX, 1, pGrab->m_srv.GetAddressOf());
}

void RenderPipeline::prePass(Camera& camera)
{

}

void RenderPipeline::gbufferPass(Camera& camera)
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());
	auto* goMgr = m_pWorld->getGameObjectManager();

	// �r���[�|�[�g�w��
	Viewport viewport(
		0,
		0,
		camera.width,
		camera.height
	);
	renderer->setViewport(viewport);

	//// �r���[�|�[�g�w��
	//float scale = 1.0f;
	//if (camera.viewportScale > 0.0f)
	//	scale = camera.viewportScale;
	//Viewport viewport(
	//	camera.viewportOffset.x,
	//	camera.viewportOffset.y,
	//	camera.width * scale,
	//	camera.height * scale
	//);
	//renderer->setViewport(viewport);

	// MRT�w��
	ID3D11RenderTargetView* rtvs[3] = { 
		renderer->m_gbuffer.m_diffuseRTV.Get(), 
		renderer->m_gbuffer.m_normalRTV.Get(),
		renderer->m_gbuffer.m_positionRTV.Get()};
	auto* dsv = static_cast<D3D11DepthStencil*>(renderer->getDepthStencil(camera.depthStencilID));
	renderer->m_d3dContext->OMSetRenderTargets(3, rtvs, dsv->m_dsv.Get());
	// GBuffer�N���A
	renderer->m_d3dContext->ClearRenderTargetView(rtvs[0], DirectX::Colors::Black);
	renderer->m_d3dContext->ClearRenderTargetView(rtvs[1], DirectX::Colors::Black);
	renderer->m_d3dContext->ClearRenderTargetView(rtvs[2], DirectX::Colors::Black);

	// �o�b�`�`��
	for (auto& bitch : m_batchList)
	{
		auto* pBitch = renderer->getBatchGroup(bitch.first);
		const auto* mat = renderer->getMaterial(pBitch->m_materialID);
		const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, pBitch->m_meshID);

		renderer->setD3D11Material(pBitch->m_materialID);
		renderer->setD3D11RenderBuffer(rdID);

		// �ő�1000�܂Ńo�b�`�`��
		const std::size_t count = bitch.second.size();
		const std::size_t maxNum = SHADER::MAX_TRANSFORM_MATRIX_COUNT;

		for (std::size_t i = 0; i < count / maxNum + 1; ++i)
		{
			std::size_t drawNum = maxNum;
			if ((count - i * maxNum) / maxNum <= 0)
			{
				drawNum = count % maxNum;
			}

			renderer->setD3DTransformBuffer(bitch.second.data() + i * maxNum, drawNum);
			renderer->d3dRender(rdID, drawNum);
		}
	}

	// �s�����`��
	for (auto& opeque : m_deferredList)
	{
		// �{�[���X�V
		if (opeque.isBone)
		{
			auto* pMesh = renderer->getMesh(opeque.meshID);
			updateBoneMatrix(goMgr, opeque.rootID, *pMesh);
			renderer->setD3DAnimationBuffer(pMesh->m_calcBoneBuffer);
			//if (opeque.rootID != NONE_GAME_OBJECT_ID)
			//{
			//	auto root = goMgr->getComponentData<Transform>(opeque.rootID);
			//	if (root) opeque.worldMatrix = root->globalMatrix;
			//}
		}
		renderer->setD3D11Material(opeque.materialID);
		renderer->setD3DTransformBuffer(&opeque.worldMatrix, 1);
		renderer->setD3D11RenderBuffer(opeque.renderBufferID);
		renderer->d3dRender(opeque.renderBufferID);
	}
}

void RenderPipeline::shadowPass(Camera& camera)
{

}

void RenderPipeline::opaquePass(Camera& camera)
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());
	auto* goMgr = m_pWorld->getGameObjectManager();

	//// �r���[�|�[�g�w��
	//float scale = 1.0f;
	//if (camera.viewportScale > 0.0f)
	//	scale = camera.viewportScale;
	//Viewport viewport(
	//	camera.viewportOffset.x,
	//	camera.viewportOffset.y,
	//	camera.width * scale,
	//	camera.height * scale
	//);
	//renderer->setViewport(viewport);


	//----- �f�t�@�[�h�����_�����O -----
	// �����_�[�^�[�Q�b�g�w��
	renderer->setRenderTarget(camera.renderTargetID, NONE_DEPTH_STENCIL_ID);

	// �}�e���A���w��
	renderer->setD3D11Material(m_defferdLitMat);

	// �e�N�X�`���w��
	renderer->setD3D11ShaderResourceView(0, renderer->m_gbuffer.m_diffuseSRV.Get(), ShaderStage::PS);
	renderer->setD3D11ShaderResourceView(1, renderer->m_gbuffer.m_normalSRV.Get(), ShaderStage::PS);
	renderer->setD3D11ShaderResourceView(2, renderer->m_gbuffer.m_positionSRV.Get(), ShaderStage::PS);
	auto ds = static_cast<D3D11DepthStencil*>(renderer->getDepthStencil(camera.depthStencilID));
	renderer->setD3D11ShaderResourceView(3, ds->m_srv.Get(), ShaderStage::PS);

	// �����_�[�o�b�t�@�w��
	renderer->setD3D11RenderBuffer(m_quadRb);

	// �g�����X�t�H�[���w��
	Vector3 windowSize = Vector3(engine->getWindowWidth(), engine->getWindowHeight(), 1);
	Matrix matrix = Matrix::CreateScale(windowSize);
	//matrix *= Matrix::CreateRotationY(XMConvertToDegrees(90));
	renderer->setD3DTransformBuffer(&matrix, 1);
	
	auto skytexID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(SHADER::SHADER_SRV_SLOT_SKYDOOM, skytexID, ShaderStage::PS);
	renderer->setD3D11Sampler(SHADER::SHADER_SS_SLOT_SKYBOX, SamplerState::ANISOTROPIC_WRAP, ShaderStage::PS);

	// �`��
	renderer->d3dRender(m_quadRb);


	//----- �t�H���[�h�����_�����O -----
	// �����_�[�^�[�Q�b�g�w��
	renderer->setRenderTarget(camera.renderTargetID, camera.depthStencilID);

	// �s�����`��
	for (auto& opeque : m_opequeList)
	{
		// �{�[���X�V
		if (opeque.isBone)
		{
			auto* pMesh = renderer->getMesh(opeque.meshID);
			updateBoneMatrix(goMgr ,opeque.rootID, *pMesh);
			renderer->setD3DAnimationBuffer(pMesh->m_calcBoneBuffer);
		}
		renderer->setD3D11Material(opeque.materialID);
		renderer->setD3DTransformBuffer(&opeque.worldMatrix, 1);
		renderer->setD3D11RenderBuffer(opeque.renderBufferID);
		renderer->d3dRender(opeque.renderBufferID);
	}

	/*ImGui::Begin("RenderImage");

	ImGui::Image(renderer->m_gbuffer.m_normalSRV.Get(), ImVec2(1920 * 0.25, 1080 * 0.25));
	ImGui::Image(renderer->m_gbuffer.m_diffuseSRV.Get(), ImVec2(1920 * 0.25, 1080 * 0.25));
	ImGui::Image(renderer->m_gbuffer.m_positionSRV.Get(), ImVec2(1920 * 0.25, 1080 * 0.25));

	ImGui::End();*/
}

void RenderPipeline::skyPass(Camera& camera)
{

}

void RenderPipeline::transparentPass(Camera& camera)
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());
	auto* goMgr = m_pWorld->getGameObjectManager();

	// �����_�[�^�[�Q�b�g�R�s�[
	renderer->copyRenderTarget(m_renderTarget, camera.renderTargetID);
	renderer->d3dGenerateMips(m_renderTarget);

	// �������`��
	for (auto& opeque : m_transparentList)
	{
		// �{�[���X�V
		if (opeque.isBone)
		{
			auto* pMesh = renderer->getMesh(opeque.meshID);
			updateBoneMatrix(goMgr, opeque.rootID, *pMesh);
			renderer->setD3DAnimationBuffer(pMesh->m_calcBoneBuffer);
		}
		renderer->setD3D11Material(opeque.materialID);
		renderer->setD3DTransformBuffer(&opeque.worldMatrix, 1);
		renderer->setD3D11RenderBuffer(opeque.renderBufferID);
		renderer->d3dRender(opeque.renderBufferID);
	}
}

void RenderPipeline::postPass(Camera& camera)
{
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());
	auto* goMgr = m_pWorld->getGameObjectManager();

	// �����_�[�^�[�Q�b�g�R�s�[
	renderer->copyRenderTarget(m_renderTarget, camera.renderTargetID);
	renderer->d3dGenerateMips(m_renderTarget);

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
	// ���s���e
	camera.projection2d = Matrix::CreateOrthographic(
		camera.width, camera.height, camera.nearZ, camera.farZ);
	// �������e
	camera.projection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(camera.fovY), camera.aspect, camera.nearZ, camera.farZ);

}

void RenderPipeline::updateBoneMatrix(GameObjectManager* goMgr ,const GameObjectID& goID, Mesh& mesh)
{
	auto* go = goMgr->getGameObject(goID);
	if (go)
	{
		auto itr = mesh.m_boneTabel.find(go->getName().data());
		if (mesh.m_boneTabel.end() != itr)
		{
			auto* transform = goMgr->getComponentData<ecs::Transform>(goID);
			mesh.m_calcBoneBuffer[itr->second] = (mesh.m_invMatrixData[itr->second] * 
				transform->globalMatrix).Transpose();	// �|�u
		}
		// �q�m�[�h
		for (auto childID : goMgr->GetChilds(goID))
		{
			updateBoneMatrix(goMgr, childID, mesh);
		}
	}
}
