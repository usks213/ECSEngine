/*****************************************************************//**
 * \file   WindowManager.cpp
 * \brief  アプリケーションウィンドウのベースクラス
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "WindowManager.h"

WindowManager::WindowManager(std::string windowName, int windowWidth, int windowHeight)
	: m_windowName(windowName), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
}

WindowManager::~WindowManager()
{
}