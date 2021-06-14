/*****************************************************************//**
 * \file   Engine.h
 * \brief  �Q�[���G���W���N���X
 * 
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#pragma once

#include <memory>
#include"Utility/Singleton.h"
#include "WindowManagerBase.h"
#include"RendererManagerBase.h"
#include"WorldManager.h"


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
	std::uint64_t m_dwExecLastTime;
	std::uint64_t m_dwFPSLastTime;
	std::uint64_t m_dwCurrentTime;
	std::uint64_t m_dwFrameCount;
};

