/*****************************************************************//**
 * \file   Asset.h
 * \brief  �A�Z�b�g
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
	/// @brief �R���X�g���N�^
	/// @param id �C���X�^���XID
	/// @param name ���O
	explicit Asset(const InstanceID& id, std::string_view name) :
		Object(id, name)
	{
	}

	/// @brief �f�X�g���N�^
	~Asset() = default;

private:

};

