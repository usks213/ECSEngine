/*****************************************************************//**
 * \file   ColliderComponentData.h
 * \brief　コライダー
 * 
 * \author USAMI KOSHI
 * \date   2021/07/22
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/IComponentData.h>
#include <Engine/Utility/Mathf.h>


namespace ecs {

	/// @brief 物理コンポーネントデータ
	struct Collider : IComponentData
	{
		ECS_DECLARE_COMPONENT_DATA(Collider);

		/// @brief コライダーの種類
		enum class ColliderType : std::uint32_t
		{
			UNKNOWN = 0, // 未定義
			BOX,         // キューブ
			SPHERE,      // スフィア
			CYLINDER,    // シリンダー
			CAPSULE,     // カプセル
			CONE,        // コーン
			PLANE,       // 平面
			POLYGON,     // ポリゴン
			COMPOUND     // 混合シェイプ
		};
		
		/// @brief コライダータイプ
		ColliderType colliderType;
		/// @brief 押し出し使用
		bool trigger;

		/// @brief 中心オフセット
		Vector3 center;
		/// @brief サイズ
		Vector3 size;

		/// @brief コンストラクタ
		Collider(ColliderType type, Vector3 center = Vector3(), Vector3 size = Vector3(1,1,1)):
		colliderType(type), trigger(false), center(center), size(size)
		{}

	};
}