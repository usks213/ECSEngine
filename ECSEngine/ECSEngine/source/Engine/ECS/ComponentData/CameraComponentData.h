/*****************************************************************//**
 * \file   CameraComponentData.h
 * \brief  �J�����R���|�[�l���f�[�^
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>
#include <Engine/Renderer/Base/RenderTarget.h>
#include <Engine/Renderer/Base/DepthStencil.h>

namespace ecs {

	struct Camera : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Camera);

		bool isOrthographic;			// ���s���e
		float fovY;						// ����p(Degree)
		float nearZ;					// �O���N���b�v����
		float farZ;						// ����N���b�v����
		float viewportScale;			// �r���[�|�[�g�̕��E�����̃X�P�[��
		Vector2 viewportOffset;			// �r���[�|�[�g�̍��ォ��̃I�t�Z�b�g
		RenderTargetID renderTargetID;	// �����_�[�^�[�Q�b�gID
		DepthStencilID depthStencilID;	// �[�x�X�e���V��ID

		float width;
		float height;
		float aspect;				
		Matrix world;
		Matrix view;
		Matrix projection;
	};
}