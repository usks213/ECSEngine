/*****************************************************************//**
 * \file   SystemTable.h
 * \brief  システムテーブル
 * 
 * \author USAMI KOSHI
 * \date   2021/08/03
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/Utility/Singleton.h>
#include <functional>
#include <unordered_map>

namespace ecs {

	class SystemTable final : public Singleton<SystemTable>
	{
		friend class Singleton<SystemTable>;
	public:
		/// @brief IDからシステムの生成
		/// @param hash システム種別ハッシュ値
		/// @param pWorld ワールド
		/// @return システムベース
		std::unique_ptr<SystemBase> makeSystem(std::size_t hash, World* pWorld) {
			auto itr = m_makeSystemTable.find(hash);
			if (m_makeSystemTable.end() == itr) return nullptr;
			return std::move(itr->second(pWorld));
		}

		void PushSystem(std::size_t typeHash, std::function<std::unique_ptr<SystemBase>(World* pWorld)> func)
		{
			m_makeSystemTable.emplace(typeHash, func);
		}

	private:
		/// @brief コンストラクタ
		SystemTable();

		/// @brief デストラクタ
		~SystemTable() = default;

		/// @brief システム種別生成関数
		std::unordered_map<std::size_t, std::function<std::unique_ptr<SystemBase>(World* pWorld)>> m_makeSystemTable;
	};
}