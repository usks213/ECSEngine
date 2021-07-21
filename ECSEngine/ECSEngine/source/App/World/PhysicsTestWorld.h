/*****************************************************************//**
 * \file   PhysicsTestWorld.h
 * \brief  物理テストワールド
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

		/// @brief スタート
		void Start() override;

		/// @brief エンド
		void End() override;

	private:

	};
}
