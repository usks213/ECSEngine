/*****************************************************************//**
 * \file   AnimationComponentData.h
 * \brief  �A�j���[�V�����R���|�[�l���g�f�[�^
 * 
 * \author USAMI KOSHI
 * \date   2021/09/30
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Renderer/Base/Animation.h>


namespace ecs {

	/// @brief ���W�b�h�{�f�B
	struct Animator : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Animator);

		// �A�j���[�V����ID
		AnimationID animationID;
		// �A�o�^�[ID

		double time;

		/// @brief �R���X�g���N�^
		Animator()
		{
		}
	};
}