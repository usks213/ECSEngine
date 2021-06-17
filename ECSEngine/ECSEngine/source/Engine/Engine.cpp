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
	: m_windowManager(), m_rendererManager(), m_worldManager()
{
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
	m_ExecLastTime = m_FPSLastTime = m_CurrentTime =
		std::chrono::system_clock::now();
	m_dwFrameCount = 0;

	return true;
}

/// @brief �X�V
void Engine::tick()
{
	// �����_���[�̃N���A


	// �^�C�}�[�X�V
	m_CurrentTime = std::chrono::system_clock::now();

	// FPS����	0.5�b���ƂɎ��s
	long long fpsTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_FPSLastTime).count();
	if (fpsTime >= 500)
	{
		m_nCurrentFPS = m_dwFrameCount * 1000 / (fpsTime);
		m_FPSLastTime = m_CurrentTime;
		m_dwFrameCount = 0;
	}

	//--- �Œ�t���[�����[�g�X�V ---
	long long frameTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(m_CurrentTime - m_ExecLastTime).count();
	if (frameTime >= (1000 / 60)) 
	{
		m_ExecLastTime = m_CurrentTime;


	}

	//--- �σt���[�����[�g�X�V ---


	// �t���[���J�E���^�X�V
	m_dwFrameCount++;
}

/// @brief �I������
void Engine::finalize()
{
}
