/*****************************************************************//**
 * \file   SystemBase.cpp
 * \brief  システムベース
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "SystemBase.h"
using namespace ecs;


/// @brief コンストラクタ
SystemBase::SystemBase(World* pWorld) : m_pWorld(pWorld)
{
}

/// @brief 生成時
void SystemBase::onCreate()
{
}

/// @brief 削除時
void SystemBase::onDestroy()
{
}

/// @brief 更新順番号取得
int SystemBase::getExecutionOrder() const
{
	return m_nExecutionOrder;
}

/// @brief エンティティマネージャー取得
/// @return エンティティマネージャー
EntityManager* SystemBase::getEntityManager() const
{
	return m_pWorld->getEntityManager();
}

/// @brief ゲームオブジェクトマネージャー取得
/// @return ゲームオブジェクトマネージャー
GameObjectManager* SystemBase::getGameObjectManager() const
{
	return m_pWorld->getGameObjectManager();
}

/// @brief 更新順設定
/// @param 更新番号
void SystemBase::setExecutionOrder(const int executionOrder)
{
	m_nExecutionOrder = executionOrder;
}
