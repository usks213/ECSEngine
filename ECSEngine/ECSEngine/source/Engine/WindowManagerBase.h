/*****************************************************************//**
 * \file   WindowManagerBase.h
 * \brief  アプリケーションウィンドウのベースクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once

#include <string>


class WindowManagerBase
{
public:
	WindowManagerBase(std::string windowName, int windowWidth, int windowHeight);
	~WindowManagerBase();

	[[nodiscard]] std::string getWindowName() { return m_windowName; }
	[[nodiscard]] int getWindowWidth() { return m_windowWidth; }
	[[nodiscard]] int getWindowHeight()	{ return m_windowHeight; }

	WindowManagerBase(const WindowManagerBase&) = delete;
	WindowManagerBase(WindowManagerBase&&) = delete;

protected:
	std::string m_windowName;
	int m_windowWidth;
	int m_windowHeight;
};

