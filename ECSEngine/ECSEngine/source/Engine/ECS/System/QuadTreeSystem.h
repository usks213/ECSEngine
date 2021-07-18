/*****************************************************************//**
 * \file   QuadTreeSystem.h
 * \brief  空間分割システム(四分木)
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <array>


namespace ecs {

	class QuadTreeSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(QuadTreeSystem);
	public:
		/// @brief コンストラクタ
		/// @param pWorld ワールド
		explicit QuadTreeSystem(World* pWorld);

		/// デストラクタ
		~QuadTreeSystem() = default;

		/// @brief 生成時
		void onCreate() override;
		/// @brief 削除時
		void onDestroy() override;
		/// @brief 更新
		void onUpdate() override;

		/// @brief 空間分割数の取得
		std::uint32_t getCellNum() { return m_uiMaxCell; }

	private:

		// ビット分割関数
		std::uint32_t BitSeparate32(std::uint32_t n)
		{
			n = (n | (n << 8)) & 0x00ff00ff;
			n = (n | (n << 4)) & 0x0f0f0f0f;
			n = (n | (n << 2)) & 0x33333333;
			return (n | (n << 1)) & 0x55555555;
		}

		// 2Dモートン空間番号算出関数
		std::uint16_t Get2DMortonNumber(std::uint16_t x, std::uint16_t y)
		{
			return (std::uint16_t)(BitSeparate32(x) | (BitSeparate32(y) << 1));
		}

		// 座標→線形4分木要素番号変換関数
		std::uint32_t GetPointElem(float pos_x, float pos_y)
		{
			// 階層オフセット
			pos_x += m_fWidth / 2;
			pos_y += m_fHeight / 2;

			// 領域外なら弾く
			if (pos_x - m_fLeft < 0 || pos_y - m_fTop < 0)
				return 0xffffffff;

			// 本当はフィールドの大きさとか
			return Get2DMortonNumber((std::uint16_t)((pos_x - m_fLeft) / m_fUnit_W),
				(std::uint16_t)((pos_y - m_fTop) / m_fUnit_H));
		}

		void SetLevel(std::uint32_t level)
		{
			if (level >= MAX_LEVEL) return;

			m_uiLevel = level;
			m_uiMaxCell = (m_iPow[level + 1] - 1) / 3;
		}

		void SetQuadSize(float width, float height)
		{
			m_fWidth = width;
			m_fHeight = height;
			// 最小レベル空間サイズ更新
			m_fUnit_W = width / (float)(1 << (m_uiLevel - 1));
			m_fUnit_H = height / (float)(1 << (m_uiLevel - 1));
		}

		void SetLeftTop(float left, float height)
		{
			m_fLeft = left;
			m_fTop = height;
		}

	private:
		float m_fUnit_W;				// 最小レベル空間の幅単位
		float m_fUnit_H;				// 最小レベル空間の高単位
		float m_fLeft;					// 領域の左側（X軸最小値）
		float m_fTop;					// 領域の上側（Z軸最小値）
		float m_fWidth;					// 領域の幅
		float m_fHeight;				// 領域の高さ
		std::uint32_t m_uiLevel;		// 最下位レベル
		std::uint32_t m_uiMaxCell;		// 空間分割数

		static constexpr std::uint32_t MAX_LEVEL = 9;
		std::array<std::uint32_t, MAX_LEVEL + 1> m_iPow;

	public:
		std::vector<std::vector<Entity>> m_dynamicMainList;
		std::vector<std::vector<Entity>> m_dynamicSubList;
		std::vector<std::vector<Entity>> m_staticMainList;
		std::vector<std::vector<Entity>> m_staticSubList;
	};
}