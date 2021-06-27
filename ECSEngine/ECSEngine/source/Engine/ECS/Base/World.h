/*****************************************************************//**
 * \file   World.h
 * \brief  ワールド
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include"Chunk.h"


namespace ecs {
	class EntityManager;
	class SystemBase;

	/// @class World
	/// @brief ワールド
	class World
	{
		friend class EntityManager;
	public:
		/// @brief コンストラクタ
		World();
		/// @brief デストラクタ
		~World();

		/// @brief エンティティマネージャーの取得
		/// @return エンティティマネージャー
		[[nodiscard]] EntityManager* getEntityManager() const;

		/// @brief システムの更新
		void update();

		/// @brief  システムの追加
		/// @tparam システムの型
		template<class T>
		void addSystem()
		{
			callDoCreate(m_SystemList.emplace_back(new T(this)).get());
			sortSystem();
		}

		/// @brief 他のワールドのデータを結合する
		/// SystemListは結合されない
		/// @param other 結合するワールド
		void mage(World&& other);

		/// @brief チャンクリストをバイナリストリームに書き出す
		/// @param output 書き出し先のストリーム
		void writeBinaryStream(BinaryStream& output);

		/// @brief チャンクリストをバイナリストリームから読み込む
		/// @param output 読み込み先のストリーム
		void readBinaryStream(BinaryStream& input);

	private:

		/// @brief システムの更新順整理
		void sortSystem();

		/// @brief システム生成時のコールバック
		static void callDoCreate(SystemBase* pSystem);

		/// @brief チャンクリスト
		std::vector<Chunk> m_ChunkList;
		/// @brief システムリスト
		std::vector<std::unique_ptr<SystemBase>> m_SystemList;
		/// @brief エンティティマネージャー
		std::unique_ptr<EntityManager> m_pEntityManager;
	};
}