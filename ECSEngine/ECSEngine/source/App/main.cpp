/*****************************************************************//**
 * \file   main.cpp
 * \brief  ECSEngine
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/


//===== インクルード =====
#include <windows.h>

#include <Engine/Engine.h>
#include <Engine/OS/Win/WindowsWindow.h>
#include <Engine/Renderer/D3D11/D3D11RendererManager.h>
#include <Engine/ECS/Base/WorldManager.h>

#include <Engine/Utility/Input.h>

#include <App/World/DevelopWorld.h>

// imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

//-------- ライブラリのリンク
#pragma comment(lib, "imm32")


//===== 定数定義 =====


//===== プロトタイプ宣言 =====
// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ウィンドウ初期化
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
// imgui
IMGUI_IMPL_API LRESULT   ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/// @brief エントリポイント
/// @param hInstance		インスタンスハンドル
/// @param hPrevInstance	前回のインスタンスハンドル
/// @param lpCmdLine		コマンドライン
/// @param nCmdShow			ウィンドウ状態
/// @return 終了コード
int WINAPI WinMain(	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
					_In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 未使用宣言
	UNREFERENCED_PARAMETER(lpCmdLine);		// 未使用宣言

	// 入力
	InitInput();

	// エンジン取得
	auto& engine = Engine::get();
	// ウィンドウの生成
	WindowsWindow* pWin = engine.createWindow<WindowsWindow>("ECSEngine", 1920, 1080);
	// レンダラーの生成
	D3D11RendererManager* pRenderer = engine.createRenderer<D3D11RendererManager>();

	// ウィンドウの初期化
	pWin->initialize(hInstance, "ECSAppClass", nCmdShow, WndProc);
	// レンダラーの初期化
	pRenderer->initialize(pWin->getWindowHandle(), pWin->getWindowWidth(), pWin->getWindowHeight());
	// エンジンの初期化
	engine.initialize();

	// ワールド
	engine.getWorldManager()->LoadWorld<ecs::DevelopWorld>();


	// メッセージループ
	MSG msg = {};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// 入力
			UpdateInput();

			// エンジンの更新
			engine.tick();
			// FPS
			char fps[256];
			sprintf_s(fps, "FPS:%u", engine.getCurrentFPS());
			SetWindowText(pWin->getWindowHandle(), fps);
			
		}
	}


	// エンジンの終了処理
	engine.finalize();

	// 入力
	UninitInput();

	return (int)msg.wParam;
}



/// @brief プロシージャ
/// @param hWnd 
/// @param uMsg 
/// @param wParam 
/// @param lParam 
/// @return 
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		true;

	switch (uMsg) {
	case WM_CREATE:					//----- ウィンドウが生成された
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	case WM_DESTROY:				//----- ウィンドウ破棄指示がきた
		PostQuitMessage(0);				// システムにスレッドの終了を要求
		break;
	case WM_KEYDOWN:				//----- キーボードが押された
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]キーが押された
			PostMessage(hWnd, WM_CLOSE, 0, 0);	// [x]が押されたように振舞う
			return 0;
		}
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16;			// [Alt]+[Enter]時のBEEPを抑止
	case WM_DPICHANGED:
	{
		int dpi = HIWORD(wParam);
		float scale = (float)dpi / USER_DEFAULT_SCREEN_DPI;

		// ここにDPI変更に反応する処理を書く予定
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	}
	break;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// @brief WM_CREATEメッセージハンドラ
/// @param hWnd 
/// @param lpcs 
/// @return 
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	// クライアント領域サイズをSCREEN_WIDTH×SCREEN_HEIGHTに再設定.
	RECT rcClnt;
	int SCREEN_WIDTH = Engine::get().getWindowWidth();
	int SCREEN_HEIGHT = Engine::get().getWindowHeight();

	GetClientRect(hWnd, &rcClnt);
	rcClnt.right -= rcClnt.left;
	rcClnt.bottom -= rcClnt.top;
	if (rcClnt.right != SCREEN_WIDTH || rcClnt.bottom != SCREEN_HEIGHT) {
		RECT rcWnd;
		GetWindowRect(hWnd, &rcWnd);
		SIZE sizeWnd;
		sizeWnd.cx = (rcWnd.right - rcWnd.left) - rcClnt.right + SCREEN_WIDTH;
		sizeWnd.cy = (rcWnd.bottom - rcWnd.top) - rcClnt.bottom + SCREEN_HEIGHT;

		SetWindowPos(hWnd, nullptr, 0, 0, sizeWnd.cx, sizeWnd.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	// IME無効化
	ImmAssociateContext(hWnd, nullptr);

	return 0;	// -1を返すとCreateWindow[Ex]が失敗する.
}
