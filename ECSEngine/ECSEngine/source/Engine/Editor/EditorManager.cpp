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
#include <Engine/Renderer/D3D11/D3D11RenderTarget.h>
#include <Engine/ECS/Base/RenderPipeline.h>
#include <Engine/ECS/System/TransformSystem.h>

#include <Engine/ECS/Base/WorldManager.h>
#include <Engine/ECS/Base/GameObjectManager.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/ComponentTag.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/PhysicsComponentData.h>

#include <Engine/ECS/System/PhysicsSystem.h>


using namespace ecs;

#define CheckType(Type) typeHash == TypeToHash(Type)


void EditorManager::initialize()
{
	auto* renderer = m_pEngine->getRendererManager();

	Transform transform(0);
	transform.translation = Vector3(0 ,10, -10);
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
	ImGui::Begin("Hierarchy");

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
	ImGui::Begin("Inspector");

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
				ImGui::Text(std::to_string(type.getHash()).c_str());
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
	// ワールドマネージャー取得
	auto* pWorldManager = m_pEngine->getWorldManager();
	// 現在のワールド取得
	auto* pWorld = pWorldManager->getCurrentWorld();
	// パイプライン取得
	auto* pipeline = pWorld->getRenderPipeline();

	// カリング
	pipeline->cullingPass(m_editorCamera.camera);

	// 描画
	pipeline->beginPass(m_editorCamera.camera);
	pipeline->prePass(m_editorCamera.camera);
	pipeline->gbufferPass(m_editorCamera.camera);
	pipeline->shadowPass(m_editorCamera.camera);
	pipeline->opaquePass(m_editorCamera.camera);
	pipeline->skyPass(m_editorCamera.camera);
	pipeline->transparentPass(m_editorCamera.camera);
	pipeline->postPass(m_editorCamera.camera);


	// BulletDebugDraw
	auto* physicsSytem = pWorld->getSystem<PhysicsSystem>();
	if (physicsSytem)
	{
		physicsSytem->debugDraw();
	}

	pipeline->endPass(m_editorCamera.camera);
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
			moveSpeed *= 2;
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
		ImGuizmo::SetRect(camera.viewportOffset.x, camera.viewportOffset.y, 
			camera.width * camera.viewportScale, camera.height * camera.viewportScale);

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