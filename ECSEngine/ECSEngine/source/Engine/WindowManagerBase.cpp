/*****************************************************************//**
 * \file   WindowManagerBase.cpp
 * \brief  アプリケーションウィンドウのベースクラス
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "WindowManagerBase.h"

WindowManagerBase::WindowManagerBase(std::string windowName, int windowWidth, int windowHeight)
	: m_windowName(windowName), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
}

WindowManagerBase::~WindowManagerBase()
{
}