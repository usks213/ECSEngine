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
	foreach<Transform>(
		[](Transform& transform)
		{
			// �g�k
			transform.localMatrix = Matrix::CreateScale(transform.scale);
			// ��]
			transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
			// �ړ�
			transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
		});
}


