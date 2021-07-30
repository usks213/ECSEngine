/*****************************************************************//**
 * \file   WorldManager.h
 * \brief  ワールドマネージャーのベースクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once

#include "World.h"

class Engine;

class WorldManager final
{
	friend class Engine;
public:
	WorldManager(Engine* pEngine) :
		m_pEngine(pEngine)
	{
	}

	~WorldManager() = default;

	void initialize();
	void finalize();

	void fixedUpdate();
	void update();
	void render();

	WorldManager(const WorldManager&) = delete;
	WorldManager(WorldManager&&) = delete;

public:

	template<class T, typename = std::enable_if_t<std::is_base_of_v<ecs::World, T>>>
	void LoadWorld() {
		m_pWorld = std::make_unique<T>(this);
		m_pWorld->Start();
	}

	/// @brief エンジンの取得
	Engine* getEngine() { return m_pEngine; }

	/// @brief 現在のワールド取得
	ecs::World* getCurrentWorld() { return m_pWorld.get(); }

protected:
	Engine* m_pEngine;
	std::unique_ptr<ecs::World>	m_pWorld;
};

