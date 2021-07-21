/*****************************************************************//**
 * \file   PhysicsTestWorld.h
 * \brief  �����e�X�g���[���h
 * 
 * \author USAMI KOSHI
 * \date   2021/07/19
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/World.h>

namespace ecs {

	class PhysicsTestWorld final : public World
	{
	public:
		PhysicsTestWorld(WorldManager* pWorldManager) :
			World(pWorldManager)
		{}
		~PhysicsTestWorld() = default;

		/// @brief �X�^�[�g
		void Start() override;

		/// @brief �G���h
		void End() override;

	private:

	};
}
