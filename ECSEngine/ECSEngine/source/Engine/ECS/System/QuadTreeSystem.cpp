/*****************************************************************//**
 * \file   QuadTreeSystem.h
 * \brief  空間分割システム(四分木)
 *
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/

#include "QuadTreeSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>

#include <imgui.h>


using namespace ecs;


/// @brief コンストラクタ
/// @param pWorld ワールド
QuadTreeSystem::QuadTreeSystem(World* pWorld) :
	SystemBase(pWorld),
	m_uiLevel(0),
	m_uiMaxCell(0),
	m_fUnit_W(0.0f),
	m_fUnit_H(0.0f),
	m_fLeft(0.0f),
	m_fTop(0.0f),
	m_fWidth(0.0f),
	m_fHeight(0.0f)
{
	// 階層ごとの空間数
	m_iPow[0] = 1;
	for (std::uint32_t i = 1; i < MAX_LEVEL + 1; i++)
		m_iPow[i] = m_iPow[i - 1] * 4;

}

 /// @brief 生成時
void QuadTreeSystem::onCreate()
{
	// 空間レベル
	SetLevel(3); // 3=85, 4=341,
	// 空間サイズ
	SetQuadSize(32, 32);
	// 空間の端
	SetLeftTop(-m_fWidth / 2, -m_fHeight / 2);

	// メモリ確保
	m_dynamicMainList.resize(m_uiMaxCell);
	m_dynamicSubList.resize(m_uiMaxCell);
	m_staticMainList.resize(m_uiMaxCell);
	m_staticSubList.resize(m_uiMaxCell);
	for (unsigned int i = 0; i < m_uiMaxCell; ++i)
	{
		m_dynamicMainList[i].reserve(10);
		m_dynamicSubList[i].reserve(10);
		m_staticMainList[i].reserve(10);
		m_staticSubList[i].reserve(10);
	}
}

/// @brief 削除時
void QuadTreeSystem::onDestroy()
{

}

/// @brief 更新
void QuadTreeSystem::onUpdate()
{
	// 前回のデータを削除(メモリ解放はしない)
	for (unsigned int i = 0; i < m_uiMaxCell; ++i)
	{
		m_dynamicMainList[i].clear();
		m_dynamicSubList[i].clear();
		m_staticMainList[i].clear();
		m_staticSubList[i].clear();
	}

	// モートン番号
	std::uint32_t Def = 0;
	std::uint32_t wLeftTop = 0;
	std::uint32_t wRightDown = 0;

	// カメラ座標
	Transform* cameraTrans = nullptr;
	foreach<Camera, Transform>(
		[&cameraTrans](Camera& camera, Transform& transform)
		{
			cameraTrans = &transform;
		});

	// カメラ座標からの相対座標
	if (cameraTrans)
	{
		//Vector3 pos = Vector3::Transform(cameraTrans->translation, cameraTrans->globalMatrix);
		Vector3 pos = cameraTrans->translation;
		// 空間端
		SetLeftTop(pos.x - m_fWidth / 2, pos.y - m_fHeight / 2);
	}
	else
	{
		// 空間端
		SetLeftTop(-m_fWidth / 2, -m_fHeight / 2);
	}

	// アーキタイプ指定
	auto staticObject = Archetype::create<Transform, StaticType>();
	auto dynamicObject = Archetype::create<Transform, DynamicType>();

	// チャンクループ
	int chunkIndex = 0;
	for (auto&& chunk : m_pWorld->getChunkList())
	{
		// 静的オブジェクト
		if (staticObject.isIn(chunk.getArchetype()))
		{
			auto transforms = chunk.getComponentArray<Transform>();
			for (std::uint32_t index = 0; index < chunk.getSize(); ++index)
			{
				// AABB
				Vector3 min(-0.5f, -0.5f, -0.5f);
				Vector3 max(0.5f, 0.5f, 0.5f);
				min = Vector3::Transform(min, transforms[index].globalMatrix);
				max = Vector3::Transform(max, transforms[index].globalMatrix);

				// ここで空間の登録をする
				// 左上と右下を出す
				wLeftTop = GetPointElem(min.x, min.y);
				wRightDown = GetPointElem(max.x, max.y);
				// 空間外
				if (wLeftTop >= m_uiMaxCell - 1 || wRightDown >= m_uiMaxCell - 1)
				{
					continue;
				}

				// XORをとる	
				Def = wLeftTop ^ wRightDown;
				unsigned int HiLevel = 0;
				unsigned int i;
				for (i = 0; i < m_uiLevel; ++i)
				{
					std::uint32_t Check = (Def >> (i * 2)) & 0x3;
					if (Check != 0)
						HiLevel = i + 1;
				}
				std::uint32_t SpaceNum = wRightDown >> (HiLevel * 2);
				std::uint32_t AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 3;
				SpaceNum += AddNum;	// これが今いる空間

				// 空間外ははじく
				if (SpaceNum > m_uiMaxCell - 1)
				{
					continue;
				}

				// 今いる空間のメインリストに格納
				m_staticMainList[SpaceNum].push_back(Entity(chunkIndex, index));

				// 今いる空間の親のサブに格納
				while (SpaceNum > 0)
				{
					SpaceNum--;
					SpaceNum /= 4;
					m_staticSubList[SpaceNum].push_back(Entity(chunkIndex, index));
				}
			}
		}
		// 動的オブジェクト
		if (dynamicObject.isIn(chunk.getArchetype()))
		{
			auto transforms = chunk.getComponentArray<Transform>();
			for (std::uint32_t index = 0; index < chunk.getSize(); ++index)
			{
				// AABB
				Vector3 min(-0.5f, -0.5f, -0.5f);
				Vector3 max(0.5f, 0.5f, 0.5f);
				min = Vector3::Transform(min, transforms[index].globalMatrix);
				max = Vector3::Transform(max, transforms[index].globalMatrix);

				// ここで空間の登録をする
				// 左上と右下を出す
				wLeftTop = GetPointElem(min.x, min.y);
				wRightDown = GetPointElem(max.x, max.y);
				// 空間外
				if (wLeftTop >= m_uiMaxCell - 1 || wRightDown >= m_uiMaxCell - 1)
				{
					continue;
				}

				// XORをとる	
				Def = wLeftTop ^ wRightDown;
				unsigned int HiLevel = 0;
				unsigned int i;
				for (i = 0; i < m_uiLevel; ++i)
				{
					std::uint32_t Check = (Def >> (i * 2)) & 0x3;
					if (Check != 0)
						HiLevel = i + 1;
				}
				std::uint32_t SpaceNum = wRightDown >> (HiLevel * 2);
				std::uint32_t AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 3;
				SpaceNum += AddNum;	// これが今いる空間

				// 空間外ははじく
				if (SpaceNum > m_uiMaxCell - 1)
				{
					continue;
				}

				// 今いる空間のメインリストに格納
				m_dynamicMainList[SpaceNum].push_back(Entity(chunkIndex, index));

				// 今いる空間の親のサブに格納
				while (SpaceNum > 0)
				{
					SpaceNum--;
					SpaceNum /= 4;
					m_dynamicSubList[SpaceNum].push_back(Entity(chunkIndex, index));
				}
			}
		}
		chunkIndex++;
	}

	//ImGui::Begin("QuadTree");
	//int i = 0;
	//for (const auto& list : m_staticSubList)
	//{
	//	if (ImGui::TreeNodeEx(std::to_string(i).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		for (const auto& entity : list)
	//		{
	//			ImGui::Text(std::to_string(entity.m_index).c_str());
	//		}
	//		ImGui::TreePop();
	//	}
	//	i++;
	//}

	//ImGui::End();
}
