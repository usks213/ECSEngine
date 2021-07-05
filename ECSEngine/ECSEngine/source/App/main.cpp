/*****************************************************************//**
 * \file   main.cpp
 * \brief  ECSEngine
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/


//===== �C���N���[�h =====
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

//-------- ���C�u�����̃����N
#pragma comment(lib, "imm32")


//===== �萔��` =====


//===== �v���g�^�C�v�錾 =====
// �E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// �E�B���h�E������
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
// imgui
IMGUI_IMPL_API LRESULT   ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/// @brief �G���g���|�C���g
/// @param hInstance		�C���X�^���X�n���h��
/// @param hPrevInstance	�O��̃C���X�^���X�n���h��
/// @param lpCmdLine		�R�}���h���C��
/// @param nCmdShow			�E�B���h�E���
/// @return �I���R�[�h
int WINAPI WinMain(	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
					_In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// ���g�p�錾
	UNREFERENCED_PARAMETER(lpCmdLine);		// ���g�p�錾

	// ����
	InitInput();

	// �G���W���擾
	auto& engine = Engine::get();
	// �E�B���h�E�̐���
	WindowsWindow* pWin = engine.createWindow<WindowsWindow>("ECSEngine", 1920, 1080);
	// �����_���[�̐���
	D3D11RendererManager* pRenderer = engine.createRenderer<D3D11RendererManager>();

	// �E�B���h�E�̏�����
	pWin->initialize(hInstance, "ECSAppClass", nCmdShow, WndProc);
	// �����_���[�̏�����
	pRenderer->initialize(pWin->getWindowHandle(), pWin->getWindowWidth(), pWin->getWindowHeight());
	// �G���W���̏�����
	engine.initialize();

	// ���[���h
	engine.getWorldManager()->LoadWorld<ecs::DevelopWorld>();


	// ���b�Z�[�W���[�v
	MSG msg = {};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// ����
			UpdateInput();

			// �G���W���̍X�V
			engine.tick();
			// FPS
			char fps[256];
			sprintf_s(fps, "FPS:%u", engine.getCurrentFPS());
			SetWindowText(pWin->getWindowHandle(), fps);
			
		}
	}


	// �G���W���̏I������
	engine.finalize();

	// ����
	UninitInput();

	return (int)msg.wParam;
}



/// @brief �v���V�[�W��
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
	case WM_CREATE:					//----- �E�B���h�E���������ꂽ
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	case WM_DESTROY:				//----- �E�B���h�E�j���w��������
		PostQuitMessage(0);				// �V�X�e���ɃX���b�h�̏I����v��
		break;
	case WM_KEYDOWN:				//----- �L�[�{�[�h�������ꂽ
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
			PostMessage(hWnd, WM_CLOSE, 0, 0);	// [x]�������ꂽ�悤�ɐU����
			return 0;
		}
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16;			// [Alt]+[Enter]����BEEP��}�~
	case WM_DPICHANGED:
	{
		int dpi = HIWORD(wParam);
		float scale = (float)dpi / USER_DEFAULT_SCREEN_DPI;

		// ������DPI�ύX�ɔ������鏈���������\��
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	}
	break;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// @brief WM_CREATE���b�Z�[�W�n���h��
/// @param hWnd 
/// @param lpcs 
/// @return 
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	// �N���C�A���g�̈�T�C�Y��SCREEN_WIDTH�~SCREEN_HEIGHT�ɍĐݒ�.
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

	// IME������
	ImmAssociateContext(hWnd, nullptr);

	return 0;	// -1��Ԃ���CreateWindow[Ex]�����s����.
}
