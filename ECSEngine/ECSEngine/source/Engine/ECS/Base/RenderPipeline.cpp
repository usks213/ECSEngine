/*****************************************************************//**
 * \file   RenderPipeline.h
 * \brief  レンダーパイプライン
 *
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/

#include "RenderPipeline.h"
#include <Engine/ECS/Base/EntityManager.h>

#include <Engine/Engine.h>
#include <Engine/Renderer/D3D11/D3D11RendererManager.h>

#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/System/PhysicsSystem.h>


using namespace ecs;


/// @brief 生成時
void RenderPipeline::onCreate()
{
	foreach<Transform>(
		[](Transform& transform)
		{
			// 拡縮
			transform.localMatrix = Matrix::CreateScale(transform.scale);
			// 回転
			transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
			// 移動
			transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
		});

	// トランスフォーム更新
	transformPass();
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

	// トランスフォーム更新
	transformPass();

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

	// システム更新
	systemPass(*mainCamera);

	// カリング
	cullingPass(*mainCamera);

	// 描画
	renderingPass();

}

void RenderPipeline::beginPass()
{

}

void RenderPipeline::transformPass()
{
	// トランスフォーム更新
	foreach<Transform, DynamicType>(
		[](Transform& transform, DynamicType& type)
		{
			updateTransform(transform);
		});

	// 階層構造更新
	for (const auto& id : m_pWorld->getGameObjectManager()->getRootList())
	{
		auto* transform = getGameObjectManager()->getComponentData<Transform>(id);
		if (transform == nullptr) continue;
		transform->globalMatrix = transform->localMatrix;
		transform->globalScale = transform->scale;

		for (auto child : getGameObjectManager()->GetChilds(id))
		{
			updateChilds(child, transform->globalMatrix, transform->globalScale);
		}
	}
}

void RenderPipeline::systemPass(Camera& camera)
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());


	// ディレクショナルライト
	DirectionalLightData dirLit;
	dirLit.ambient = Vector4(0.3f, 0.3f, 0.3f, 0.3f);
	dirLit.diffuse = Vector4(2.0f, 2.0f, 2.0f, 1.0f);
	dirLit.specular = Vector4(0.1f, 0.1f, 0.1f, 0.2f);
	dirLit.direction = Vector4(1.f, -1.5f, -1.f, 0.0f);

	// システムバッファの設定
	D3D::SystemBuffer sysBuffer;
	sysBuffer._mView = camera.view.Transpose();
	sysBuffer._mProj = camera.projection.Transpose();
	sysBuffer._viewPos = Vector4(camera.world.Translation());
	sysBuffer._directionalLit = dirLit;
	renderer->setD3DSystemBuffer(sysBuffer);
}

void RenderPipeline::cullingPass(Camera& camera)
{

}

void RenderPipeline::renderingPass()
{
	// レンダラーマネージャー
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());


	// バッチ描画
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


	// オブジェクト描画
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
}

void RenderPipeline::endPass()
{

}

inline void RenderPipeline::updateTransform(Transform& transform)
{
	// 拡縮
	transform.localMatrix = Matrix::CreateScale(transform.scale);
	// 回転
	transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
	// 移動
	transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
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
	if (camera.isOrthographic)
	{
		// 並行投影
		camera.projection = Matrix::CreateOrthographic(
			camera.width, camera.height, camera.nearZ, camera.farZ);
	}
	else
	{
		// 透視投影
		camera.projection = Matrix::CreatePerspectiveFieldOfView(
			XMConvertToRadians(camera.fovY), camera.aspect, camera.nearZ, camera.farZ);
	}
}

inline void RenderPipeline::updateChilds(const GameObjectID& parent, const Matrix& globalMatrix, const Vector3& globalScale)
{
	auto* transform = getGameObjectManager()->getComponentData<Transform>(parent);
	transform->globalMatrix = transform->localMatrix * globalMatrix;
	transform->globalScale = transform->scale * globalScale;

	for (auto child : getGameObjectManager()->GetChilds(parent))
	{
		updateChilds(child, transform->globalMatrix, transform->globalScale);
	}
}