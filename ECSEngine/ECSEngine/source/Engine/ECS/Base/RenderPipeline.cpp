/*****************************************************************//**
 * \file   RenderPipeline.h
 * \brief  �����_�[�p�C�v���C��
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


/// @brief ������
void RenderPipeline::onCreate()
{
	foreach<Transform>(
		[](Transform& transform)
		{
			// �g�k
			transform.localMatrix = Matrix::CreateScale(transform.scale);
			// ��]
			transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
			// �ړ�
			transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
		});

	// �g�����X�t�H�[���X�V
	transformPass();
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

	// �g�����X�t�H�[���X�V
	transformPass();

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

	// �V�X�e���X�V
	systemPass(*mainCamera);

	// �J�����O
	cullingPass(*mainCamera);

	// �`��
	renderingPass();

}

void RenderPipeline::beginPass()
{

}

void RenderPipeline::transformPass()
{
	// �g�����X�t�H�[���X�V
	foreach<Transform, DynamicType>(
		[](Transform& transform, DynamicType& type)
		{
			updateTransform(transform);
		});

	// �K�w�\���X�V
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
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());


	// �f�B���N�V���i�����C�g
	DirectionalLightData dirLit;
	dirLit.ambient = Vector4(0.3f, 0.3f, 0.3f, 0.3f);
	dirLit.diffuse = Vector4(2.0f, 2.0f, 2.0f, 1.0f);
	dirLit.specular = Vector4(0.1f, 0.1f, 0.1f, 0.2f);
	dirLit.direction = Vector4(1.f, -1.5f, -1.f, 0.0f);

	// �V�X�e���o�b�t�@�̐ݒ�
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
	// �����_���[�}�l�[�W���[
	auto* engine = m_pWorld->getWorldManager()->getEngine();
	auto* renderer = static_cast<D3D11RendererManager*>(engine->getRendererManager());


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
}

void RenderPipeline::endPass()
{

}

inline void RenderPipeline::updateTransform(Transform& transform)
{
	// �g�k
	transform.localMatrix = Matrix::CreateScale(transform.scale);
	// ��]
	transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
	// �ړ�
	transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
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