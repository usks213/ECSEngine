/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  �Q�[���G���W���N���X
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include <windows.h>
#include "Engine.h"

// Windows�̋@�\...
#include <mmsystem.h>
#pragma comment(lib, "winmm")


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
	timeBeginPeriod(1);		// ����\��ݒ�
	m_dwExecLastTime = m_dwFPSLastTime = timeGetTime();
	m_dwCurrentTime = m_dwFrameCount = 0;

	return true;
}

/// @brief �X�V
void Engine::tick()
{
	// �����_���[�̃N���A

	// �^�C�}�[�X�V
	m_dwCurrentTime = timeGetTime();
	// FPS����
	if ((m_dwCurrentTime - m_dwFPSLastTime) >= 500) {	// 0.5�b���ƂɎ��s
		m_nCurrentFPS = m_dwFrameCount * 1000 / (m_dwCurrentTime - m_dwFPSLastTime);
		m_dwFPSLastTime = m_dwCurrentTime;
		m_dwFrameCount = 0;
	}

	//--- �Œ�t���[�����[�g�X�V ---
	if ((m_dwCurrentTime - m_dwExecLastTime) >= (1000 / 60)) {
		m_dwExecLastTime = m_dwCurrentTime;


	}

	//--- �σt���[�����[�g�X�V ---


	// �t���[���J�E���^�X�V
	m_dwFrameCount++;
}

/// @brief �I������
void Engine::finalize()
{
	// �^�C�}�ݒ�����ɖ߂�
	timeEndPeriod(1);
}
