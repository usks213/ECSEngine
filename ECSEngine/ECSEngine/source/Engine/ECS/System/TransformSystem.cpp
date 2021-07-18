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
			transform.localToWorld = Matrix::CreateScale(transform.scale);
			//transform.localScale = transform.scale;
			// ��]
			transform.localToWorld *= Matrix::CreateFromQuaternion(transform.rotation);
			// �ړ�
			transform.localToWorld *= Matrix::CreateTranslation(transform.translation);
		});
}


