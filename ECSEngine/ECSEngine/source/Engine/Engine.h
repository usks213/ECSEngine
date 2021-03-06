/*****************************************************************//**
 * \file   Engine.h
 * \brief  ゲームエンジンクラス
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#pragma once

#include <memory>
#include <chrono>

#include "Utility/Singleton.h"
#include "OS/Base/WindowManager.h"
#include "Renderer/Base/RendererManager.h"
#include "ECS/Base/WorldManager.h"
#include "Editor/EditorManager.h"


/// @class Engine
/// @brief エンジン
class Engine final : public Singleton<Engine>
{
public:
	[[nodiscard]] bool initialize();

	void tick();

	void finalize();

public:

	/// @brief ウィンドウマネージャーの生成
	/// @tparam T ウィンドウタイプ
	/// @tparam WindowManager継承型のみ
	/// @return ウィンドウのポインタ
	template<class T, typename = std::enable_if_t<std::is_base_of_v<WindowManager, T>>>
	T* createWindow(std::string windowName, int windowWidth, int windowHeight) {
		m_windowManager = std::make_unique<T>(windowName, windowWidth, windowHeight);
		m_windowManager->m_pEngine = this;
		return static_cast<T*>(m_windowManager.get());
	}

	/// @brief レンダラーマネージャーの生成
	/// @tparam T レンダラータイプ
	/// @tparam RendererManager継承型のみ
	/// @return レンダラーのポインタ
	template<class T, typename = std::enable_if_t<std::is_base_of_v<RendererManager, T>>>
	T* createRenderer() {
		m_rendererManager = std::make_unique<T>();
		m_rendererManager->m_pEngine = this;
		return static_cast<T*>(m_rendererManager.get());
	}

public:
	[[nodiscard]] WindowManager* getWindowManager()		{ return m_windowManager.get(); }
	[[nodiscard]] RendererManager* getRendererManager() { return m_rendererManager.get(); }
	[[nodiscard]] WorldManager* getWorldManager()		{ return m_worldManager.get(); }
	[[nodiscard]] EditorManager* getEditorManager()		{ return m_editorManager.get(); }

	[[nodiscard]] int getWindowWidth()	{ return m_windowManager.get()->getWindowWidth(); }
	[[nodiscard]] int getWindowHeight() { return m_windowManager.get()->getWindowHeight(); }
	[[nodiscard]] std::uint32_t getCurrentFPS() { return m_nCurrentFPS; }

	std::uint32_t getFrameCount() { return m_nFrameCount; }
	float getCurrentTime()
	{
		std::int64_t fpsTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			m_CurrentTime - m_InitTime).count();
		return fpsTime / 1000.0f;
	}

private:
	friend Singleton<Engine>;

	/// @brief コンストラクタ
	Engine();

	/// @brief デストラクタ
	~Engine() = default;

	std::unique_ptr<WindowManager>		m_windowManager;
	std::unique_ptr<RendererManager>	m_rendererManager;
	std::unique_ptr<WorldManager>		m_worldManager;
	std::unique_ptr<EditorManager>		m_editorManager;

	//--- タイマー 
	std::uint32_t m_nCurrentFPS;
	std::chrono::system_clock::time_point m_FixedExecLastTime;
	std::chrono::system_clock::time_point m_ExecLastTime;
	std::chrono::system_clock::time_point m_FPSLastTime;
	std::chrono::system_clock::time_point m_CurrentTime;
	std::chrono::system_clock::time_point m_InitTime;
	std::uint64_t m_nFrameCount;
	// デルタタイム
	float m_deltaTime;
	float m_fixedDeltaTime;
	// フレームレート
	std::int32_t m_nMaxFPS;
	std::int64_t m_nFixedTime;
};

