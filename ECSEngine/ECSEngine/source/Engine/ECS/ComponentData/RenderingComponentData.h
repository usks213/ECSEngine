/*****************************************************************//**
 * \file   RenderingComponentData.h
 * \brief  �����_�����O�R���|�[�l���f�[�^
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
		//std::vector<MaterialID>	materialIDs;	// �T�u���b�V���g�p��
		MaterialID materialID;
		MeshID		meshID;
		GameObjectID rootObjectID;

		// �R���X�g���N�^
		SkinnedMeshRenderer() : materialID(NONE_MATERIAL_ID), meshID(NONE_MESH_ID),
			rootObjectID(NONE_GAME_OBJECT_ID)
		{}
		SkinnedMeshRenderer(const MaterialID& matID, const MeshID& meshID, 
			const GameObjectID rootID) :
			materialID(matID), meshID(meshID), rootObjectID(rootID)
		{}
	};

}