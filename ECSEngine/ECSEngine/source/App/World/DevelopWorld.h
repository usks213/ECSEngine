/*****************************************************************//**
 * \file   DevelopWorld.h
 * \brief  開発用テストワールド
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

		/// @brief スタート
		void Start() override;

		/// @brief エンド
		void End() override;

	private:

	};
}
