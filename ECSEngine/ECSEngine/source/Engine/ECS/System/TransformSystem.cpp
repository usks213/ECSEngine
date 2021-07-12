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
	foreach<Position, Rotation, Scale, LocalToWorld>(
		[](Position& pos, Rotation& rot, Scale& sca, LocalToWorld& mtxWorld)
		{
			// 拡縮
			mtxWorld.value = Matrix::CreateScale(sca.value);
			// 回転
			mtxWorld.value *= Matrix::CreateFromQuaternion(rot.value);
			// 移動
			mtxWorld.value *= Matrix::CreateTranslation(pos.value);
		});
}


