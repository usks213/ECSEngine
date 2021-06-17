/*****************************************************************//**
 * \file   WindowsWindow.h
 * \brief  ウィンドウズ用ウィンドウ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once

#include <Engine/OS/Base/WindowManagerBase.h>
#include <Windows.h>


class WindowsWindow final : public WindowManagerBase
{
public:
	WindowsWindow(std::string windowName, int windowWidth, int windowHeight);
	~WindowsWindow();

	[[nodiscard]] bool initialize(HINSTANCE& hInstance, std::string className, int nCmdShow, WNDPROC lpfnWndProc);

	void finalize();

	HWND getWindowHandle() { return m_hWnd; }

	WindowsWindow(const WindowsWindow&) = delete;
	WindowsWindow(WindowsWindow&&) = delete;
private:
	HINSTANCE   m_hInstance;
	HWND		m_hWnd;
	std::string m_className;
	int m_nCmdShow;				// フルスクリーン切り替えで使うかも？
};
