/*****************************************************************//**
 * \file   SystemTable.h
 * \brief  �V�X�e���e�[�u��
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


/// @brief �R���X�g���N�^
SystemTable::SystemTable()
{
	// �f�t�H���g�̃V�X�e�����i�[
	PUSU_SYSTEM(AnimationSystem);
	PUSU_SYSTEM(PhysicsSystem);
	PUSU_SYSTEM(TransformSystem);

}
