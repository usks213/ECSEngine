/*****************************************************************//**
 * \file   WorldManager.cpp
 * \brief  ワールドマネージャーのベースクラス
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "WorldManager.h"
#include <Engine/Engine.h>


void WorldManager::initialize()
{
	
}

void WorldManager::finalize()
{

}

void WorldManager::fixedUpdate()
{

}

void WorldManager::update()
{
	m_pWorld->update();
}

void WorldManager::render()
{
	
}


