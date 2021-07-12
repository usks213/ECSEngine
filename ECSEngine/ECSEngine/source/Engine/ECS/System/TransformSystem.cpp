/*****************************************************************//**
 * \file   TransformSystem.h
 * \brief  �g�����X�t�H�[���V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "TransformSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>

using namespace ecs;


 /// @brief ������
void TransformSystem::onCreate()
{

}

/// @brief �폜��
void TransformSystem::onDestroy()
{

}

/// @brief �X�V
void TransformSystem::onUpdate()
{
	foreach<Position, Rotation, Scale, LocalToWorld>(
		[](Position& pos, Rotation& rot, Scale& sca, LocalToWorld& mtxWorld)
		{
			// �g�k
			mtxWorld.value = Matrix::CreateScale(sca.value);
			// ��]
			mtxWorld.value *= Matrix::CreateFromQuaternion(rot.value);
			// �ړ�
			mtxWorld.value *= Matrix::CreateTranslation(pos.value);
		});
}


