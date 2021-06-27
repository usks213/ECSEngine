/*****************************************************************//**
 * \file   World.cpp
 * \brief  ワールド
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "World.h"
#include "EntityManager.h"
#include "SystemBase.h"
#include <algorithm>

using namespace ecs;


/// @brief コンストラクタ
World::World(WorldManager* pWorldManager):
	m_pWorldManager(pWorldManager)
{
	m_pEntityManager.reset(new EntityManager(this));
}

/// @brief デストラクタ
World::~World()
{
	for (auto&& system : m_SystemList)
	{
		system->onDestroy();
	}
}

/// @brief エンティティマネージャーの取得
/// @return エンティティマネージャー
EntityManager* World::getEntityManager() const
{
	return m_pEntityManager.get();
}

/// @brief システムの更新
void World::update()
{
	for (auto&& system : m_SystemList)
	{
		system->onUpdate();
	}
}

/// @brief 他のワールドのデータを結合する
/// SystemListは結合されない
/// @param other 結合するワールド
void World::mage(World&& other)
{
	for (auto& otherChunk : m_ChunkList)
	{
		auto findItr = std::find(m_ChunkList.begin(), m_ChunkList.end(), otherChunk);
		// 見つからなかった
		if (m_ChunkList.end() == findItr)
		{
			m_ChunkList.push_back(std::move(otherChunk));
		}
		// 既にある
		else
		{
			findItr->marge(std::move(otherChunk));
		}
	}

	other.m_ChunkList.clear();		// 解放されてないからメモリリークするかも？？
	other.m_SystemList.clear();
}

/// @brief チャンクリストをバイナリストリームに書き出す
/// @param output 書き出し先のストリーム
void World::writeBinaryStream(BinaryStream& output)
{
	const auto chunkSize = m_ChunkList.size();
	// チャンク数の書き出し
	output.write(chunkSize);
	// データの書き出し
	for (auto&& chunk : m_ChunkList)
	{
		chunk.writeBinaryStream(output);
	}
}

/// @brief チャンクリストをバイナリストリームから読み込む
/// @param output 読み込み先のストリーム
void World::readBinaryStream(BinaryStream& input)
{
	// チャンク数の取得
	std::size_t chunkSize;
	input.read(&chunkSize);
	// チャンク数でメモリを確保
	m_ChunkList.resize(chunkSize);
	// チャンクデータの読み込み
	for (size_t i = 0; i < chunkSize; ++i)
	{
		m_ChunkList[i] = Chunk::readBinaryStream(input);
	}
}

/// @brief システムの更新順整理
void World::sortSystem()
{
	std::sort(m_SystemList.begin(), m_SystemList.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return lhs->getExecutionOrder() < rhs->getExecutionOrder();
		});
}

/// @brief システム生成時のコールバック
void World::callDoCreate(SystemBase* pSystem)
{
	pSystem->onCreate();
}