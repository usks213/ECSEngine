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
			//transform.localMatrix = Matrix::CreateScale(sca.value);
			transform.localScale = transform.globalScale = sca.value;
			// ��]
			transform.localMatrix = Matrix::CreateFromQuaternion(rot.value);
			// �ړ�
			transform.localMatrix *= Matrix::CreateTranslation(pos.value);
			// �O���[�o���}�g���b�N�X�X�V
			transform.globalMatrix = transform.localMatrix;
		});
}


