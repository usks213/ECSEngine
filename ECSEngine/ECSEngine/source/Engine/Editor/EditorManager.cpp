/*****************************************************************//**
 * \file   EditorManager.h
 * \brief  エディタマネージャー
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

// 定数定義
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
	// レンダラーマネージャー
	auto* renderer = static_cast<D3D11RendererManager*>(m_pEngine->getRendererManager());
	Camera& camera = m_editorCamera.camera;
	ImVec2 winSize(
		camera.width * camera.viewportScale + camera.viewportOffset.x * 2,
		camera.height * camera.viewportScale + camera.viewportOffset.y  + camera.viewportOffset.x
	);

	// ワールドマネージャー取得
	auto* pWorldManager = m_pEngine->getWorldManager();
	// 現在のワールド取得
	auto* pWorld = pWorldManager->getCurrentWorld();
	// パイプライン取得
	auto* pipeline = pWorld->getRenderPipeline();
	// ゲームシーンレンダーターゲット
	auto* gameRender = static_cast<D3D11RenderTarget*>(renderer->getRenderTarget(pipeline->m_renderTarget));
	

	// ランタイム
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
	// デバッグ
	else
	{
		// シーンビュー
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

	// GUI表示
	dispHierarchy();
	dispAsset();
	dispInspector();
}

void EditorManager::dispHierarchy()
{
	// ワールドマネージャー取得
	auto* pWorldManager = m_pEngine->getWorldManager();
	// 現在のワールド取得
	auto* pWorld = pWorldManager->getCurrentWorld();

	//----- ヒエラルキー -----
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_HorizontalScrollbar);

	// ルートへ戻す
	ImGui::Button("Return Root", ImVec2(200, 30));
	if (ImGui::BeginDragDropTarget()) {
		// Some processing...
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObjectID));
			GameObjectID payload_n = *(const GameObjectID*)payload->Data;
			// 親子関係再構築
			pWorld->getGameObjectManager()->ResetParent(payload_n);
		}
		ImGui::EndDragDropTarget();
	}

	// ルートオブジェクト
	for (auto& root : pWorld->getGameObjectManager()->getRootList())
	{
		GameObject* gameObject = pWorld->getGameObjectManager()->getGameObject(root);
		if (gameObject == nullptr) continue;
		std::string name(gameObject->getName());
		std::size_t childNum = gameObject->getChildCount(); // 子がいるか

		// 子ノード
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

		// クリック
		if (ImGui::IsItemClicked()) {
			// Some processing...
			m_selectObjectInfo.typeID = TypeToID(GameObject);
			m_selectObjectInfo.instanceID = root;
		}
		// ドロップ
		if (ImGui::BeginDragDropTarget()) {
			// Some processing...
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				IM_ASSERT(payload->DataSize == sizeof(GameObjectID));
				GameObjectID payload_n = *(const GameObjectID*)payload->Data;
				// 親子関係再構築
				pWorld->getGameObjectManager()->AddChild(root, payload_n);
			}
			ImGui::EndDragDropTarget();
		}
		// ドラッグ
		if (ImGui::BeginDragDropSource()) {
			// Some processing...
			GameObjectID id = root;
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &id, sizeof(GameObjectID));
			ImGui::Text("Move Parent");

			ImGui::EndDragDropSource();
		}
		// ノード内
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
	// ワールドマネージャー取得
	auto* pWorldManager = m_pEngine->getWorldManager();
	// 現在のワールド取得
	auto* pWorld = pWorldManager->getCurrentWorld();
	// レンダラーの取得
	auto* renderer = m_pEngine->getRendererManager();

	//----- インスペクター -----
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Inspector", 0, ImGuiWindowFlags_HorizontalScrollbar);

	if (m_selectObjectInfo.typeID == TypeToID(GameObject))
	{
		if (m_selectObjectInfo.instanceID != NONE_GAME_OBJECT_ID)
		{
			// 選択オブジェクト
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

			// トランスフォームデータを持っているか
			for (int i = 0; i < archetype.getArchetypeSize(); ++i)
			{
				const auto& type = archetype.getTypeInfo(i);
				auto typeHash = type.getHash();
				void* data = chunk.getComponentData(typeHash, curObject->m_entity.m_index);
				if (CheckType(Transform))		transform = (Transform*)data;
			}

			// トランスフォームデータ情報
			if (transform)
			{
				EditTransform(pWorld, m_editorCamera.camera, *transform);
			}

			// コンポーネントデータ情報
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
	// レンダラー取得
	auto* renderer = m_pEngine->getRendererManager();
	// ワールドマネージャー取得
	auto* pWorldManager = m_pEngine->getWorldManager();
	// 現在のワールド取得
	auto* pWorld = pWorldManager->getCurrentWorld();
	// パイプライン取得
	auto* pipeline = pWorld->getRenderPipeline();

	// パイプラインデータ取得
	auto pipelineData = pipeline->GetPipelineData();
	pipelineData.camera = &m_editorCamera.camera;

	// カリング
	pipeline->cullingPass(renderer, pipelineData);

	// 描画
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
	// ワールドマネージャー取得
	auto* pWorldManager = m_pEngine->getWorldManager();
	// 現在のワールド取得
	auto* pWorld = pWorldManager->getCurrentWorld();

	// トランスフォーム更新
	auto chunkList = pWorld->getEntityManager()->getChunkList<Transform>();
	for (auto& chunk : chunkList)
	{
		auto transforms = chunk->getComponentArray<Transform>();
		for (int i = 0; i < transforms.Count(); ++i)
		{
			TransformSystem::updateTransform(transforms[i]);
		}
	}

	// 階層構造更新
	TransformSystem::updateHierarchy(pWorld->getGameObjectManager(),
		pWorld->getGameObjectManager()->getRootList());
}

void EditorManager::updateView()
{
	// カメラ操作
	Transform& transform = m_editorCamera.transform;
	POINT* mousePos = GetMousePosition();
	POINT mouseDist = {
		mousePos->x - m_oldMousePos.x,
		mousePos->y - m_oldMousePos.y,
	};
	m_oldMousePos = *mousePos;

	float SCREEN_WIDTH = m_pEngine->getWindowWidth();
	float SCREEN_HEIGHT = m_pEngine->getWindowHeight();

	// 向き
	Matrix matR = Matrix::CreateFromQuaternion(transform.rotation);
	Vector3 vPos = matR.Translation();
	Vector3 right = matR.Right();
	Vector3 up = Vector3(0, 1, 0);
	Vector3 forward = matR.Forward();
	Vector3 vLook = vPos + forward;
	float focus = 0.0f;

	// 速度
	float moveSpeed = 1.0f / 60.0f;
	float rotSpeed = 3.141592f / 60.0f;

	// 左ボタン(カメラ回り込み
	if (GetKeyPress(VK_RBUTTON))
	{
		// 回転量
		float angleX = 360.f * mouseDist.x / SCREEN_WIDTH * 0.5f;	// 画面一周で360度回転(画面サイズの半分で180度回転)
		float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT * 0.5f;	// 画面一周で180度回転(画面サイズの半分で90度回転)

		transform.rotation *= Quaternion::CreateFromAxisAngle(up, XMConvertToRadians(-angleX));
		transform.rotation *= Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-angleY));
	}
	// 回転
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

	// 移動
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

	// Matrix更新
	// 拡縮
	transform.localMatrix = Matrix::CreateScale(transform.scale);
	// 回転
	transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
	// 移動
	transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
	// グローバルマトリックス更新
	transform.localMatrix.Up(up);
	transform.globalMatrix = transform.localMatrix;

	// カメラ更新
	RenderPipeline::updateCamera(m_editorCamera.camera, transform, SCREEN_WIDTH, SCREEN_HEIGHT);

}

void EditorManager::DispChilds(const InstanceID parentID)
{
	// ワールドマネージャー取得
	auto* pWorldManager = m_pEngine->getWorldManager();
	// 現在のワールド取得
	World* pWorld = pWorldManager->getCurrentWorld();

	int nIndex = 0;
	for (auto& child : pWorld->getGameObjectManager()->GetChilds(parentID))
	{
		GameObject* gameObject = pWorld->getGameObjectManager()->getGameObject(child);
		if (gameObject == nullptr) continue;
		std::string name(gameObject->getName());
		std::size_t childNum = gameObject->getChildCount(); // 子がいるか

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

		// クリック
		if (ImGui::IsItemClicked()) {
			// Some processing...
			m_selectObjectInfo.typeID = TypeToID(GameObject);
			m_selectObjectInfo.instanceID = child;
		}
		// ドロップ
		if (ImGui::BeginDragDropTarget()) {
			// Some processing...
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				IM_ASSERT(payload->DataSize == sizeof(GameObjectID));
				GameObjectID payload_n = *(const GameObjectID*)payload->Data;
				// 親子関係再構築
				pWorld->getGameObjectManager()->AddChild(child, payload_n);
				auto* transform = pWorld->getGameObjectManager()->getComponentData<Transform>(payload_n);
			}
			ImGui::EndDragDropTarget();
		}
		// ドラッグ
		if (ImGui::BeginDragDropSource()) {
			// Some processing...
			GameObjectID id = child;
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &id, sizeof(GameObjectID));
			ImGui::Text("Move Parent");

			ImGui::EndDragDropSource();
		}
		// ノード内
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

		// ビューポート
		Camera& camera = m_editorCamera.camera;
		//ImGuizmo::SetRect(camera.viewportOffset.x, camera.viewportOffset.y, 
		//	camera.width * camera.viewportScale, camera.height * camera.viewportScale);
		ImGuizmo::SetRect(0, 0, camera.width, camera.height);

		// ギズモ
		Matrix globalMatrix = transform.globalMatrix;
		ImGuizmo::Manipulate(&camera.view.m[0][0], &camera.projection.m[0][0], mCurrentGizmoOperation, mCurrentGizmoMode,
			&globalMatrix.m[0][0], NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

		// 変更を取得
		Vector3 newTra, newRot, newSca, oldTra, oldRot, oldSca;
		ImGuizmo::DecomposeMatrixToComponents(&globalMatrix.m[0][0], (float*)&newTra, (float*)&newRot, (float*)&newSca);
		ImGuizmo::DecomposeMatrixToComponents(&oldGlobalMatrix.m[0][0], (float*)&oldTra, (float*)&oldRot, (float*)&oldSca);
		// ローカルマトリックス
		Matrix local = globalMatrix;

		// 親のがいる場合
		auto parent = pWorld->getGameObjectManager()->GetParent(transform.id);
		if (parent != NONE_GAME_OBJECT_ID)
		{
			auto parentTrans = pWorld->getGameObjectManager()->getComponentData<Transform>(parent);
			parentTrans->globalMatrix.Invert(local);
			local = globalMatrix * local;
		}
		Vector3 locTra, locRot, locSca;
		ImGuizmo::DecomposeMatrixToComponents(&local.m[0][0], (float*)&locTra, (float*)&locRot, (float*)&locSca);

		// ローカルマトリックス更新
		transform.localMatrix = local;

		if (oldTra != newTra)
		{
			// 速度
			auto physics = pWorld->getGameObjectManager()->getComponentData<Physics>(transform.id);
			if (physics) physics->velocity = locTra - transform.translation;
			// 移動
			transform.translation = locTra;
		}
		if (oldRot != newRot)
		{
			// 回転
			Matrix invSca = Matrix::CreateScale(locSca);
			invSca = invSca.Invert();
			transform.rotation = Quaternion::CreateFromRotationMatrix(invSca * local);
		}
		else if (oldSca != newSca)
		{
			//Matrix invSca = Matrix::CreateScale(locSca);
			//invSca = invSca.Invert();
			//Quaternion rot = Quaternion::CreateFromRotationMatrix(invSca * local);
			//// 拡縮
			//Matrix invRot = Matrix::CreateFromQuaternion(rot);
			//invRot = invRot.Invert();
			//Matrix sca = local * invRot;
			//transform.scale.x = sca.m[0][0];
			//transform.scale.y = sca.m[1][1];
			//transform.scale.z = sca.m[2][2];
			////transform.scale += newSca - oldSca;
			transform.scale = locSca;
		}

		// カメラの姿勢
		ImGuizmo::ViewManipulate(&camera.view.m[0][0], camDistance, 
			ImVec2(camera.width * camera.viewportScale - 128, camera.viewportOffset.y),
			ImVec2(128, 128), 0x10101010);

		// ビュー行列から姿勢を抽出
		m_editorCamera.transform.rotation = 
			Quaternion::CreateFromRotationMatrix(camera.view.Transpose());
	}
}

//--- OpenFileDialogを利用した
void OpenFBXFile(Model &out)
{
	// ファイルの指定、読み込み
	char filename[MAX_PATH] = "";
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetActiveWindow();
	// ファイル名の格納先
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	// フィルター
	ofn.lpstrFilter =
		"All Files\0*.*\0.fbx\0*.fbx\0";
	ofn.nFilterIndex = 2;
	// フラグ
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	/* ダイアログの表示
	* ダイアログを閉じるまでは処理が
	* 返らない。「開く」を押すと戻り値で
	* TRUEが返る。それ以外はFALSE
	*/
	if (TRUE == GetOpenFileName(&ofn))
	{
		// AssimpModelTest
		AssimpLoader::get().LoadModel(Engine::get().getRendererManager(), ofn.lpstrFile, out);

		// FBXファイルの読み込み
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

	// オブジェクト生成
	Archetype archetype = Archetype::create<DynamicType, Transform, SkinnedMeshRenderer>();
	auto goID = magaer->createGameObject(node.name, archetype);
	magaer->SetParent(goID, parent);
	magaer->setComponentData(goID, Transform(goID, pos, rot, sca));
	magaer->setComponentData(goID, rd);

	if (node.meshIndexes.size() > 0)
	{
		auto& mesh = model.m_meshList[node.meshIndexes.front()];
		// ボーンを持っている
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

	// この検索
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
	//	// モデルロード
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

	//	// オブジェクト生成
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

	//	// スキンメッシュのルートボーンを検索
	//	for (auto& skin : skinMeshList)
	//	{
	//		FindRootBoneID(goMgr, parentID, skin.second, skin.first);
	//	}
	//}


	//ImGui::End();
}
