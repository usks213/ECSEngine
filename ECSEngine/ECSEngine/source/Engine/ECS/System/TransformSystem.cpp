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
	foreach<Position, Rotation, Scale, Transform>(
		[](Position& pos, Rotation& rot, Scale& sca, Transform& transform)
		{
			// �g�k
			transform.LocalToWorld = Matrix::CreateScale(sca.value);
			// ��]
			transform.LocalToWorld *= Matrix::CreateFromQuaternion(rot.value);
			// �ړ�
			transform.LocalToWorld *= Matrix::CreateTranslation(pos.value);
		});
}


