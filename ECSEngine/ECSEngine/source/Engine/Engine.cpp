/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  ゲームエンジンクラス
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#include "Engine.h"

// Windowsの機能...
//#include <mmsystem.h>
//#pragma comment(lib, "winmm")


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
	m_ExecLastTime = m_FPSLastTime = m_CurrentTime =
		std::chrono::system_clock::now();
	m_dwFrameCount = 0;

	return true;
}

/// @brief 更新
void Engine::tick()
{
	// レンダラーのクリア


	// タイマー更新
	m_CurrentTime = std::chrono::system_clock::now();

	// FPS測定	0.5秒ごとに実行
	long long fpsTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_FPSLastTime).count();
	if (fpsTime >= 500)
	{
		m_nCurrentFPS = m_dwFrameCount * 1000 / (fpsTime);
		m_FPSLastTime = m_CurrentTime;
		m_dwFrameCount = 0;
	}

	//--- 固定フレームレート更新 ---
	long long frameTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_ExecLastTime).count();
	if (frameTime >= (1000 / 60)) 
	{
		m_ExecLastTime = m_CurrentTime;


	}

	//--- 可変フレームレート更新 ---


	// フレームカウンタ更新
	m_dwFrameCount++;
}

/// @brief 終了処理
void Engine::finalize()
{
}
