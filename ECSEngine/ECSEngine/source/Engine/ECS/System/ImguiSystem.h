/*****************************************************************//**
 * \file   ImguiSystem.h
 * \brief  imgui表示システム
 * 
 * \author USAMI KOSHI
 * \date   2021/07/05
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>

namespace ecs {

	class ImguiSystem final : public SystemBase
	{
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit ImguiSystem(World* pWorld):
			m_MainCamera(nullptr), 
			m_MaimCameraTransform(nullptr), 
			m_selectObjectID(NONE_GAME_OBJECT_ID),
			SystemBase(pWorld)
		{}
		/// デストラクタ
		~ImguiSystem() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

	private:
		/// @brief GUI表示・入力
		/// @param typeName 型名
		/// @param data データ
		void DispGui(std::string_view typeName, void* data);

		void DispChilds(const GameObjectID parentID);

		Camera*			m_MainCamera;
		Transform*		m_MaimCameraTransform;
		GameObjectID	m_selectObjectID;
	};
}