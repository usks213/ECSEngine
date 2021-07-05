/*****************************************************************//**
 * \file   ImguiSystem.h
 * \brief  imgui�\���V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/07/05
 *********************************************************************/

#include "ImguiSystem.h"
#include "imgui.h"

using namespace ecs;


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
	ImGui::Begin("Entity");

	for (auto& chunk : m_pWorld->getChunkList())
	{
		if (ImGui::TreeNode("archetype")) 
		{
			const auto& archetype = chunk.getArchetype();
			for (int i = 0; i < archetype.getArchetypeSize(); ++i)
			{
				const auto& type = archetype.getTypeInfo(i);
				ImGui::Text("Type:%u", type.getHash());
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
}


