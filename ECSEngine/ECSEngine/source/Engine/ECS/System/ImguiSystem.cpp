/*****************************************************************//**
 * \file   ImguiSystem.h
 * \brief  imgui表示システム
 *
 * \author USAMI KOSHI
 * \date   2021/07/05
 *********************************************************************/

#include "ImguiSystem.h"
#include "imgui.h"
#include "ImGuizmo.h"

#include <string>
#include <Engine/Engine.h>
#include <Engine/ECS/Base/GameObjectManager.h>

#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/ComponentTag.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/PhysicsComponentData.h>

using namespace ecs;

/// @brief 型チェック
#define CheckType(Type) typeName == TypeToString(Type)


 /// @brief 生成時
void ImguiSystem::onCreate()
{

}

/// @brief 削除時
void ImguiSystem::onDestroy()
{

}

/// @brief 更新
void ImguiSystem::onUpdate()
{
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
			getGameObjectManager()->ResetParent(payload_n);
		}
		ImGui::EndDragDropTarget();
	}

	// ルートオブジェクト
	for (auto& root : m_pWorld->getGameObjectManager()->getRootList())
	{
		GameObject* gameObject = m_pWorld->getGameObjectManager()->getGameObjectMap()[root].get();
		std::string name(gameObject->getName());
		std::size_t childNum = gameObject->getChildCount(); // 子がいるか

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			(m_selectObjectID == root ? ImGuiTreeNodeFlags_Selected : 0) | (childNum ? 0 : ImGuiTreeNodeFlags_Leaf));
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
			m_selectObjectID = root;
		}
		// ドロップ
		if (ImGui::BeginDragDropTarget()) {
			// Some processing...
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				IM_ASSERT(payload->DataSize == sizeof(GameObjectID));
				GameObjectID payload_n = *(const GameObjectID*)payload->Data;
				// 親子関係再構築
				getGameObjectManager()->AddChild(root, payload_n);
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

	m_MainCamera = nullptr;
	m_MaimCameraTransform = nullptr;
	// メインカメラの取得
	foreach<Camera, Transform>([this](Camera& camera, Transform& transform)
		{
			m_MainCamera = &camera;
			m_MaimCameraTransform = &transform;
		});


	//----- インスペクター -----
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Inspector");
	if (m_selectObjectID != NONE_GAME_OBJECT_ID)
	{
		// 選択オブジェクト
		auto* curObject = getGameObjectManager()->getGameObjectMap()[m_selectObjectID].get();
		auto& chunk = m_pWorld->getChunkList()[curObject->m_chunkIndex];

		Transform* transform = nullptr;
		const auto& archetype = chunk.getArchetype();

		// トランスフォームデータを持っているか
		for (int i = 0; i < archetype.getArchetypeSize(); ++i)
		{
			const auto& type = archetype.getTypeInfo(i);
			std::string_view typeName = type.getName();
			void* data = chunk.getComponentData(type.getName(), curObject->m_index);
			if (CheckType(Transform))		transform = (Transform*)data;
		}

		// トランスフォームデータ情報
		if (m_MainCamera && m_MaimCameraTransform &&
			m_MaimCameraTransform->id != m_selectObjectID)
		{
			if (transform)
			{
				EditTransform(*m_MainCamera, *transform);
			}
		}

		// コンポーネントデータ情報
		for (int i = 0; i < archetype.getArchetypeSize(); ++i)
		{
			const auto& type = archetype.getTypeInfo(i);
			ImGui::Text(type.getName().data());
			DispGui(type.getName(), chunk.getComponentData(type.getName(), curObject->m_index));
		}
	}
	ImGui::End();
}


void ImguiSystem::DispChilds(const GameObjectID parentID)
{
	int nIndex = 0;
	for (auto& child : m_pWorld->getGameObjectManager()->GetChilds(parentID))
	{
		GameObject* gameObject = m_pWorld->getGameObjectManager()->getGameObjectMap()[child].get();
		std::string name(gameObject->getName());
		std::size_t childNum = gameObject->getChildCount(); // 子がいるか

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_FramePadding | 
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			(m_selectObjectID == child ? ImGuiTreeNodeFlags_Selected : 0) | (childNum ? 0 : ImGuiTreeNodeFlags_Leaf));
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
			m_selectObjectID = child;
		}
		// ドロップ
		if (ImGui::BeginDragDropTarget()) {
			// Some processing...
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				IM_ASSERT(payload->DataSize == sizeof(GameObjectID));
				GameObjectID payload_n = *(const GameObjectID*)payload->Data;
				// 親子関係再構築
				getGameObjectManager()->AddChild(child, payload_n);
				auto* transform = getGameObjectManager()->getComponentData<Transform>(payload_n);
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


/// @brief GUI表示・入力
/// @param typeName 型名
/// @param data データ
void ImguiSystem::DispGui(std::string_view typeName, void* data)
{
/*	if (CheckType(Position))
	{
		ImGui::DragFloat3(typeName.data(), (float*)data);
	}
	else if (CheckType(Rotation))
	{
		Rotation* rot = static_cast<Rotation*>(data);
		Matrix mat = Matrix::CreateFromQuaternion(rot->value);
		float tx, ty, tz;
		if (mat.m[2][1] == 1.f)
		{
			tx = XM_PI * 0.5f;
			ty = 0;
			tz = atan2f(mat.m[1][0], mat.m[0][0]);
		}
		else if (mat.m[2][1] == -1.f)
		{
			tx = -XM_PI * 0.5f;
			ty = 0;
			tz = atan2f(mat.m[1][0], mat.m[0][0]);
		}
		else
		{
			tx = asinf(-mat.m[2][1]);
			ty = atan2f(mat.m[2][0], mat.m[2][2]);
			tz = atan2f(mat.m[0][1], mat.m[1][1]);
		}
		tx = XMConvertToDegrees(tx);
		ty = XMConvertToDegrees(ty);
		tz = XMConvertToDegrees(tz);
		Vector3 temp(tx, ty, tz);

		ImGui::DragFloat3(typeName.data(), (float*)&temp);
		Quaternion q = Quaternion::CreateFromYawPitchRoll(
			XMConvertToRadians(temp.y), XMConvertToRadians(temp.x), XMConvertToRadians(temp.z));
		rot->value = q;
	}
	else if (CheckType(Scale))
	{
		ImGui::DragFloat3(typeName.data(), (float*)data);
	}
	//else *///if (CheckType(LocalToWorld))
	//{
	//	EditTransform(*m_MainCamera, ((LocalToWorld*)data)->value);
	//}

}
//
//void EditTransform(Camera& camera,  Matrix& matrix)
//{
//	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
//	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
//	if (ImGui::IsKeyPressed(90))
//		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//	if (ImGui::IsKeyPressed(69))
//		mCurrentGizmoOperation = ImGuizmo::ROTATE;
//	if (ImGui::IsKeyPressed(82)) // r Key
//		mCurrentGizmoOperation = ImGuizmo::SCALE;
//	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
//		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//	ImGui::SameLine();
//	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
//		mCurrentGizmoOperation = ImGuizmo::ROTATE;
//	ImGui::SameLine();
//	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
//		mCurrentGizmoOperation = ImGuizmo::SCALE;
//	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
//	ImGuizmo::DecomposeMatrixToComponents(&matrix.m[0][0], matrixTranslation, matrixRotation, matrixScale);
//	ImGui::InputFloat3("Tr", matrixTranslation);
//	ImGui::InputFloat3("Rt", matrixRotation);
//	ImGui::InputFloat3("Sc", matrixScale);
//	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &matrix.m[0][0]);
//
//	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
//	{
//		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
//			mCurrentGizmoMode = ImGuizmo::LOCAL;
//		ImGui::SameLine();
//		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
//			mCurrentGizmoMode = ImGuizmo::WORLD;
//	}
//	static bool useSnap(false);
//	if (ImGui::IsKeyPressed(83))
//		useSnap = !useSnap;
//	ImGui::Checkbox("", &useSnap);
//	ImGui::SameLine();
//	//Vector3 snap;
//	//switch (mCurrentGizmoOperation)
//	//{
//	//case ImGuizmo::TRANSLATE:
//	//	snap = config.mSnapTranslation;
//	//	ImGui::InputFloat3("Snap", &snap.x);
//	//	break;
//	//case ImGuizmo::ROTATE:
//	//	snap = config.mSnapRotation;
//	//	ImGui::InputFloat("Angle Snap", &snap.x);
//	//	break;
//	//case ImGuizmo::SCALE:
//	//	snap = config.mSnapScale;
//	//	ImGui::InputFloat("Scale Snap", &snap.x);
//	//	break;
//	//}
//
//	ImGuiIO& io = ImGui::GetIO();
//	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
//	ImGuizmo::Manipulate(&camera.view.m[0][0], &camera.projection.m[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &matrix.m[0][0]);
//
//}

void ImguiSystem::EditTransform(Camera& camera, Transform& transform)
{
	bool editTransformDecomposition = true;
	Matrix oldGlobalMatrix = transform.globalMatrix;

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	static bool useWindow = false;
	int gizmoCount = 1;
	float camDistance = 8.0f;

	if (editTransformDecomposition)
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
	}

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	if (useWindow)
	{
		ImGui::SetNextWindowSize(ImVec2(800, 400));
		ImGui::SetNextWindowPos(ImVec2(400, 20));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
		ImGui::Begin("Gizmo", 0, ImGuiWindowFlags_NoMove);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
		viewManipulateTop = ImGui::GetWindowPos().y;
	}
	else
	{
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}

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
	auto parent = getGameObjectManager()->GetParent(transform.id);
	if (parent != NONE_GAME_OBJECT_ID)
	{
		auto parentTrans = getGameObjectManager()->getComponentData<Transform>(parent);
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
		auto physics = getGameObjectManager()->getComponentData<Physics>(transform.id);
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

	ImGuizmo::ViewManipulate(&camera.view.m[0][0], camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

	if (useWindow)
	{
		ImGui::End();
		ImGui::PopStyleColor(1);
	}
}

