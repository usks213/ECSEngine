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
	};

}