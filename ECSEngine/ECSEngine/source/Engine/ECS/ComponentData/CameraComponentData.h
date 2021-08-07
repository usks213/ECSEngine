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
#include <Engine/Renderer/Base/RenderTarget.h>
#include <Engine/Renderer/Base/DepthStencil.h>

namespace ecs {

	struct Camera : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Camera);

		bool isOrthographic;			// 平行投影
		float fovY;						// 視野角(Degree)
		float nearZ;					// 前方クリップ距離
		float farZ;						// 後方クリップ距離
		float viewportScale;			// ビューポートの幅・高さのスケール
		Vector2 viewportOffset;			// ビューポートの左上からのオフセット
		RenderTargetID renderTargetID;	// レンダーターゲットID
		DepthStencilID depthStencilID;	// 深度ステンシルID

		float width;
		float height;
		float aspect;				
		Matrix world;
		Matrix view;
		Matrix projection;
	};
}