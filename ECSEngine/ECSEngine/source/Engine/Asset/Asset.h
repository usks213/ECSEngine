/*****************************************************************//**
 * \file   Asset.h
 * \brief  アセット
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include <Engine/Object.h>


using AssetID = InstanceID;


class Asset : public Object
{
public:
	/// @brief コンストラクタ
	/// @param id インスタンスID
	/// @param name 名前
	explicit Asset(const InstanceID& id, std::string_view name) :
		Object(id, name)
	{
	}

	/// @brief デストラクタ
	~Asset() = default;

private:

};

