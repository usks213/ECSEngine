/*****************************************************************//**
 * \file   GameObject.h
 * \brief  ゲームオブジェクト
 *
 * \author USAMI KOSHI
 * \date   2021/07/13
 *********************************************************************/

#include "GameObject.h"

using namespace ecs;

/// @brief イベントの格納
/// @param  イベント関数(name:イベント名, value:値)
void GameObject::registerEvent(std::function<void(std::string_view name, void* value)> event)
{
	m_eventList.push_back(event);
}

/// @brief イベントの呼び出し
/// @param name イベント名
/// @param value 値
void GameObject::invokeEvent(std::string_view name, void* value)
{
	for (auto& event : m_eventList)
	{
		event(name, value);
	}

	m_eventList.clear();
}
