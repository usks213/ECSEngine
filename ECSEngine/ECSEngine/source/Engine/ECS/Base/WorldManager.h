/*****************************************************************//**
 * \file   WorldManager.h
 * \brief  ���[���h�}�l�[�W���[�̃x�[�X�N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once

class WorldManager
{
public:
	WorldManager() = default;
	virtual ~WorldManager() = default;

	void initialize();
	void finalize();

	void fixedUpdate();
	void update();
	void render();

	WorldManager(const WorldManager&) = delete;
	WorldManager(WorldManager&&) = delete;

protected:

};

