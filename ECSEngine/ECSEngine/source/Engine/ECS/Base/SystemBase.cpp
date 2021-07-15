/*****************************************************************//**
 * \file   SystemBase.cpp
 * \brief  �V�X�e���x�[�X
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "SystemBase.h"
using namespace ecs;


/// @brief �R���X�g���N�^
SystemBase::SystemBase(World* pWorld) : m_pWorld(pWorld)
{
}

/// @brief ������
void SystemBase::onCreate()
{
}

/// @brief �폜��
void SystemBase::onDestroy()
{
}

/// @brief �X�V���ԍ��擾
int SystemBase::getExecutionOrder() const
{
	return m_nExecutionOrder;
}

/// @brief �G���e�B�e�B�}�l�[�W���[�擾
/// @return �G���e�B�e�B�}�l�[�W���[
EntityManager* SystemBase::getEntityManager() const
{
	return m_pWorld->getEntityManager();
}

/// @brief �Q�[���I�u�W�F�N�g�}�l�[�W���[�擾
/// @return �Q�[���I�u�W�F�N�g�}�l�[�W���[
GameObjectManager* SystemBase::getGameObjectManager() const
{
	return m_pWorld->getGameObjectManager();
}

/// @brief �X�V���ݒ�
/// @param �X�V�ԍ�
void SystemBase::setExecutionOrder(const int executionOrder)
{
	m_nExecutionOrder = executionOrder;
}
