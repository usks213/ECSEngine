/*****************************************************************//**
 * \file   Object.h
 * \brief  �I�u�W�F�N�g
 * \detail �C���X�y�N�^�[�ɕ\���\
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include "Utility/TypeUtil.h"
#include "cereal.hpp"
#include "archives/json.hpp"
#include "archives/binary.hpp"

#include "types/string.hpp"
#include "types/vector.hpp"
#include "types/list.hpp"
#include "types/unordered_map.hpp"
#include "types/base_class.hpp"


 /// @brief �C���X�^���XID
using InstanceID = std::uint32_t;
/// @brief �^�C�vID
using TypeID = std::size_t;

 /// @brief �I�u�W�F�N�g�̌^ID��t��
#define DECLARE_TYPE_ID(T)							\
DECLARE_TYPE_INFO( T );								\
[[nodiscard]] TypeID getTypeID() override {	\
	return getTypeHash();							\
}													\
void _dumyFunction2() = delete



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
	[[nodiscard]] virtual TypeID getTypeID() = 0;

	/// @brief �C���X�^���XID�擾
	/// @return ID
	[[nodiscard]] InstanceID getInstanceID() { return m_instanceID; }

	/// @brief ���O�̎擾
	/// @return ���O
	[[nodiscard]] std::string_view getName() { return m_name; }

	/// @brief �V���A���C�Y��
	template<class T>
	void serialize(T& archive)
	{
		archive(CEREAL_NVP(m_instanceID), CEREAL_NVP(m_name));
	}

protected:
	/// @brief �C���X�^���XID
	InstanceID		m_instanceID;
	/// @brief ���O
	std::string		m_name;
};
