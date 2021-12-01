/*****************************************************************//**
 * \file   AnimationComponentData.h
 * \brief  アニメーションコンポーネントデータ
 * 
 * \author USAMI KOSHI
 * \date   2021/09/30
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Renderer/Base/Animation.h>


namespace ecs {

	/// @brief リジッドボディ
	struct Animator : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Animator);

		// アニメーションID
		AnimationID animationID;
		// アバターID

		double time;

		/// @brief コンストラクタ
		Animator()
		{
		}
	};
}