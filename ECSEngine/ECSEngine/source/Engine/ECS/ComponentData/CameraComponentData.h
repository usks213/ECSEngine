/*****************************************************************//**
 * \file   CameraComponentData.h
 * \brief  カメラコンポーネンデータ
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
		float aspect;				// 縦横比
		float fovY;					// 視野角(Degree)
		float nearZ;				// 前方クリップ距離
		float farZ;					// 後方クリップ距離
	};
}