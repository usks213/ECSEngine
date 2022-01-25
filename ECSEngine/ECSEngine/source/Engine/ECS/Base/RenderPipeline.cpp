/*****************************************************************//**
 * \file   RenderPipeline.h
 * \brief  レンダーパイプライン
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


/// @brief 生成時
void RenderPipeline::onCreate()
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// シャドウ解像度
	m_shadowMapSize = Vector2(2048, 2048);

	// 最終描画バッファ
	m_renderTarget = renderer->createRenderTarget("RenderPipeline");

	// シェーダー読み込み
	ShaderDesc desc;
	desc.m_name = "DeferredLit";
	desc.m_stages = ShaderStageFlags::VS | ShaderStageFlags::PS;
	auto defferdLitShader = renderer->createShader(desc);
	m_defferdLitMat = renderer->createMaterial("DeferredLit", defferdLitShader);
	auto mat = renderer->getMaterial(m_defferdLitMat);
	//mat->m_rasterizeState = RasterizeState::CULL_NONE;
	mat->m_depthStencilState = DepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE;

	// 深度パスシェーダー
	desc.m_name = "DepthWrite";
	desc.m_stages = 0 | ShaderStageFlags::VS;
	m_depthWriteShader = renderer->createShader(desc);
	m_depthWriteMat = renderer->createMaterial("DepthWrite", m_depthWriteShader);

	auto quadMesh = renderer->createMesh("Quad");
	auto* pQuad = renderer->getMesh(quadMesh);
	Geometry::Quad(*pQuad);
	m_quadRb = renderer->createRenderBuffer(defferdLitShader ,quadMesh);

}

/// @brief 削除時
void RenderPipeline::onDestroy()
{

}

/// @brief 更新
void RenderPipeline::onUpdate()
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// パイプラインデータ取得
	PipelineData pipelineData = GetPipelineData();

	// カリング
	cullingPass(renderer, pipelineData);

	// 描画
	beginPass(renderer, pipelineData);
	prePass(renderer, pipelineData);
	gbufferPass(renderer, pipelineData);
	shadowPass(renderer, pipelineData);
	opaquePass(renderer, pipelineData);
	skyPass(renderer, pipelineData);
	transparentPass(renderer, pipelineData);
	postPass(renderer, pipelineData);
	//endPass(renderer, pipelineData);

	renderer->copyRenderTarget(m_renderTarget, pipelineData.camera->renderTargetID);

	//// バックバッファに反映
	//auto* rt = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(mainCamera->renderTargetID));
	//renderer->d3dCopyResource(renderer->m_gbuffer.m_diffuseRT.Get(), rt->m_tex.Get());
}

void RenderPipeline::cullingPass(RendererManager* renderer, const PipelineData& data)
{
	m_batchList.clear();
	m_opequeList.clear();
	m_transparentList.clear();
	m_deferredList.clear();
	m_pointLights.clear();
	m_spotLights.clear();
	m_shadowList.clear();

	// カメラフラスタム
	Frustum cameraFrustum(data.camera->farZ, data.camera->view, data.camera->projection);
	Camera* camera = data.camera;
	// シャドウフラスタム
	Frustum shadowFrustum(data.directionalLight->camera.farZ, data.directionalLight->camera.view, data.directionalLight->camera.projection);

	// バッチデータ
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
				// カメラカリング
				//if (cameraFrustum.CheckAABB(aabb))
				{
					if (mat->m_shaderType == ShaderType::Forward)
					{
						RenderingData data;
						data.materialID = bitchID.second->m_materialID;
						data.renderBufferID = renderer->createRenderBuffer(mat->m_shaderID, bitchID.second->m_meshID);
						data.worldMatrix = transform[i].globalMatrix.Transpose();
						data.cameraLengthSqr = (camera->world.Translation() -
							transform[i].globalMatrix.Translation()).LengthSquared();
						// フォワード
						if (mat->m_isTransparent)
						{
							// 半透明
							m_transparentList.push_back(data);
						}
						else
						{
							// 不透明
							m_opequeList.push_back(data);
						}
					}
					else if(mat->m_shaderType == ShaderType::Deferred)
					{
						// デファード
						m_batchList[bitchID.first].push_back(transform[i].globalMatrix.Transpose());
					}
				}
				// シャドウカリング
				if (shadowFrustum.CheckAABB(aabb))
				{
					m_shadowList[mesh->m_id].push_back(transform[i].globalMatrix.Transpose());
				}
			}
		}
	}

	// オブジェクトデータ
	foreach<RenderData, Transform>(
		[&](RenderData& rd, Transform& transform)
		{
			const auto* mat = renderer->getMaterial(rd.materialID);
			const auto* mesh = renderer->getMesh(rd.meshID);
			if (!mat || !mesh) return;
			const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, rd.meshID);

			AABB aabb;
			AABB::transformAABB(transform.globalMatrix, mesh->m_aabb, aabb);
			// カメラカリング
			//if (cameraFrustum.CheckAABB(aabb))
			{
				RenderingData data;
				data.materialID = rd.materialID;
				data.renderBufferID = rdID;
				data.worldMatrix = transform.globalMatrix.Transpose();
				data.cameraLengthSqr = (camera->world.Translation() -
					transform.globalMatrix.Translation()).LengthSquared();
				data.isBone = false;
				data.meshID = rd.meshID;
				data.rootID = NONE_GAME_OBJECT_ID;

				if (mat->m_shaderType == ShaderType::Forward)
				{
					// フォワード
					if (mat->m_isTransparent)
					{
						// 半透明
						m_transparentList.push_back(data);
					}
					else
					{
						// 不透明
						m_opequeList.push_back(data);
					}
				}
				else if (mat->m_shaderType == ShaderType::Deferred)
				{
					// 不透明(デファード)
					m_deferredList.push_back(data);
				}
			}
			// シャドウカリング
			if (shadowFrustum.CheckAABB(aabb))
			{
				m_shadowList[mesh->m_id].push_back(transform.globalMatrix.Transpose());
			}
		});

	// スキンメッシュ
	foreach<SkinnedMeshRenderer, Transform>(
		[&](SkinnedMeshRenderer& rd, Transform& transform)
		{
			const auto* mat = renderer->getMaterial(rd.materialID);
			const auto* mesh = renderer->getMesh(rd.meshID);
			if (!mat || !mesh) return;
			const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, rd.meshID);

			AABB aabb;
			AABB::transformAABB(transform.globalMatrix, mesh->m_aabb, aabb);
			// カメラカリング
			//if (cameraFrustum.CheckAABB(aabb))
			{
				RenderingData data;
				data.materialID = rd.materialID;
				data.renderBufferID = rdID;
				data.worldMatrix = transform.globalMatrix.Transpose();
				data.cameraLengthSqr = (camera->world.Translation() -
					transform.globalMatrix.Translation()).LengthSquared();
				data.isBone = true;
				data.meshID = rd.meshID;
				data.rootID = rd.rootObjectID;

				if (mat->m_shaderType == ShaderType::Forward)
				{
					// フォワード
					if (mat->m_isTransparent)
					{
						// 半透明
						m_transparentList.push_back(data);
					}
					else
					{
						// 不透明
						m_opequeList.push_back(data);
					}
				}
				else if (mat->m_shaderType == ShaderType::Deferred)
				{
					// 不透明(デファード)
					m_deferredList.push_back(data);
				}
			}
			// シャドウカリング
			if (shadowFrustum.CheckAABB(aabb))
			{
				m_shadowList[mesh->m_id].push_back(transform.globalMatrix.Transpose());
			}
		});


	// 不透明のソート カメラから近い順(昇順)
	std::sort(m_opequeList.begin(), m_opequeList.end(), [](RenderingData& l, RenderingData& r) {
		return l.cameraLengthSqr < r.cameraLengthSqr;
		});
	std::sort(m_deferredList.begin(), m_deferredList.end(), [](RenderingData& l, RenderingData& r) {
		return l.cameraLengthSqr < r.cameraLengthSqr;
		});

	// シャドウカメラから近い順にソート
	Vector3 shadowPos = data.directionalLight->camera.world.Translation();
	for (auto& meshes : m_shadowList)
	{
		std::sort(meshes.second.begin(), meshes.second.end(),
			[&shadowPos](const Matrix& l, const Matrix& r)
			{
				return (l.Translation() - shadowPos).LengthSquared() <
					(r.Translation() - shadowPos).LengthSquared();
			});
	}

	// 半透明のソート カメラから遠い順(降順)
	std::sort(m_transparentList.begin(), m_transparentList.end(), [](RenderingData& l, RenderingData& r) {
		return l.cameraLengthSqr > r.cameraLengthSqr;
		});


	//----- ライトカリング

	// ポイントライト
	foreach<PointLight, Transform>(
		[&](PointLight& point, Transform& transform)
		{
			// 座標データ
			point.data.position = transform.globalMatrix.Translation();

			// カメラカリング
			//if (cameraFrustum.CheckSphere(aabb))
			{
				m_pointLights.push_back(point.data);
			}
		});

	// スポットライト
	foreach<SpotLight, Transform>(
		[&](SpotLight& spot, Transform& transform)
		{
			// 座標・向き
			spot.data.position = transform.globalMatrix.Translation();
			spot.data.direction = transform.globalMatrix.Forward();

			// カメラカリング
			//if (cameraFrustum.Check(aabb))
			{
				m_spotLights.push_back(spot.data);
			}
		});

}

void RenderPipeline::beginPass(RendererManager* renderer, const PipelineData& data)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* d3drenderer = static_cast<D3D11RendererManager*>(renderer);

	// ライトバッファの設定
	d3drenderer->setD3DLightBuffer(m_pointLights, m_spotLights);

	// ディレクショナルライト
	DirectionalLightData dirLit;
	if (data.directionalLight) {
		dirLit = data.directionalLight->data;
	}
	else {
		dirLit.ambient = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
		dirLit.color = Vector4();
		dirLit.direction = Vector4();
	}

	// システムバッファの設定
	SHADER::SystemBuffer sysBuffer;
	// メインカメラ
	{
		sysBuffer._mainCamera._mView = data.camera->view.Transpose();
		if (data.camera->isOrthographic)
			sysBuffer._mainCamera._mProj = data.camera->projection2d.Transpose();
		else
			sysBuffer._mainCamera._mProj = data.camera->projection.Transpose();
		sysBuffer._mainCamera._mProj2D = data.camera->projection2d.Transpose();
		sysBuffer._mainCamera._mViewInv = data.camera->view.Invert().Transpose();
		sysBuffer._mainCamera._mProjInv = data.camera->projection.Invert().Transpose();
		sysBuffer._mainCamera._viewPos = Vector4(data.camera->world.Translation());
	}
	// シャドウカメラ
	{
		sysBuffer._shadowCamera._mView = data.directionalLight->camera.view.Transpose();
		if (data.directionalLight->camera.isOrthographic)
			sysBuffer._shadowCamera._mProj = data.directionalLight->camera.projection2d.Transpose();
		else
			sysBuffer._shadowCamera._mProj = data.directionalLight->camera.projection.Transpose();
		sysBuffer._shadowCamera._mProj2D = data.directionalLight->camera.projection2d.Transpose();
		sysBuffer._shadowCamera._mViewInv = data.directionalLight->camera.view.Invert().Transpose();
		sysBuffer._shadowCamera._mProjInv = data.directionalLight->camera.projection.Invert().Transpose();
		sysBuffer._shadowCamera._viewPos = Vector4(data.directionalLight->camera.world.Translation());
	}
	// ライトデータ
	sysBuffer._directionalLit = dirLit;
	sysBuffer._pointLightNum = m_pointLights.size();
	sysBuffer._spotLightNum = m_spotLights.size();
	sysBuffer._time = engine->getCurrentTime();
	sysBuffer._frame = engine->getFrameCount();
	d3drenderer->setD3DSystemBuffer(sysBuffer);

	// レンダーターゲットクリア
	renderer->clearRenderTarget(data.camera->renderTargetID);
	renderer->clearDepthStencil(data.camera->depthStencilID);
	renderer->clearRenderTarget(data.directionalLight->camera.renderTargetID);
	renderer->clearDepthStencil(data.directionalLight->camera.depthStencilID);

	// グラブテクスチャセット
	auto pGrab = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(m_renderTarget));
	d3drenderer->m_d3dContext->PSSetShaderResources(SHADER::SHADER_SRV_SLOT_GRABTEX, 1, pGrab->m_srv.GetAddressOf());
}

void RenderPipeline::prePass(RendererManager* renderer, const PipelineData& data)
{
	auto* d3drenderer = static_cast<D3D11RendererManager*>(renderer);

	// レンダーターゲットの切り替え
	RenderTargetID RTid = 0;
	renderer->setRenderTarget(RTid, data.directionalLight->camera.depthStencilID);

	// マテリアルの指定
	renderer->setMaterial(m_depthWriteMat);

	// メッシュごとにインスタンシングで描画
	for (const auto& meshes : m_shadowList)
	{
		// レンダーバッファ作成
		auto rdID = renderer->createRenderBuffer(m_depthWriteShader, meshes.first);
		// メッシュ指定
		renderer->getRenderBuffer(rdID);

		// 最大1000個までバッチ描画
		const std::size_t count = meshes.second.size();
		const std::size_t maxNum = SHADER::MAX_TRANSFORM_MATRIX_COUNT;

		// インスタンシング描画
		for (std::size_t i = 0; i < count / maxNum + 1; ++i)
		{
			std::size_t drawNum = maxNum;
			if ((count - i * maxNum) / maxNum <= 0)
			{
				drawNum = count % maxNum;
			}

			d3drenderer->setD3DTransformBuffer(meshes.second.data() + i * maxNum, drawNum);
			d3drenderer->d3dRender(rdID, drawNum);
		}
	}
}

void RenderPipeline::gbufferPass(RendererManager* renderer, const PipelineData& data)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* d3drenderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());
	auto* goMgr = m_pWorld->getGameObjectManager();

	// ビューポート指定
	Viewport viewport(
		0,
		0,
		data.camera->width,
		data.camera->height
	);
	renderer->setViewport(viewport);

	//// ビューポート指定
	//float scale = 1.0f;
	//if (data.camera->viewportScale > 0.0f)
	//	scale = data.camera->viewportScale;
	//Viewport viewport(
	//	data.camera->viewportOffset.x,
	//	data.camera->viewportOffset.y,
	//	data.camera->width * scale,
	//	data.camera->height * scale
	//);
	//renderer->setViewport(viewport);

	// MRT指定
	ID3D11RenderTargetView* rtvs[3] = { 
		d3drenderer->m_gbuffer.m_diffuseRTV.Get(),
		d3drenderer->m_gbuffer.m_normalRTV.Get(),
		d3drenderer->m_gbuffer.m_positionRTV.Get()};
	auto* dsv = static_cast<D3D11DepthStencil*>(d3drenderer->getDepthStencil(data.camera->depthStencilID));
	d3drenderer->m_d3dContext->OMSetRenderTargets(3, rtvs, dsv->m_dsv.Get());
	// GBufferクリア
	d3drenderer->m_d3dContext->ClearRenderTargetView(rtvs[0], DirectX::Colors::Black);
	d3drenderer->m_d3dContext->ClearRenderTargetView(rtvs[1], DirectX::Colors::Black);
	d3drenderer->m_d3dContext->ClearRenderTargetView(rtvs[2], DirectX::Colors::Black);

	// バッチ描画
	for (auto& bitch : m_batchList)
	{
		auto* pBitch = d3drenderer->getBatchGroup(bitch.first);
		const auto* mat = d3drenderer->getMaterial(pBitch->m_materialID);
		const auto& rdID = d3drenderer->createRenderBuffer(mat->m_shaderID, pBitch->m_meshID);

		d3drenderer->setD3D11Material(pBitch->m_materialID);
		d3drenderer->setD3D11RenderBuffer(rdID);

		// 最大1000個までバッチ描画
		const std::size_t count = bitch.second.size();
		const std::size_t maxNum = SHADER::MAX_TRANSFORM_MATRIX_COUNT;

		for (std::size_t i = 0; i < count / maxNum + 1; ++i)
		{
			std::size_t drawNum = maxNum;
			if ((count - i * maxNum) / maxNum <= 0)
			{
				drawNum = count % maxNum;
			}

			d3drenderer->setD3DTransformBuffer(bitch.second.data() + i * maxNum, drawNum);
			d3drenderer->d3dRender(rdID, drawNum);
		}
	}

	// 不透明描画
	for (auto& opeque : m_deferredList)
	{
		// ボーン更新
		if (opeque.isBone)
		{
			auto* pMesh = d3drenderer->getMesh(opeque.meshID);
			updateBoneMatrix(goMgr, opeque.rootID, *pMesh);
			d3drenderer->setD3DAnimationBuffer(pMesh->m_calcBoneBuffer);
			//if (opeque.rootID != NONE_GAME_OBJECT_ID)
			//{
			//	auto root = goMgr->getComponentData<Transform>(opeque.rootID);
			//	if (root) opeque.worldMatrix = root->globalMatrix;
			//}
		}
		d3drenderer->setD3D11Material(opeque.materialID);
		d3drenderer->setD3DTransformBuffer(&opeque.worldMatrix, 1);
		d3drenderer->setD3D11RenderBuffer(opeque.renderBufferID);
		d3drenderer->d3dRender(opeque.renderBufferID);
	}
}

void RenderPipeline::shadowPass(RendererManager* renderer, const PipelineData& data)
{

}

void RenderPipeline::opaquePass(RendererManager* renderer, const PipelineData& data)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* d3drenderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());
	auto* goMgr = m_pWorld->getGameObjectManager();

	//// ビューポート指定
	//float scale = 1.0f;
	//if (data.camera->viewportScale > 0.0f)
	//	scale = data.camera->viewportScale;
	//Viewport viewport(
	//	data.camera->viewportOffset.x,
	//	data.camera->viewportOffset.y,
	//	data.camera->width * scale,
	//	data.camera->height * scale
	//);
	//renderer->setViewport(viewport);


	//----- デファードレンダリング -----
	// レンダーターゲット指定
	renderer->setRenderTarget(data.camera->renderTargetID, NONE_DEPTH_STENCIL_ID);

	// マテリアル指定
	d3drenderer->setD3D11Material(m_defferdLitMat);

	// テクスチャ指定
	d3drenderer->setD3D11ShaderResourceView(0, d3drenderer->m_gbuffer.m_diffuseSRV.Get(), ShaderStage::PS);
	d3drenderer->setD3D11ShaderResourceView(1, d3drenderer->m_gbuffer.m_normalSRV.Get(), ShaderStage::PS);
	d3drenderer->setD3D11ShaderResourceView(2, d3drenderer->m_gbuffer.m_positionSRV.Get(), ShaderStage::PS);
	auto ds = static_cast<D3D11DepthStencil*>(d3drenderer->getDepthStencil(data.camera->depthStencilID));
	d3drenderer->setD3D11ShaderResourceView(3, ds->m_srv.Get(), ShaderStage::PS);

	// レンダーバッファ指定
	d3drenderer->setD3D11RenderBuffer(m_quadRb);

	// トランスフォーム指定
	Vector3 windowSize = Vector3(engine->getWindowWidth(), engine->getWindowHeight(), 1);
	Matrix matrix = Matrix::CreateScale(windowSize);
	//matrix *= Matrix::CreateRotationY(XMConvertToDegrees(90));
	d3drenderer->setD3DTransformBuffer(&matrix, 1);
	
	auto skytexID = d3drenderer->createTextureFromFile("data/texture/environment.hdr");
	d3drenderer->setTexture(SHADER::SHADER_SRV_SLOT_SKYDOOM, skytexID, ShaderStage::PS);
	d3drenderer->setD3D11Sampler(SHADER::SHADER_SS_SLOT_SKYBOX, SamplerState::ANISOTROPIC_WRAP, ShaderStage::PS);

	// 描画
	d3drenderer->d3dRender(m_quadRb);


	//----- フォワードレンダリング -----
	// レンダーターゲット指定
	d3drenderer->setRenderTarget(data.camera->renderTargetID, data.camera->depthStencilID);

	// 不透明描画
	for (auto& opeque : m_opequeList)
	{
		// ボーン更新
		if (opeque.isBone)
		{
			auto* pMesh = d3drenderer->getMesh(opeque.meshID);
			updateBoneMatrix(goMgr ,opeque.rootID, *pMesh);
			d3drenderer->setD3DAnimationBuffer(pMesh->m_calcBoneBuffer);
		}
		d3drenderer->setD3D11Material(opeque.materialID);
		d3drenderer->setD3DTransformBuffer(&opeque.worldMatrix, 1);
		d3drenderer->setD3D11RenderBuffer(opeque.renderBufferID);
		d3drenderer->d3dRender(opeque.renderBufferID);
	}

	/*ImGui::Begin("RenderImage");

	ImGui::Image(d3drenderer->m_gbuffer.m_normalSRV.Get(), ImVec2(1920 * 0.25, 1080 * 0.25));
	ImGui::Image(d3drenderer->m_gbuffer.m_diffuseSRV.Get(), ImVec2(1920 * 0.25, 1080 * 0.25));
	ImGui::Image(d3drenderer->m_gbuffer.m_positionSRV.Get(), ImVec2(1920 * 0.25, 1080 * 0.25));

	ImGui::End();*/
}

void RenderPipeline::skyPass(RendererManager* renderer, const PipelineData& data)
{

}

void RenderPipeline::transparentPass(RendererManager* renderer, const PipelineData& data)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* d3drenderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());
	auto* goMgr = m_pWorld->getGameObjectManager();

	// レンダーターゲットコピー
	d3drenderer->copyRenderTarget(m_renderTarget, data.camera->renderTargetID);
	d3drenderer->d3dGenerateMips(m_renderTarget);

	// 半透明描画
	for (auto& opeque : m_transparentList)
	{
		// ボーン更新
		if (opeque.isBone)
		{
			auto* pMesh = d3drenderer->getMesh(opeque.meshID);
			updateBoneMatrix(goMgr, opeque.rootID, *pMesh);
			d3drenderer->setD3DAnimationBuffer(pMesh->m_calcBoneBuffer);
		}
		d3drenderer->setD3D11Material(opeque.materialID);
		d3drenderer->setD3DTransformBuffer(&opeque.worldMatrix, 1);
		d3drenderer->setD3D11RenderBuffer(opeque.renderBufferID);
		d3drenderer->d3dRender(opeque.renderBufferID);
	}
}

void RenderPipeline::postPass(RendererManager* renderer, const PipelineData& data)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* d3drenderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());
	auto* goMgr = m_pWorld->getGameObjectManager();

	// レンダーターゲットコピー
	d3drenderer->copyRenderTarget(m_renderTarget, data.camera->renderTargetID);
	d3drenderer->d3dGenerateMips(m_renderTarget);

}

void RenderPipeline::endPass(RendererManager* renderer, const PipelineData& data)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* d3drenderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// バックバッファに反映
	auto* rt = static_cast<D3D11RenderTarget*>(d3drenderer->getRenderTarget(data.camera->renderTargetID));
	d3drenderer->d3dCopyResource(d3drenderer->m_backBufferRT.Get(), rt->m_tex.Get());
}

void renderingPass()
{
	//// レンダラーマネージャー
	//auto* engine = m_pWorld->getWorldManager()->getEngine();
	//auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());


	//// バッチ描画
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


	//// オブジェクト描画
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

RenderPipeline::PipelineData RenderPipeline::GetPipelineData()
{
	auto* engine = m_pWorld->getWorldManager()->getEngine();

	// カメラ設定
	Camera* mainCamera = nullptr;
	foreach<Camera, Transform>(
		[&mainCamera, &engine](Camera& camera, Transform& transform)
		{
			// カメラ更新
			updateCamera(camera, transform, engine->getWindowWidth(), engine->getWindowHeight());
			// メインカメラ
			mainCamera = &camera;
		});

	// ディレクショナルライト
	Vector2 shadowMapSize = m_shadowMapSize;
	DirectionalLight* mainLit = nullptr;
	foreach<DirectionalLight, Transform>(
		[&mainLit, &shadowMapSize](DirectionalLight& lit, Transform& transform)
		{
			// 向きの更新
			lit.data.direction = Vector4(transform.globalMatrix.Forward());
			// シャドウカメラ更新
			updateCamera(lit.camera, transform, shadowMapSize.x, shadowMapSize.y);
			// メインライト
			mainLit = &lit;
		});

	// パイプラインデータ
	PipelineData pipelineData;
	pipelineData.camera = mainCamera;
	pipelineData.directionalLight = mainLit;

	return pipelineData;
}


inline void RenderPipeline::updateCamera(Camera& camera, Transform& transform, float width, float height)
{
	// ワールドマトリックス更新
	camera.world = transform.globalMatrix;

	// ビューマトリックス更新
	Vector3 pos = camera.world.Translation();
	Vector3 target = pos + camera.world.Forward();
	Vector3 up = camera.world.Up();
	camera.view = Matrix::CreateLookAt(pos, target, up);

	// ウィンドウサイズ
	camera.width = width;
	camera.height = height;
	camera.aspect = camera.width / camera.height;

	// プロジェクションマトリックス更新
	// 並行投影
	camera.projection2d = Matrix::CreateOrthographic(
		camera.width, camera.height, camera.nearZ, camera.farZ);
	// 透視投影
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
				transform->globalMatrix).Transpose();	// 倒置
		}
		// 子ノード
		for (auto childID : goMgr->GetChilds(goID))
		{
			updateBoneMatrix(goMgr, childID, mesh);
		}
	}
}

