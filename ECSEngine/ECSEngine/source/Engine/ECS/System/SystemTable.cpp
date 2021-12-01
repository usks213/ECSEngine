/*****************************************************************//**
 * \file   SystemTable.h
 * \brief  システムテーブル
 *
 * \author USAMI KOSHI
 * \date   2021/08/03
 *********************************************************************/

#include "SystemTable.h"

#include "AnimationSystem.h"
#include "PhysicsSystem.h"
#include "TransformSystem.h"

using namespace ecs;

#define PUSU_SYSTEM(T) m_makeSystemTable.emplace(T::getTypeHash(), T::Create)


/// @brief コンストラクタ
SystemTable::SystemTable()
{
	// デフォルトのシステムを格納
	PUSU_SYSTEM(AnimationSystem);
	PUSU_SYSTEM(PhysicsSystem);
	PUSU_SYSTEM(TransformSystem);

}
