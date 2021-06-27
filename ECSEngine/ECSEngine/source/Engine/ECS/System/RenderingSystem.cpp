/*****************************************************************//**
 * \file   RenderingSystem.h
 * \brief  �����_�����O�V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "RenderingSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>

#include <Engine/Engine.h>

using namespace ecs;


/// @brief ������
void RenderingSystem::onCreate()
{

}

/// @brief �폜��
void RenderingSystem::onDestroy()
{

}

/// @brief �X�V
void RenderingSystem::onUpdate()
{
	// �����_���[�}�l�[�W���[
	auto* renderer = m_pWorld->getWorldManager()->getEngine()->getRendererManager();

	foreach<RenderData, WorldMatrix>(
		[&renderer](RenderData& rd, WorldMatrix& mtxWorld)
		{
			renderer->render(mtxWorld.value, rd.materialID, rd.meshID);
		});
}


