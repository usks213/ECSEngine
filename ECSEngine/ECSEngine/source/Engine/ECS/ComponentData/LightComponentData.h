/*****************************************************************//**
 * \file   LightComponentData.h
 * \brief  ���C�g�R���|�[�l���g�f�[�^
 * 
 * \author USAMI KOSHI
 * \date   2021/08/12
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Renderer/Base/LightData.h>
#include <Engine/Renderer/Base/DepthStencil.h>

namespace ecs {

	struct DirectionalLight : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(DirectionalLight);
		DirectionalLightData	data;			// ���s�����f�[�^
		DepthStencilID			depthStencilID;	// �[�x�X�e���V��ID
	};

	struct PointLight : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(PointLight);
		PointLightData	data;
	};

	struct SpotLight : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(SpotLight);
		SpotLightData	data;
	};
}