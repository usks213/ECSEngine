/*****************************************************************//**
 * \file   WindowsWindow.cpp
 * \brief  ウィンドウズ用ウィンドウ
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
	// ウィンドウEx構造体
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

	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
		MessageBox(NULL, ("COMの初期化に失敗しました。"), ("error"), MB_OK);
		return false;
	}

	// インスタンス ハンドル保存
	m_hInstance = hInstance;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// 高DPI対応
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	// クライアント領域サイズからウィンドウ サイズ算出
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION
		| WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
	DWORD dwExStyle = 0;
	RECT rc = { 0, 0, m_windowWidth, m_windowHeight };
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// ウィンドウの作成
	m_hWnd = CreateWindowEx(dwExStyle,
		className.c_str(),
		m_windowName.c_str(),
		dwStyle,
		CW_USEDEFAULT,		// ウィンドウの左座標
		CW_USEDEFAULT,		// ウィンドウの上座標
		rc.right - rc.left,	// ウィンドウ横幅
		rc.bottom - rc.top,	// ウィンドウ縦幅
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);

	// 格納
	m_className = className;
	m_nCmdShow = nCmdShow;

	return true;
}


void WindowsWindow::finalize()
{
	// ウィンドウクラスの登録を解除
	UnregisterClass(m_className.c_str(), m_hInstance);

	// COM終了処理
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