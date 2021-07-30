/*****************************************************************//**
 * \file   AssetManager.h
 * \brief  アセットマネージャー
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include "Asset.h"
#include <Engine/Engine.h>


class AssetManager final
{
	friend class Engine;
public:
	AssetManager() = default;
	~AssetManager() = default;
	AssetManager(const AssetManager&) = delete;
	AssetManager(AssetManager&&) = delete;

public:

	template<class T, typename = std::enable_if_t<std::is_base_of_v<Asset, T>>>
	AssetID createAsset()
	{
		
	}

private:
	Engine* m_pEngine;

	std::unordered_map<TypeID, std::unordered_map<AssetID, std::unique_ptr<Asset>>> m_aseetPool;
};
