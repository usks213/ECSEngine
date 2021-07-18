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
			transform.localToWorld = Matrix::CreateScale(transform.scale);
			//transform.localScale = transform.scale;
			// 回転
			transform.localToWorld *= Matrix::CreateFromQuaternion(transform.rotation);
			// 移動
			transform.localToWorld *= Matrix::CreateTranslation(transform.translation);
		});
}


