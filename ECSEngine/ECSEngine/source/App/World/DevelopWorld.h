/*****************************************************************//**
 * \file   DevelopWorld.h
 * \brief  �J���p�e�X�g���[���h
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/World.h>

namespace ecs {

	class DevelopWorld final : public World
	{
	public:
		DevelopWorld(WorldManager* pWorldManager) :
			World(pWorldManager)
		{}
		~DevelopWorld() = default;

		/// @brief �X�^�[�g
		void Start() override;

		/// @brief �G���h
		void End() override;

	private:

	};
}
