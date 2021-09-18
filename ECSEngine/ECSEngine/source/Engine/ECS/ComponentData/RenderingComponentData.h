/*****************************************************************//**
 * \file   RenderingComponentData.h
 * \brief  レンダリングコンポーネンデータ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Renderer/Base/Material.h>
#include <Engine/Renderer/Base/Mesh.h>

namespace ecs {

	struct RenderData : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(RenderData);
		MaterialID	materialID;
		MeshID		meshID;
		RenderData():materialID(NONE_MATERIAL_ID), meshID(NONE_MESH_ID){}
		RenderData(const MaterialID& matID, const MeshID& meshID):
			materialID(matID), meshID(meshID)
		{}
	};

	struct SkinnedMeshRenderer : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(SkinnedMeshRenderer);
		MaterialID	materialID;
		MeshID		meshID;
		// ルートボーン Boneのトランスフォーム
		// BoneID or GameObjectID

		// コンストラクタ
		SkinnedMeshRenderer() :materialID(NONE_MATERIAL_ID), meshID(NONE_MESH_ID) {}
		SkinnedMeshRenderer(const MaterialID& matID, const MeshID& meshID) :
			materialID(matID), meshID(meshID)
		{}
	};

}