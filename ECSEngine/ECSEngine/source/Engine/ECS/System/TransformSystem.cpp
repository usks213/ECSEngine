/*****************************************************************//**
 * \file   TransformSystem.h
 * \brief  トランスフォームシステム
 *
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "TransformSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>

using namespace ecs;


 /// @brief 生成時
void TransformSystem::onCreate()
{

}

/// @brief 削除時
void TransformSystem::onDestroy()
{

}

/// @brief 更新
void TransformSystem::onUpdate()
{
	foreach<Transform>(
		[](Transform& transform)
		{
			// 拡縮
			transform.localMatrix = Matrix::CreateScale(transform.scale);
			// 回転
			transform.localMatrix *= Matrix::CreateFromQuaternion(transform.rotation);
			// 移動
			transform.localMatrix *= Matrix::CreateTranslation(transform.translation);
		});
}


