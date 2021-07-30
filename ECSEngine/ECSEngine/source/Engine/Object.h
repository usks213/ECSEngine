/*****************************************************************//**
 * \file   Object.h
 * \brief  オブジェクト
 * \detail インスペクターに表示可能
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include "Utility/TypeUtil.h"


 /// @brief インスタンスID
using InstanceID = std::uint32_t;
/// @brief タイプID
using TypeID = std::size_t;

 /// @brief オブジェクトの型IDを付加
#define DECLARE_TYPE_ID(T)							\
DECLARE_TYPE_INFO( T );								\
[[nodiscard]] TypeID getTypeID() override {	\
	return getTypeHash();							\
}													\
void _dumyFunction2() = delete



/// @brief オブジェクト
class Object
{
public:
	/// @brief コンストラクタ
	/// @param id インスタンスID
	/// @param name 名前
	explicit Object(const InstanceID& id, std::string_view name) :
		m_instanceID(id), m_name(name)
	{
	}

	/// @brief デストラクタ
	~Object() = default;

	/// @brief タイプID取得
	/// @return ID
	[[nodiscard]] virtual TypeID getTypeID() = 0;

	/// @brief インスタンスID取得
	/// @return ID
	[[nodiscard]] InstanceID getInstanceID() { return m_instanceID; }

	/// @brief 名前の取得
	/// @return 名前
	[[nodiscard]] std::string_view getName() { return m_name; }

protected:
	/// @brief インスタンスID
	InstanceID		m_instanceID;
	/// @brief 名前
	std::string		m_name;
};
