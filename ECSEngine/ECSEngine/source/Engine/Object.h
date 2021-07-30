/*****************************************************************//**
 * \file   Object.h
 * \brief  �I�u�W�F�N�g
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include "Utility/TypeUtil.h"


 /// @brief �I�u�W�F�N�g�̌^ID��t��
#define DECLARE_TYPE_ID(T)							\
DECLARE_TYPE_INFO( T );								\
[[nodiscard]] std::size_t getTypeID() override {	\
	return getTypeHash();							\
}													\
void _dumyFunction2() = delete


/// @brief �C���X�^���XID
using InstanceID = std::uint32_t;


/// @brief �I�u�W�F�N�g
class Object
{
public:
	/// @brief �R���X�g���N�^
	/// @param id �C���X�^���XID
	/// @param name ���O
	explicit Object(const InstanceID& id, std::string_view name) :
		m_instanceID(id), m_name(name)
	{
	}

	/// @brief �f�X�g���N�^
	~Object() = default;

	/// @brief �^�C�vID�擾
	/// @return ID
	[[nodiscard]] virtual std::size_t getTypeID() = 0;

	/// @brief �C���X�^���XID�擾
	/// @return ID
	[[nodiscard]] InstanceID getInstanceID() { return m_instanceID; }

	/// @brief ���O�̎擾
	/// @return ���O
	[[nodiscard]] std::string_view getName() { return m_name; }

protected:
	/// @brief �C���X�^���XID
	InstanceID		m_instanceID;
	/// @brief ���O
	std::string		m_name;
};
