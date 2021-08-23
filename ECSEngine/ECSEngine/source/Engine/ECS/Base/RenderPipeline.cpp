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

	// カメラなし
	if (mainCamera == nullptr) return;

	// ディレクショナルライト
	

	// カリング
	cullingPass(*mainCamera);

	// 描画
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

	//// バックバッファに反映
	//auto* rt = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(mainCamera->renderTargetID));
	//renderer->d3dCopyResource(renderer->m_gbuffer.m_diffuseRT.Get(), rt->m_tex.Get());
}

void RenderPipeline::beginPass(Camera& camera)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// ディレクショナルライト
	DirectionalLightData dirLit;
	dirLit.ambient = Vector4(0.3f, 0.3f, 0.3f, 0.3f);
	dirLit.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	dirLit.direction = Vector4(1.f, -1.5f, -1.f, 1.0f);

	// システムバッファの設定
	D3D::SystemBuffer sysBuffer;
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
	renderer->setD3DSystemBuffer(sysBuffer);

	// レンダーターゲットクリア
	renderer->clearRenderTarget(camera.renderTargetID);
	renderer->clearDepthStencil(camera.depthStencilID);

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

	// カメラフラスタム
	Frustum cameraFrustum(camera.farZ, camera.view, camera.projection);


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
						data.worldMatrix = transform[i].globalMatrix;
						data.cameraLengthSqr = (camera.world.Translation() -
							transform[i].globalMatrix.Translation()).LengthSquared();
						// フォワード
						m_opequeList.push_back(data);
					}
					else if(mat->m_shaderType == ShaderType::Deferred)
					{
						// デファード
						m_batchList[bitchID.first].push_back(transform[i].globalMatrix);
					}
				}
				// シャドウカリング

			}
		}
	}

	// オブジェクトデータ
	foreach<RenderData, Transform>(
		[&](RenderData& rd, Transform& transform)
		{
			const auto* mat = renderer->getMaterial(rd.materialID);
			const auto* mesh = renderer->getMesh(rd.meshID);
			const auto& rdID = renderer->createRenderBuffer(mat->m_shaderID, rd.meshID);

			AABB aabb;
			AABB::transformAABB(transform.globalMatrix, mesh->m_aabb, aabb);
			// カメラカリング
			//if (cameraFrustum.CheckAABB(aabb))
			{
				RenderingData data;
				data.materialID = rd.materialID;
				data.renderBufferID = rdID;
				data.worldMatrix = transform.globalMatrix;
				data.cameraLengthSqr = (camera.world.Translation() -
					transform.globalMatrix.Translation()).LengthSquared();

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

		});


	// 不透明のソート カメラから近い順(昇順)
	std::sort(m_opequeList.begin(), m_opequeList.end(), [](RenderingData& l, RenderingData& r) {
		return l.cameraLengthSqr < r.cameraLengthSqr;
		});
	std::sort(m_deferredList.begin(), m_deferredList.end(), [](RenderingData& l, RenderingData& r) {
		return l.cameraLengthSqr < r.cameraLengthSqr;
		});

	// 半透明のソート カメラから遠い順(降順)
	std::sort(m_transparentList.begin(), m_transparentList.end(), [](RenderingData& l, RenderingData& r) {
		return l.cameraLengthSqr > r.cameraLengthSqr;
		});

}

void RenderPipeline::prePass(Camera& camera)
{

}

void RenderPipeline::gbufferPass(Camera& camera)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// ビューポート指定
	Viewport viewport(
		0,
		0,
		camera.width,
		camera.height
	);
	renderer->setViewport(viewport);

	//// ビューポート指定
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

	// MRT指定
	ID3D11RenderTargetView* rtvs[3] = { 
		renderer->m_gbuffer.m_diffuseRTV.Get(), 
		renderer->m_gbuffer.m_normalRTV.Get(),
		renderer->m_gbuffer.m_positionRTV.Get()};
	auto* dsv = static_cast<D3D11DepthStencil*>(renderer->getDepthStencil(camera.depthStencilID));
	renderer->m_d3dContext->OMSetRenderTargets(3, rtvs, dsv->m_dsv.Get());
	// GBufferクリア
	renderer->m_d3dContext->ClearRenderTargetView(rtvs[0], DirectX::Colors::Black);
	renderer->m_d3dContext->ClearRenderTargetView(rtvs[1], DirectX::Colors::Black);
	renderer->m_d3dContext->ClearRenderTargetView(rtvs[2], DirectX::Colors::Black);

	// バッチ描画
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

	// 不透明描画
	for (auto& opeque : m_deferredList)
	{
		renderer->setD3D11Material(opeque.materialID);
		renderer->setD3DTransformBuffer(opeque.worldMatrix);
		renderer->setD3D11RenderBuffer(opeque.renderBufferID);
		renderer->d3dRender(opeque.renderBufferID);
	}
}

void RenderPipeline::shadowPass(Camera& camera)
{

}

void RenderPipeline::opaquePass(Camera& camera)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	//// ビューポート指定
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


	//----- デファードレンダリング -----
	// レンダーターゲット指定
	renderer->setRenderTarget(camera.renderTargetID, NONE_DEPTH_STENCIL_ID);

	// マテリアル指定
	renderer->setD3D11Material(m_defferdLitMat);

	// テクスチャ指定
	renderer->setD3D11ShaderResourceView(0, renderer->m_gbuffer.m_diffuseSRV.Get(), ShaderStage::PS);
	renderer->setD3D11ShaderResourceView(1, renderer->m_gbuffer.m_normalSRV.Get(), ShaderStage::PS);
	renderer->setD3D11ShaderResourceView(2, renderer->m_gbuffer.m_positionSRV.Get(), ShaderStage::PS);
	auto ds = static_cast<D3D11DepthStencil*>(renderer->getDepthStencil(camera.depthStencilID));
	renderer->setD3D11ShaderResourceView(3, ds->m_srv.Get(), ShaderStage::PS);

	// レンダーバッファ指定
	renderer->setD3D11RenderBuffer(m_quadRb);

	// トランスフォーム指定
	Matrix matrix = Matrix::CreateScale(1920 * 1.25f,1080 * 1.25f,1);
	//matrix *= Matrix::CreateRotationY(XMConvertToDegrees(90));
	renderer->setD3DTransformBuffer(matrix);
	
	auto skytexID = renderer->createTextureFromFile("data/texture/environment.hdr");
	renderer->setTexture(D3D::SHADER_TEX_SLOT_SKYBOX, skytexID, ShaderStage::PS);
	renderer->setD3D11Sampler(D3D::SHADER_SS_SLOT_SKYBOX, SamplerState::ANISOTROPIC_WRAP, ShaderStage::PS);

	// 描画
	renderer->d3dRender(m_quadRb);


	//----- フォワードレンダリング -----
	// レンダーターゲット指定
	renderer->setRenderTarget(camera.renderTargetID, camera.depthStencilID);

	// 不透明描画
	for (auto& opeque : m_opequeList)
	{
		renderer->setD3D11Material(opeque.materialID);
		renderer->setD3DTransformBuffer(opeque.worldMatrix);
		renderer->setD3D11RenderBuffer(opeque.renderBufferID);
		renderer->d3dRender(opeque.renderBufferID);
	}

	ImGui::Begin("RenderImage");

	ImGui::Image(renderer->m_gbuffer.m_normalSRV.Get(), ImVec2(1920 * 0.25, 1080 * 0.25));
	ImGui::Image(renderer->m_gbuffer.m_diffuseSRV.Get(), ImVec2(1920 * 0.25, 1080 * 0.25));
	ImGui::Image(renderer->m_gbuffer.m_positionSRV.Get(), ImVec2(1920 * 0.25, 1080 * 0.25));

	ImGui::End();
}

void RenderPipeline::skyPass(Camera& camera)
{

}

void RenderPipeline::transparentPass(Camera& camera)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// 半透明描画
	for (auto& opeque : m_transparentList)
	{
		renderer->setD3D11Material(opeque.materialID);
		renderer->setD3DTransformBuffer(opeque.worldMatrix);
		renderer->setD3D11RenderBuffer(opeque.renderBufferID);
		renderer->d3dRender(opeque.renderBufferID);
	}
}

void RenderPipeline::postPass(Camera& camera)
{

}

void RenderPipeline::endPass(Camera& camera)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());

	// バックバッファに反映
	auto* rt = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(camera.renderTargetID));
	renderer->d3dCopyResource(renderer->m_backBufferRT.Get(), rt->m_tex.Get());
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
