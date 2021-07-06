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

#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/ComponentTag.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>

using namespace ecs;

/// @brief 型チェック
#define CheckType(Type) typeName == TypeToString(Type)
void EditTransform(Camera& camera, Matrix& matrix);


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
	ImGui::SetNextWindowBgAlpha(0.4f);
	ImGui::Begin("Entity");

	foreach<Camera>([this](Camera& camera)
		{
			m_MainCamera = &camera;
		});

	for (auto* chunk : getEntityManager()->getChunkList<Name>())
	{
		auto names = chunk->getComponentArray<Name>();
		for (std::uint32_t index = 0; index < names.Count(); ++index)
		{
			std::string name(names[index].value);
			if (ImGui::TreeNode((name + std::to_string(index)).c_str()))
			{
				const auto& archetype = chunk->getArchetype();
				for (int i = 0; i < archetype.getArchetypeSize(); ++i)
				{
					const auto& type = archetype.getTypeInfo(i);
					DispGui(type.getName(), chunk->getComponentData(type.getName(), index));
				}
				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}


/// @brief GUI表示・入力
/// @param typeName 型名
/// @param data データ
void ImguiSystem::DispGui(std::string_view typeName, void* data)
{
	if (CheckType(Position))
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
	else if (CheckType(WorldMatrix))
	{
		//EditTransform(*m_MainCamera, ((WorldMatrix*)data)->value);
	}
}

void EditTransform(Camera& camera,  Matrix& matrix)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&matrix.m[0][0], matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &matrix.m[0][0]);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	//Vector3 snap;
	//switch (mCurrentGizmoOperation)
	//{
	//case ImGuizmo::TRANSLATE:
	//	snap = config.mSnapTranslation;
	//	ImGui::InputFloat3("Snap", &snap.x);
	//	break;
	//case ImGuizmo::ROTATE:
	//	snap = config.mSnapRotation;
	//	ImGui::InputFloat("Angle Snap", &snap.x);
	//	break;
	//case ImGuizmo::SCALE:
	//	snap = config.mSnapScale;
	//	ImGui::InputFloat("Scale Snap", &snap.x);
	//	break;
	//}

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(&camera.view.m[0][0], &camera.projection.m[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &matrix.m[0][0]);

}