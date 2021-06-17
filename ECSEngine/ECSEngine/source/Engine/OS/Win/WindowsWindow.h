/*****************************************************************//**
 * \file   WindowsWindow.h
 * \brief  �E�B���h�E�Y�p�E�B���h�E
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#pragma once

#include <Engine/WindowManagerBase.h>
#include <Windows.h>


class WindowsWindow final : public WindowManagerBase
{
public:
	WindowsWindow(std::string windowName, int windowWidth, int windowHeight);
	~WindowsWindow();

	[[nodiscard]] bool initialize(HINSTANCE& hInstance, std::string className, int nCmdShow, WNDPROC lpfnWndProc);

	void finalize();

	WindowsWindow(const WindowsWindow&) = delete;
	WindowsWindow(WindowsWindow&&) = delete;

private:
	HINSTANCE   m_hInstance;
	HWND		m_hWnd;
	std::string m_className;
	int m_nCmdShow;				// �t���X�N���[���؂�ւ��Ŏg�������H
};
