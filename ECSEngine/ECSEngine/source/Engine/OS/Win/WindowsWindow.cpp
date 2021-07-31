/*****************************************************************//**
 * \file   WindowsWindow.cpp
 * \brief  �E�B���h�E�Y�p�E�B���h�E
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "WindowsWindow.h"
#include <resource.h>


WindowsWindow::WindowsWindow(std::string windowName, int windowWidth, int windowHeight)
	: WindowManager(windowName, windowWidth, windowHeight),
	m_hInstance(NULL), m_hWnd(NULL), m_nCmdShow(NULL)
{
}

WindowsWindow::~WindowsWindow()
{
}


bool WindowsWindow::initialize(HINSTANCE& hInstance, std::string className, int nCmdShow, WNDPROC lpfnWndProc)
{
	// �E�B���h�EEx�\����
	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		lpfnWndProc,
		0,
		0,
		hInstance,
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)),
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		nullptr,
		className.c_str(),
		nullptr
	};

	// COM������
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
		MessageBox(NULL, ("COM�̏������Ɏ��s���܂����B"), ("error"), MB_OK);
		return false;
	}

	// �C���X�^���X �n���h���ۑ�
	m_hInstance = hInstance;

	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// ��DPI�Ή�
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	// �N���C�A���g�̈�T�C�Y����E�B���h�E �T�C�Y�Z�o
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION
		| WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
	DWORD dwExStyle = 0;
	RECT rc = { 0, 0, m_windowWidth, m_windowHeight };
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// �E�B���h�E�̍쐬
	m_hWnd = CreateWindowEx(dwExStyle,
		className.c_str(),
		m_windowName.c_str(),
		dwStyle,
		CW_USEDEFAULT,		// �E�B���h�E�̍����W
		CW_USEDEFAULT,		// �E�B���h�E�̏���W
		rc.right - rc.left,	// �E�B���h�E����
		rc.bottom - rc.top,	// �E�B���h�E�c��
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);

	// �i�[
	m_className = className;
	m_nCmdShow = nCmdShow;

	return true;
}


void WindowsWindow::finalize()
{
	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(m_className.c_str(), m_hInstance);

	// COM�I������
	CoUninitialize();
}

//void UpdateButtonLayoutForDpi(HWND hWnd)
//{
//	int iDpi = GetDpiForWindow(hWnd);
//	int dpiScaledX = MulDiv(INITIALX_96DPI, iDpi, 96);
//	int dpiScaledY = MulDiv(INITIALY_96DPI, iDpi, 96);
//	int dpiScaledWidth = MulDiv(INITIALWIDTH_96DPI, iDpi, 96);
//	int dpiScaledHeight = MulDiv(INITIALHEIGHT_96DPI, iDpi, 96);
//	SetWindowPos(hWnd, hWnd, dpiScaledX, dpiScaledY, dpiScaledWidth, dpiScaledHeight, SWP_NOZORDER | SWP_NOACTIVATE);
//}