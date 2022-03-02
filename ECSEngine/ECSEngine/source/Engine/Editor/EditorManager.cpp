/*****************************************************************//**
 * \file   EditorManager.h
 * \brief  �G�f�B�^�}�l�[�W���[
 *
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/

#include "EditorManager.h"
#include "imgui.h"
#include "ImGuizmo.h"

#include <Engine/Engine.h>

#include <Engine/Utility/Input.h>
#include <Engine/Utility/AssimpLoader.h>

#include <Engine/Renderer/D3D11/D3D11RenderTarget.h>
#include <Engine/Renderer/Base/Model.h>
#include <Engine/ECS/Base/RenderPipeline.h>
#include <Engine/ECS/System/TransformSystem.h>

#include <Engine/ECS/Base/WorldManager.h>
#include <Engine/ECS/Base/GameObjectManager.h>
#include <Engine/ECS/ComponentData/AnimationComponentData.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/ComponentTag.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/LightComponentData.h>
#include <Engine/ECS/ComponentData/PhysicsComponentData.h>

#include <Engine/ECS/System/PhysicsSystem.h>

#include <App/FBXModel.h>

using namespace ecs;

// �萔��`
#define CheckType(Type) typeHash == TypeToHash(Type)

void EditorManager::initialize()
{
	auto* renderer = m_pEngine->getRendererManager();

	Transform transform(0);
	transform.translation = Vector3(0 ,10, -20);
	transform.rotation = Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(180), 0, 0);
	transform.scale = Vector3(5, 5, 5);

	Camera cameraData;
	cameraData.isOrthographic = false;
	cameraData.fovY = 60;
	cameraData.nearZ = 1.0f;
	cameraData.farZ = 1000.0f;
	cameraData.width = m_pEngine->getWindowWidth();
	cameraData.height = m_pEngine->getWindowHeight();
	cameraData.viewportOffset.x = 7;
	cameraData.viewportOffset.y = 26 * 1.5f;
	cameraData.viewportScale = 0.65f;
	cameraData.renderTargetID = renderer->createRenderTarget("SceneViewRT");
	cameraData.depthStencilID = renderer->createDepthStencil("SceneViewDS");

	m_editorCamera.transform = transform;
	m_editorCamera.camera = cameraData;

}

void EditorManager::finalize()
{

}

void EditorManager::update()
{
	// �����_���[�}�l�[�W���[
	auto* renderer = static_cast<D3D11RendererManager*>(m_pEngine->getRendererManager());
	Camera& camera = m_editorCamera.camera;
	ImVec2 winSize(
		camera.width * camera.viewportScale + camera.viewportOffset.x * 2,
		camera.height * camera.viewportScale + camera.viewportOffset.y  + camera.viewportOffset.x
	);

	// ���[���h�}�l�[�W���[�擾
	auto* pWorldManager = m_pEngine->getWorldManager();
	// ���݂̃��[���h�擾
	auto* pWorld = pWorldManager->getCurrentWorld();
	// �p�C�v���C���擾
	auto* pipeline = pWorld->getRenderPipeline();
	// �Q�[���V�[�������_�[�^�[�Q�b�g
	auto* gameRender = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(pipeline->m_renderTarget));
	

	// �����^�C��
	if (m_isRunTime)
	{
		// Game
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize(winSize);
		ImGui::Begin("Game", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		ImGui::Image(gameRender->m_srv.Get(),
			ImVec2(camera.width * camera.viewportScale, camera.height * camera.viewportScale));
		ImGui::End();
	}
	// �f�o�b�O
	else
	{
		// �V�[���r���[
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(winSize);
		ImGui::SetNextWindowBgAlpha(0);
		ImGui::Begin("Scene", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar);
		ImGui::End();

		CreateFBX();

		updateTransform();
		updateView();
		dispWorld();
	}

	// GUI�\��
	dispHierarchy();
	dispAsset();
	dispInspector();
}

void EditorManager::dispHierarchy()
{
	// ���[���h�}�l�[�W���[�擾
	auto* pWorldManager = m_pEngine->getWorldManager();
	// ���݂̃��[���h�擾
	auto* pWorld = pWorldManager->getCurrentWorld();

	//----- �q�G�����L�[ -----
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_HorizontalScrollbar);

	// ���[�g�֖߂�
	ImGui::Button("Return Root", ImVec2(200, 30));
	if (ImGui::BeginDragDropTarget()) {
		// Some processing...
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObjectID));
			GameObjectID payload_n = *(const GameObjectID*)payload->Data;
			// �e�q�֌W�č\�z
			pWorld->getGameObjectManager()->ResetParent(payload_n);
		}
		ImGui::EndDragDropTarget();
	}

	// ���[�g�I�u�W�F�N�g
	for (auto& root : pWorld->getGameObjectManager()->getRootList())
	{
		GameObject* gameObject = pWorld->getGameObjectManager()->getGameObject(root);
		if (gameObject == nullptr) continue;
		std::string name(gameObject->getName());
		std::size_t childNum = gameObject->getChildCount(); // �q�����邩

		// �q�m�[�h
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			(m_selectObjectInfo.instanceID == root && m_selectObjectInfo.typeID == TypeToID(GameObject)
				? ImGuiTreeNodeFlags_Selected : 0) | (childNum ? 0 : ImGuiTreeNodeFlags_Leaf));
		ImGui::PopStyleVar();

		ImGui::PushID(root);
		if (ImGui::BeginPopupContextItem()) {
			// Some processing...
			ImGui::Text("Move Parent");
			ImGui::EndPopup();
		}
		ImGui::PopID();

		// �N���b�N
		if (ImGui::IsItemClicked()) {
			// Some processing...
			m_selectObjectInfo.typeID = TypeToID(GameObject);
			m_selectObjectInfo.instanceID = root;
		}
		// �h���b�v
		if (ImGui::BeginDragDropTarget()) {
			// Some processing...
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				IM_ASSERT(payload->DataSize == sizeof(GameObjectID));
				GameObjectID payload_n = *(const GameObjectID*)payload->Data;
				// �e�q�֌W�č\�z
				pWorld->getGameObjectManager()->AddChild(root, payload_n);
			}
			ImGui::EndDragDropTarget();
		}
		// �h���b�O
		if (ImGui::BeginDragDropSource()) {
			// Some processing...
			GameObjectID id = root;
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &id, sizeof(GameObjectID));
			ImGui::Text("Move Parent");

			ImGui::EndDragDropSource();
		}
		// �m�[�h��
		if (open) {
			// Recursive call...
			DispChilds(root);
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void EditorManager::dispAsset()
{
	auto* pWorld = m_pEngine->getWorldManager()->getCurrentWorld();

	ImGui::Begin("Test");

	std::string name;
	if (m_isRunTime)
	{
		name = "Stop";
	}
	else
	{
		name = "Start";
	}
	if(ImGui::Button(name.c_str(), ImVec2(100, 30)))
	{
		std::string path("data/world/");

		m_isRunTime ^= 1;
		if (m_isRunTime)
		{
			pWorld->serializeWorld(path);
		}
		else
		{
			pWorld->deserializeWorld(path);
		}
	}

	ImGui::End();


	ImGui::Begin("System");

	for (auto&& system : pWorld->getSystemList())
	{
		ImGui::Text(system->getName().data());
	}

	ImGui::End();
}

void EditorManager::dispInspector()
{
	// ���[���h�}�l�[�W���[�擾
	auto* pWorldManager = m_pEngine->getWorldManager();
	// ���݂̃��[���h�擾
	auto* pWorld = pWorldManager->getCurrentWorld();
	// �����_���[�̎擾
	auto* renderer = m_pEngine->getRendererManager();

	//----- �C���X�y�N�^�[ -----
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Inspector", 0, ImGuiWindowFlags_HorizontalScrollbar);

	if (m_selectObjectInfo.typeID == TypeToID(GameObject))
	{
		if (m_selectObjectInfo.instanceID != NONE_GAME_OBJECT_ID)
		{
			// �I���I�u�W�F�N�g
			auto* curObject = pWorld->getGameObjectManager()->getGameObject(m_selectObjectInfo.instanceID);
			if (curObject == nullptr)
			{
				m_selectObjectInfo.instanceID = NONE_GAME_OBJECT_ID;
				ImGui::End();
				return;
			}

			auto& chunk = pWorld->getChunkList()[curObject->m_entity.m_chunkIndex];

			Transform* transform = nullptr;
			const auto& archetype = chunk.getArchetype();

			// �g�����X�t�H�[���f�[�^�������Ă��邩
			for (int i = 0; i < archetype.getArchetypeSize(); ++i)
			{
				const auto& type = archetype.getTypeInfo(i);
				auto typeHash = type.getHash();
				void* data = chunk.getComponentData(typeHash, curObject->m_entity.m_index);
				if (CheckType(Transform))		transform = (Transform*)data;
			}

			// �g�����X�t�H�[���f�[�^���
			if (transform)
			{
				EditTransform(pWorld, m_editorCamera.camera, *transform);
			}

			// �R���|�[�l���g�f�[�^���
			for (int i = 0; i < archetype.getArchetypeSize(); ++i)
			{
				const auto& type = archetype.getTypeInfo(i);
				componentInspector(type.getHash(), 
					chunk.getComponentData(type.getHash(), curObject->m_entity.m_index));
				//DispGui(type.getName(), chunk.getComponentData(type.getName(), curObject->m_entity.m_index));
			}
		}
	}
	else if (m_selectObjectInfo.typeID == TypeToID(Material))
	{

	}

	ImGui::End();
}

void EditorManager::dispWorld()
{
	// �����_���[�擾
	auto* renderer = m_pEngine->getRendererManager();
	// ���[���h�}�l�[�W���[�擾
	auto* pWorldManager = m_pEngine->getWorldManager();
	// ���݂̃��[���h�擾
	auto* pWorld = pWorldManager->getCurrentWorld();
	// �p�C�v���C���擾
	auto* pipeline = pWorld->getRenderPipeline();

	// �p�C�v���C���f�[�^�擾
	auto pipelineData = pipeline->GetPipelineData();
	pipelineData.camera = &m_editorCamera.camera;

	// �J�����O
	pipeline->cullingPass(renderer, pipelineData);

	// �`��
	pipeline->beginPass(renderer, pipelineData);
	pipeline->prePass(renderer, pipelineData);
	pipeline->gbufferPass(renderer, pipelineData);
	pipeline->shadowPass(renderer, pipelineData);
	pipeline->opaquePass(renderer, pipelineData);
	pipeline->skyPass(renderer, pipelineData);
	pipeline->transparentPass(renderer, pipelineData);
	pipeline->postPass(renderer, pipelineData);


	// BulletDebugDraw
	auto* physicsSytem = pWorld->getSystem<PhysicsSystem>();
	if (physicsSytem)
	{
		physicsSytem->debugDraw();
	}

	pipeline->endPass(renderer, pipelineData);
}

void EditorManager::updateTransform()
{
	// ���[���h�}�l�[�W���[�擾
	auto* pWorldManager = m_pEngine->getWorldManager();
	// ���݂̃��[���h�擾
	auto* pWorld = pWorldManager->getCurrentWorld();

	// �g�����X�t�H�[���X�V
	auto chunkList = pWorld->getEntityManager()->getChunkList<Transform>();
	for (auto& chunk : chunkList)
	{
		auto transforms = chunk->getComponentArray<Transform>();
		for (int i = 0; i < transforms.Count(); ++i)
		{
			TransformSystem::updateTransform(transforms[i]);
		}
	}

	// �K�w�\���X�V
	TransformSystem::updateHierarchy(pWorld->getGameObjectManager(),
		pWorld->getGameObjectManager()->getRootList());
}

void EditorManager::updateView()
{
	// �J��������
	Transform& transform = m_editorCamera.transform;
	POINT* mousePos = GetMousePosition();
	POINT mouseDist = {
		mousePos->x - m_oldMousePos.x,
		mousePos->y - m_oldMousePos.y,
	};
	m_oldMousePos = *mousePos;

	float SCREEN_WIDTH = m_pEngine->getWindowWidth();
	float SCREEN_HEIGHT = m_pEngine->getWindowHeight();

	// ����
	Matrix matR = Matrix::CreateFromQuaternion(transform.rotation);
	Vector3 vPos = matR.Translation();
	Vector3 right = matR.Right();
	Vector3 up = Vector3(0, 1, 0);
	Vector3 forward = matR.Forward();
	Vector3 vLook = vPos + forward;
	float focus = 0.0f;

	// ���x
	float moveSpeed = 1.0f / 60.0f;
	float rotSpeed = 3.141592f / 60.0f;

	// ���{�^��(�J������荞��
	if (GetKeyPress(VK_RBUTTON))
	{
		// ��]��
		float angleX = 360.f * mouseDist.x / SCREEN_WIDTH * 0.5f;	// ��ʈ����360�x��](��ʃT�C�Y�̔�����180�x��])
		float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT * 0.5f;	// ��ʈ����180�x��](��ʃT�C�Y�̔�����90�x��])

		transform.rotation *= Quaternion::CreateFromAxisAngle(up, XMConvertToRadians(-angleX));
		transform.rotation *= Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-angleY));
	}
	// ��]
	if (GetKeyPress(VK_RIGHT))
	{
		transform.rotation *= Quaternion::CreateFromAxisAngle(up, -rotSpeed);
	}
	if (GetKeyPress(VK_LEFT))
	{
		transform.rotation *= Quaternion::CreateFromAxisAngle(up, rotSpeed);
	}
	if (GetKeyPress(VK_UP))
	{
		transform.rotation *= Quaternion::CreateFromAxisAngle(right, rotSpeed);
	}
	if (GetKeyPress(VK_DOWN))
	{
		transform.rotation *= Quaternion::CreateFromAxisAngle(right, -rotSpeed);
	}

	// �ړ�
	if (GetMouseButton(MOUSEBUTTON_R) || GetKeyPress(VK_SHIFT))
	{
		if (GetKeyPress(VK_SHIFT))
		{
			moveSpeed *= 4;
		}
		if (GetKeyPress(VK_D))
		{
			transform.translation += right * moveSpeed;
		}
		if (GetKeyPress(VK_A))
		{
			transform.translation += right * -moveSpeed;
		}
		if (GetKeyPress(VK_W))
		{
			transform.translation += forward * moveSpeed;
		}
		if (GetKeyPress(VK_S))
		{
			transform.translation += forward * -moveSpeed;
		}
	}

	// Matrix�X�V
	// �g�k
	transform.localMatrix = Matrix::CreateScale(transform.scale);
	// ��]
	transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
	// �ړ�
	transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
	// �O���[�o���}�g���b�N�X�X�V
	transform.localMatrix.Up(up);
	transform.globalMatrix = transform.localMatrix;

	// �J�����X�V
	RenderPipeline::updateCamera(m_editorCamera.camera, transform, SCREEN_WIDTH, SCREEN_HEIGHT);

}

void EditorManager::DispChilds(const InstanceID parentID)
{
	// ���[���h�}�l�[�W���[�擾
	auto* pWorldManager = m_pEngine->getWorldManager();
	// ���݂̃��[���h�擾
	World* pWorld = pWorldManager->getCurrentWorld();

	int nIndex = 0;
	for (auto& child : pWorld->getGameObjectManager()->GetChilds(parentID))
	{
		GameObject* gameObject = pWorld->getGameObjectManager()->getGameObject(child);
		if (gameObject == nullptr) continue;
		std::string name(gameObject->getName());
		std::size_t childNum = gameObject->getChildCount(); // �q�����邩

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			(m_selectObjectInfo.instanceID == child && m_selectObjectInfo.typeID == TypeToID(GameObject)
				? ImGuiTreeNodeFlags_Selected : 0) | (childNum ? 0 : ImGuiTreeNodeFlags_Leaf));
		ImGui::PopStyleVar();

		ImGui::PushID(child);
		if (ImGui::BeginPopupContextItem()) {
			// Some processing...
			ImGui::EndPopup();
		}
		ImGui::PopID();

		// �N���b�N
		if (ImGui::IsItemClicked()) {
			// Some processing...
			m_selectObjectInfo.typeID = TypeToID(GameObject);
			m_selectObjectInfo.instanceID = child;
		}
		// �h���b�v
		if (ImGui::BeginDragDropTarget()) {
			// Some processing...
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				IM_ASSERT(payload->DataSize == sizeof(GameObjectID));
				GameObjectID payload_n = *(const GameObjectID*)payload->Data;
				// �e�q�֌W�č\�z
				pWorld->getGameObjectManager()->AddChild(child, payload_n);
				auto* transform = pWorld->getGameObjectManager()->getComponentData<Transform>(payload_n);
			}
			ImGui::EndDragDropTarget();
		}
		// �h���b�O
		if (ImGui::BeginDragDropSource()) {
			// Some processing...
			GameObjectID id = child;
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &id, sizeof(GameObjectID));
			ImGui::Text("Move Parent");

			ImGui::EndDragDropSource();
		}
		// �m�[�h��
		if (open) {
			// Recursive call...
			DispChilds(child);
			ImGui::TreePop();
		}
	}
}

void EditorManager::componentInspector(std::size_t typeHash, void* data)
{
	if (data == nullptr)
	{
		return;
	}
	else if (CheckType(DirectionalLight))
	{
		DirectionalLight* dirLit = static_cast<DirectionalLight*>(data);
		if (ImGui::TreeNode(TypeToString(DirectionalLight)))
		{
			ImGui::ColorEdit3("color", (float*)&dirLit->data.color);
			ImGui::InputFloat("intensity", (float*)&dirLit->data.color.w);
			ImGui::ColorEdit4("ambient", (float*)&dirLit->data.ambient);
			ImGui::TreePop();
		}
	}
	else if (CheckType(PointLight))
	{
		PointLight* pointLit = static_cast<PointLight*>(data);
		if (ImGui::TreeNode(TypeToString(PointLight)))
		{
			ImGui::ColorEdit3("color", (float*)&pointLit->data.color);
			ImGui::InputFloat("intensity", (float*)&pointLit->data.color.w);
			ImGui::InputFloat("range", &pointLit->data.range);
			ImGui::TreePop();
		}
	}
	else if (CheckType(SpotLight))
	{
		SpotLight* spotLit = static_cast<SpotLight*>(data);
		if (ImGui::TreeNode(TypeToString(PointLight)))
		{
			ImGui::ColorEdit3("color", (float*)&spotLit->data.color);
			ImGui::InputFloat("intensity", (float*)&spotLit->data.color.w);
			ImGui::InputFloat("range", &spotLit->data.range);
			ImGui::InputFloat("spot", &spotLit->data.spot);
			ImGui::TreePop();
		}
	}
	//else if (CheckType(SkinnedMeshRenderer))
	//{
	//	SkinnedMeshRenderer* renderer = static_cast<SkinnedMeshRenderer*>(data);
	//	if (ImGui::TreeNode(TypeToString(SkinnedMeshRenderer)))
	//	{
	//		ImGui::InputInt("materialID", (int*)renderer->materialID);
	//		ImGui::InputInt("meshID", (int*)renderer->meshID);
	//		ImGui::InputInt("rootObjectID", (int*)renderer->rootObjectID);
	//		ImGui::TreePop();
	//	}
	//}
}

void EditorManager::EditTransform(World* pWorld ,Camera& camera, Transform& transform)
{
	Matrix oldGlobalMatrix = transform.globalMatrix;

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;
	float camDistance = 8.0f;

	if (!m_isRunTime)
	{
		//if (ImGui::IsKeyPressed('1'))
		//	mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		//if (ImGui::IsKeyPressed('2'))
		//	mCurrentGizmoOperation = ImGuizmo::ROTATE;
		//if (ImGui::IsKeyPressed('3')) // r Key
		//	mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		{
			mCurrentGizmoOperation = ImGuizmo::SCALE;
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		}

		Matrix oldLocal = transform.localMatrix;
		Vector3 matrixTranslation, matrixRotation, matrixScale;
		ImGuizmo::DecomposeMatrixToComponents(&transform.localMatrix.m[0][0],
			(float*)&matrixTranslation, (float*)&matrixRotation, (float*)&matrixScale);
		ImGui::InputFloat3("Tr", (float*)&matrixTranslation);
		ImGui::InputFloat3("Rt", (float*)&matrixRotation);
		ImGui::InputFloat3("Sc", (float*)&matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents((float*)&matrixTranslation, (float*)&matrixRotation,
			(float*)&matrixScale, &transform.localMatrix.m[0][0]);

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		//if (ImGui::IsKeyPressed('4'))
		//	useSnap = !useSnap;
		//ImGui::Checkbox("", &useSnap);
		//ImGui::SameLine();

		//switch (mCurrentGizmoOperation)
		//{
		//case ImGuizmo::TRANSLATE:
		//	ImGui::InputFloat3("Snap", &snap[0]);
		//	break;
		//case ImGuizmo::ROTATE:
		//	ImGui::InputFloat("Angle Snap", &snap[0]);
		//	break;
		//case ImGuizmo::SCALE:
		//	ImGui::InputFloat("Scale Snap", &snap[0]);
		//	break;
		//}

		ImGui::Checkbox("Bound Sizing", &boundSizing);
		if (boundSizing)
		{
			ImGui::PushID(3);
			ImGui::Checkbox("", &boundSizingSnap);
			ImGui::SameLine();
			ImGui::InputFloat3("Snap", boundsSnap);
			ImGui::PopID();
		}

		// �r���[�|�[�g
		Camera& camera = m_editorCamera.camera;
		//ImGuizmo::SetRect(camera.viewportOffset.x, camera.viewportOffset.y, 
		//	camera.width * camera.viewportScale, camera.height * camera.viewportScale);
		ImGuizmo::SetRect(0, 0, camera.width, camera.height);

		// �M�Y��
		Matrix globalMatrix = transform.globalMatrix;
		ImGuizmo::Manipulate(&camera.view.m[0][0], &camera.projection.m[0][0], mCurrentGizmoOperation, mCurrentGizmoMode,
			&globalMatrix.m[0][0], NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

		// �ύX���擾
		Vector3 newTra, newRot, newSca, oldTra, oldRot, oldSca;
		ImGuizmo::DecomposeMatrixToComponents(&globalMatrix.m[0][0], (float*)&newTra, (float*)&newRot, (float*)&newSca);
		ImGuizmo::DecomposeMatrixToComponents(&oldGlobalMatrix.m[0][0], (float*)&oldTra, (float*)&oldRot, (float*)&oldSca);
		// ���[�J���}�g���b�N�X
		Matrix local = globalMatrix;

		// �e�̂�����ꍇ
		auto parent = pWorld->getGameObjectManager()->GetParent(transform.id);
		if (parent != NONE_GAME_OBJECT_ID)
		{
			auto parentTrans = pWorld->getGameObjectManager()->getComponentData<Transform>(parent);
			parentTrans->globalMatrix.Invert(local);
			local = globalMatrix * local;
		}
		Vector3 locTra, locRot, locSca;
		ImGuizmo::DecomposeMatrixToComponents(&local.m[0][0], (float*)&locTra, (float*)&locRot, (float*)&locSca);

		// ���[�J���}�g���b�N�X�X�V
		transform.localMatrix = local;

		if (oldTra != newTra)
		{
			// ���x
			auto physics = pWorld->getGameObjectManager()->getComponentData<Physics>(transform.id);
			if (physics) physics->velocity = locTra - transform.translation;
			// �ړ�
			transform.translation = locTra;
		}
		if (oldRot != newRot)
		{
			// ��]
			Matrix invSca = Matrix::CreateScale(locSca);
			invSca = invSca.Invert();
			transform.rotation = Quaternion::CreateFromRotationMatrix(invSca * local);
		}
		else if (oldSca != newSca)
		{
			//Matrix invSca = Matrix::CreateScale(locSca);
			//invSca = invSca.Invert();
			//Quaternion rot = Quaternion::CreateFromRotationMatrix(invSca * local);
			//// �g�k
			//Matrix invRot = Matrix::CreateFromQuaternion(rot);
			//invRot = invRot.Invert();
			//Matrix sca = local * invRot;
			//transform.scale.x = sca.m[0][0];
			//transform.scale.y = sca.m[1][1];
			//transform.scale.z = sca.m[2][2];
			////transform.scale += newSca - oldSca;
			transform.scale = locSca;
		}

		// �J�����̎p��
		ImGuizmo::ViewManipulate(&camera.view.m[0][0], camDistance, 
			ImVec2(camera.width * camera.viewportScale - 128, camera.viewportOffset.y),
			ImVec2(128, 128), 0x10101010);

		// �r���[�s�񂩂�p���𒊏o
		m_editorCamera.transform.rotation = 
			Quaternion::CreateFromRotationMatrix(camera.view.Transpose());
	}
}

//--- OpenFileDialog�𗘗p����
void OpenFBXFile(Model &out)
{
	// �t�@�C���̎w��A�ǂݍ���
	char filename[MAX_PATH] = "";
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetActiveWindow();
	// �t�@�C�����̊i�[��
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	// �t�B���^�[
	ofn.lpstrFilter =
		"All Files\0*.*\0.fbx\0*.fbx\0";
	ofn.nFilterIndex = 2;
	// �t���O
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	/* �_�C�A���O�̕\��
	* �_�C�A���O�����܂ł͏�����
	* �Ԃ�Ȃ��B�u�J���v�������Ɩ߂�l��
	* TRUE���Ԃ�B����ȊO��FALSE
	*/
	if (TRUE == GetOpenFileName(&ofn))
	{
		// AssimpModelTest
		AssimpLoader::get().LoadModel(Engine::get().getRendererManager(), ofn.lpstrFile, out);

		// FBX�t�@�C���̓ǂݍ���
		//FBXModel::LoadFBXModel(ofn.lpstrFile, out);
	}
}

void childCreate(Model& model, Model::NodeInfo &node, GameObjectManager* magaer,
	MaterialID& matID, GameObjectID parent, 
	std::multimap<std::string, GameObjectID>& skinMeshList)
{
	SkinnedMeshRenderer rd;
	if (node.meshIndexes.size() > 0)
	{
		auto& mesh = model.m_meshList[node.meshIndexes.front()];
		rd.meshID = mesh.meshID;
		if (mesh.materialIndex != Model::NONE_INDEX)
		{
			auto& mat = model.m_materialList[mesh.materialIndex];

			auto* renderer = Engine::get().getRendererManager();
			ShaderDesc desc;
			desc.m_name = "AnimationGBuffer";
			desc.m_stages = ShaderStageFlags::VS | ShaderStageFlags::PS;
			ShaderID shaderID = renderer->createShader(desc);
			rd.materialID = renderer->createMaterial(mat.name.c_str(), shaderID);
			auto* pMat = renderer->getMaterial(rd.materialID);
			pMat->setTexture("_MainTexture", mat.DiffuseTex);
			pMat->m_rasterizeState = RasterizeState::CULL_NONE;
		}
		else
		{
			rd.materialID = matID;
		}
	}
	Vector3 pos = node.transform.Translation();
	Quaternion rot = Quaternion::CreateFromRotationMatrix(node.transform);
	Vector3 sca = Vector3(1.0f, 1.0f, 1.0f);

	// �I�u�W�F�N�g����
	Archetype archetype = Archetype::create<DynamicType, Transform, SkinnedMeshRenderer>();
	auto goID = magaer->createGameObject(node.name, archetype);
	magaer->SetParent(goID, parent);
	magaer->setComponentData(goID, Transform(goID, pos, rot, sca));
	magaer->setComponentData(goID, rd);

	if (node.meshIndexes.size() > 0)
	{
		auto& mesh = model.m_meshList[node.meshIndexes.front()];
		// �{�[���������Ă���
		if (mesh.boneTable.size() > 0)
		{
			if(mesh.rootBoneName.size() > 0)
				skinMeshList.emplace(mesh.rootBoneName, goID);
		}
	}

	for (auto& childID : node.childs)
	{
		childCreate(model, model.m_nodeMap[childID], magaer, matID, goID, skinMeshList);
	}
}

bool FindRootBoneID(GameObjectManager* magaer, GameObjectID& parentID, 
	GameObjectID& skinID, const std::string& rootName)
{
	auto* parent = magaer->getGameObject(parentID);
	if (parent->getName() == rootName)
	{
		auto* skinMesh = magaer->getComponentData<SkinnedMeshRenderer>(skinID);
		skinMesh->rootObjectID = parentID;
		return true;
	}

	// ���̌���
	for (auto childID : magaer->GetChilds(parentID))
	{
		if (FindRootBoneID(magaer, childID, skinID, rootName))
			return true;
	}

	return false;
}

void EditorManager::CreateFBX()
{
	//ImGui::Begin("Test");

	//if (ImGui::Button("Load FBX"))
	//{
	//	// ���f�����[�h
	//	Model fbx;
	//	OpenFBXFile(fbx);

	//	auto* renderer = Engine::get().getRendererManager();

	//	ShaderDesc desc;
	//	desc.m_name = "AnimationGBuffer";
	//	desc.m_stages = ShaderStageFlags::VS | ShaderStageFlags::PS;
	//	ShaderID shaderID = renderer->createShader(desc);
	//	MaterialID matID = renderer->createMaterial(fbx.m_rootNode.name, shaderID);
	//	auto* pMat = renderer->getMaterial(matID);
	//	pMat->m_rasterizeState = RasterizeState::CULL_NONE;

	//	// �I�u�W�F�N�g����
	//	auto* goMgr = m_pEngine->getWorldManager()->getCurrentWorld()->getGameObjectManager();
	//	Archetype archetype = Archetype::create<DynamicType, Transform, Animator>();
	//	auto parentID = goMgr->createGameObject(fbx.m_name, archetype);
	//	goMgr->setComponentData(parentID, Transform(parentID, 
	//		Vector3(), Quaternion(), Vector3(0.2,0.2,0.2)));
	//	Animator anim;
	//	anim.animationID = fbx.m_animationList[0].animationID;
	//	goMgr->setComponentData(parentID, anim);

	//	std::multimap<std::string ,GameObjectID> skinMeshList;
	//	childCreate(fbx, fbx.m_rootNode, goMgr, matID, parentID, skinMeshList);

	//	// �X�L�����b�V���̃��[�g�{�[��������
	//	for (auto& skin : skinMeshList)
	//	{
	//		FindRootBoneID(goMgr, parentID, skin.second, skin.first);
	//	}
	//}


	//ImGui::End();
}
