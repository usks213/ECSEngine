/*****************************************************************//**
 * \file   WindowManagerBase.cpp
 * \brief  �A�v���P�[�V�����E�B���h�E�̃x�[�X�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "WindowManagerBase.h"

WindowManagerBase::WindowManagerBase(std::string windowName, int windowWidth, int windowHeight)
	: m_windowName(windowName), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
}

WindowManagerBase::~WindowManagerBase()
{
}