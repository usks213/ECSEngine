/*****************************************************************//**
 * \file   GameObject.h
 * \brief  �Q�[���I�u�W�F�N�g
 *
 * \author USAMI KOSHI
 * \date   2021/07/13
 *********************************************************************/

#include "GameObject.h"

using namespace ecs;

/// @brief �C�x���g�̊i�[
/// @param  �C�x���g�֐�(name:�C�x���g��, value:�l)
void GameObject::registerEvent(std::function<void(std::string_view name, void* value)> event)
{
	m_eventList.push_back(event);
}

/// @brief �C�x���g�̌Ăяo��
/// @param name �C�x���g��
/// @param value �l
void GameObject::invokeEvent(std::string_view name, void* value)
{
	for (auto& event : m_eventList)
	{
		event(name, value);
	}

	m_eventList.clear();
}
