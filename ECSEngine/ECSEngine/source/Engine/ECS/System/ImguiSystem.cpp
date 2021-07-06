/*****************************************************************//**
 * \file   ImguiSystem.h
 * \brief  imgui�\���V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/07/05
 *********************************************************************/

#include "ImguiSystem.h"
#include "imgui.h"
#include <string>

#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/ComponentTag.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>

using namespace ecs;

/// @brief �^�`�F�b�N
#define CheckType(Type) typeName == TypeToString(Type)


 /// @brief ������
void ImguiSystem::onCreate()
{

}

/// @brief �폜��
void ImguiSystem::onDestroy()
{

}

/// @brief �X�V
void ImguiSystem::onUpdate()
{
	ImGui::Begin("Entity", 0, ImGuiConfigFlags_DockingEnable);
	ImGuiConfigFlags_DockingEnable;
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


/// @brief GUI�\���E����
/// @param typeName �^��
/// @param data �f�[�^
void ImguiSystem::DispGui(std::string_view typeName, void* data)
{
	if (CheckType(Position))
	{
		ImGui::InputFloat3(typeName.data(), (float*)data);
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

		ImGui::SliderFloat3(typeName.data(), (float*)&temp, -180.0f, 180.0f);
		Quaternion q = Quaternion::CreateFromYawPitchRoll(
			XMConvertToRadians(temp.y), XMConvertToRadians(temp.x), XMConvertToRadians(temp.z));
		rot->value = q;
	}
	else if (CheckType(Scale))
	{
		ImGui::InputFloat3(typeName.data(), (float*)data);
	}
}
