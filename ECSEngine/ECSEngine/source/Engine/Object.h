/*****************************************************************//**
 * \file   Object.h
 * \brief  オブジェクト
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include "Utility/TypeUtil.h"


 /// @brief オブジェクトの型IDを付加
#define DECLARE_TYPE_ID(T)							\
DECLARE_TYPE_INFO( T );								\
[[nodiscard]] std::size_t getTypeID() override {	\
	return getTypeHash();							\
}													\
void _dumyFunction2() = delete


/// @brief インスタンスID
using InstanceID = std::uint32_t;


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
	[[nodiscard]] virtual std::size_t getTypeID() = 0;

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
