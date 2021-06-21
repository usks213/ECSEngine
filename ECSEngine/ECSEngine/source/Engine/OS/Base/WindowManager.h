/*****************************************************************//**
 * \file   WindowManager.h
 * \brief  アプリケーションウィンドウのベースクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once

#include <string>


class WindowManager
{
public:
	WindowManager(std::string windowName, int windowWidth, int windowHeight);
	virtual ~WindowManager();

	virtual void finalize() = 0;

public:
	[[nodiscard]] std::string getWindowName() { return m_windowName; }
	[[nodiscard]] int getWindowWidth() { return m_windowWidth; }
	[[nodiscard]] int getWindowHeight()	{ return m_windowHeight; }

	WindowManager(const WindowManager&) = delete;
	WindowManager(WindowManager&&) = delete;

protected:
	std::string m_windowName;
	int m_windowWidth;
	int m_windowHeight;
};

