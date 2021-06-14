/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  ゲームエンジンクラス
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include <windows.h>
#include "Engine.h"

// Windowsの機能...
#include <mmsystem.h>
#pragma comment(lib, "winmm")


/// @brief コンストラクタ
Engine::Engine()
	: m_windowManager(), m_rendererManager(), m_worldManager()
{
}

/// @brief 初期化処理
/// @param pWindow 
/// @param pRenderer 
/// @return 成功か
bool Engine::initialize(WindowManagerBase* pWindow, RendererManagerBase* pRenderer)
{
	// 格納
	m_windowManager.reset(pWindow);
	m_rendererManager.reset(pRenderer);

	// ワールドマネージャーの初期化
	//m_worldManager = std::make_unique<WorldManager>();


	// フレームカウント初期化
	timeBeginPeriod(1);		// 分解能を設定
	m_dwExecLastTime = m_dwFPSLastTime = timeGetTime();
	m_dwCurrentTime = m_dwFrameCount = 0;

	return true;
}

/// @brief 更新
void Engine::tick()
{
	// レンダラーのクリア

	// タイマー更新
	m_dwCurrentTime = timeGetTime();
	// FPS測定
	if ((m_dwCurrentTime - m_dwFPSLastTime) >= 500) {	// 0.5秒ごとに実行
		m_nCurrentFPS = m_dwFrameCount * 1000 / (m_dwCurrentTime - m_dwFPSLastTime);
		m_dwFPSLastTime = m_dwCurrentTime;
		m_dwFrameCount = 0;
	}

	//--- 固定フレームレート更新 ---
	if ((m_dwCurrentTime - m_dwExecLastTime) >= (1000 / 60)) {
		m_dwExecLastTime = m_dwCurrentTime;


	}

	//--- 可変フレームレート更新 ---


	// フレームカウンタ更新
	m_dwFrameCount++;
}

/// @brief 終了処理
void Engine::finalize()
{
	// タイマ設定を元に戻す
	timeEndPeriod(1);
}
