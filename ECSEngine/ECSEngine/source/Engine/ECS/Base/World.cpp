/*****************************************************************//**
 * \file   World.cpp
 * \brief  ワールド
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "World.h"
#include "EntityManager.h"
#include "GameObjectManager.h"
#include "SystemBase.h"
#include "RenderPipeline.h"

#include <Engine/ECS/System/SystemTable.h>
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace ecs;


/// @brief コンストラクタ
World::World(WorldManager* pWorldManager):
	m_pWorldManager(pWorldManager)
{
	m_pEntityManager = std::make_unique<EntityManager>(this);
	m_pGameObjectManager = std::make_unique<GameObjectManager>(this);
	m_pRenderPipeline = std::make_unique<RenderPipeline>(this);
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

/// @brief ゲームオブジェクトマネージャーの取得
/// @return ゲームオブジェクトマネージャー
GameObjectManager* World::getGameObjectManager() const
{
	return m_pGameObjectManager.get();
}

/// @brief レンダーパイプラインの取得
/// @return レンダーパイプライン
RenderPipeline* World::getRenderPipeline() const
{
	return m_pRenderPipeline.get();
}

/// @brief システムの更新
void World::update()
{
	// スタートアップ
	m_pGameObjectManager->StartUp();

	for (auto&& system : m_SystemList)
	{
		system->onUpdate();
	}

	// クリーンアップ
	m_pGameObjectManager->CleanUp();
}

void World::render()
{
	// 描画
	m_pRenderPipeline->onUpdate();
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

/// @brief ワールドのシリアライズ
/// @param path パス
void World::serializeWorld(std::string path)
{
	// チャンクの書き出し
	{
		BinaryStream bs;
		this->writeBinaryStream(bs);

		std::string chunkPath(path + "chunk.bin");
		std::ofstream ofs(chunkPath, std::ios::binary);
		ofs.write(bs.getBuffer(), bs.getSize());
		ofs.close();
	}

	// システムの書き出し
	{
		std::vector<std::size_t> systemData;
		for (auto& system : m_SystemList) {
			systemData.push_back(system->getHash());
		}

		std::string systemPath(path + "system.bin");
		std::ofstream ofs(systemPath, std::ios::binary);
		ofs.write(reinterpret_cast<const char*>(systemData.data()),
			systemData.size() * sizeof(std::size_t));
		ofs.close();
	}

	// ゲームオブジェクトの書き出し
	m_pGameObjectManager->Serialize(path);
}

/// @brief ワールドのシリアライズ
/// @param path パス
bool World::deserializeWorld(std::string path)
{
	// チャンクファイルの読み込み
	BinaryStream bs;
	{
		std::string chunkPath(path + "chunk.bin");
		std::ifstream ifs(chunkPath, std::ios::binary);
		if (!ifs.is_open()) return false;

		ifs.seekg(0, std::ios_base::end);
		const int fileSize = static_cast<const int>(ifs.tellg());
		ifs.seekg(0, std::ios_base::beg);
		const auto fileBuffer = std::make_unique<char[]>(fileSize);
		ifs.read(fileBuffer.get(), fileSize);
		bs.write(fileBuffer.get(), fileSize);
		ifs.close();
	}

	// システムデータの読み込み
	std::vector<std::size_t> systemData;
	{
		std::string systemPath(path + "system.bin");
		std::ifstream ifs(systemPath, std::ios::binary);
		if (!ifs.is_open()) return false;

		ifs.seekg(0, std::ios_base::end);
		const int fileSize = static_cast<const int>(ifs.tellg());
		ifs.seekg(0, std::ios_base::beg);
		const auto fileBuffer = std::make_unique<char[]>(fileSize);
		ifs.read(fileBuffer.get(), fileSize);

		systemData.resize(fileSize / sizeof(std::size_t));
		std::memcpy(systemData.data(), fileBuffer.get(), fileSize);
	}

	// ゲームオブジェクトの読み込み
	{
		auto is = m_pGameObjectManager->Deserialize(path);
		if (!is) return false;
	}

	// チャンクの生成
	this->readBinaryStream(bs);

	// システムの終了処理
	for (auto&& system : m_SystemList)
	{
		system->onDestroy();
	}
	m_SystemList.clear();

	// システムの生成
	for (auto& hash : systemData)
	{
		auto system = SystemTable::get().makeSystem(hash, this);
		if (!system) continue;

		system->onCreate();
		m_SystemList.push_back(std::move(system));
	}
	sortSystem();

	return true;
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