/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  �Q�[���G���W���N���X
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/
#include "Engine.h"

// Windows�̋@�\...
//#include <mmsystem.h>
//#pragma comment(lib, "winmm")


/// @brief �R���X�g���N�^
Engine::Engine()
	: m_windowManager(), m_rendererManager(), m_worldManager(),
	m_nCurrentFPS(0), m_nFrameCount(0), m_deltaTime(0), m_fixedDeltaTime(0)
{
	m_nMaxFPS = 90;
	m_nFixedTime = 20; // �~���b
}

/// @brief ����������
/// @param pWindow 
/// @param pRenderer 
/// @return ������
bool Engine::initialize(WindowManagerBase* pWindow, RendererManagerBase* pRenderer)
{
	// �i�[
	m_windowManager.reset(pWindow);
	m_rendererManager.reset(pRenderer);

	// ���[���h�}�l�[�W���[�̏�����
	//m_worldManager = std::make_unique<WorldManager>();


	// �t���[���J�E���g������
	m_ExecLastTime = m_FPSLastTime = 
		m_CurrentTime = m_FixedExecLastTime =
		std::chrono::system_clock::now();
	m_nFrameCount = 0;

	return true;
}

/// @brief �X�V
void Engine::tick()
{
	// �����_���[�̃N���A
	m_rendererManager->clear();

	// �^�C�}�[�X�V
	m_CurrentTime = std::chrono::system_clock::now();

	// FPS����	0.5�b���ƂɎ��s
	std::int64_t fpsTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_FPSLastTime).count();
	if (fpsTime >= 500)
	{
		m_nCurrentFPS = static_cast<std::uint32_t>(m_nFrameCount * 1000 / (fpsTime));
		m_FPSLastTime = m_CurrentTime;
		m_nFrameCount = 0;
	}

	//--- �Œ�t���[�����[�g�X�V ---
	std::int64_t fixedTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_FixedExecLastTime).count();
	if (fixedTime >= m_nFixedTime)
	{
		m_FixedExecLastTime = m_CurrentTime;

		// FixedUpdate
		//m_worldManager.FixedUpdate();
	}

	//--- �σt���[�����[�g�X�V ---
	std::int64_t deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_ExecLastTime).count();
	if (deltaTime >= (1000 / m_nMaxFPS))
	{
		m_ExecLastTime = m_CurrentTime;

		// Update
		//m_worldManager.Update();

		// Draw
		//m_worldManager.Draw();
		m_rendererManager->present();

		// �t���[���J�E���^�X�V
		m_nFrameCount++;
	}
}

/// @brief �I������
void Engine::finalize()
{
}
