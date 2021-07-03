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


namespace ecs {

	struct Camera : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Camera);

		bool isOrthographic;

		Matrix view;
		Matrix projection;

		float width;
		float height;
		float aspect;				// �c����
		float fovY;					// ����p(Degree)
		float nearZ;				// �O���N���b�v����
		float farZ;					// ����N���b�v����
	};
}