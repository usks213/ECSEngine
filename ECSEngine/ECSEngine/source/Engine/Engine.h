/*****************************************************************//**
 * \file   Engine.h
 * \brief  �Q�[���G���W���N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#pragma once

#include <memory>
#include <chrono>

#include "Utility/Singleton.h"
#include "OS/Base/WindowManagerBase.h"
#include "Renderer/Base/RendererManagerBase.h"
#include "ECS/Base/WorldManager.h"


/// @class Engine
/// @brief �G���W��
class Engine final : public Singleton<Engine>
{
public:
	[[nodiscard]] bool initialize(WindowManagerBase* pWindow, RendererManagerBase* pRenderer);

	void tick();

	void finalize();

public:

	[[nodiscard]] int getWindowWidth()	{ return m_windowManager.get()->getWindowWidth(); }
	[[nodiscard]] int getWindowHeight() { return m_windowManager.get()->getWindowHeight(); }

private:
	friend Singleton<Engine>;

	/// @brief �R���X�g���N�^
	Engine();

	/// @brief �f�X�g���N�^
	~Engine() = default;

	std::unique_ptr<WindowManagerBase>		m_windowManager;
	std::unique_ptr<RendererManagerBase>	m_rendererManager;
	std::unique_ptr<WorldManager>			m_worldManager;

	//--- �^�C�}�[ 
	std::uint32_t m_nCurrentFPS;
	std::chrono::system_clock::time_point m_FixedExecLastTime;
	std::chrono::system_clock::time_point m_ExecLastTime;
	std::chrono::system_clock::time_point m_FPSLastTime;
	std::chrono::system_clock::time_point m_CurrentTime;
	std::uint64_t m_nFrameCount;
	// �f���^�^�C��
	float m_deltaTime;
	float m_fixedDeltaTime;
	// �t���[�����[�g
	std::int32_t m_nMaxFPS;
	std::int64_t m_nFixedTime;
};

