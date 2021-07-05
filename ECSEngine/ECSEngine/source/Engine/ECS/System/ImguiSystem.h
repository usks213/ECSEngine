/*****************************************************************//**
 * \file   ImguiSystem.h
 * \brief  imgui表示システム
 * 
 * \author USAMI KOSHI
 * \date   2021/07/05
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>

namespace ecs {

	class ImguiSystem final : public SystemBase
	{
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit ImguiSystem(World* pWorld):
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
	};
}