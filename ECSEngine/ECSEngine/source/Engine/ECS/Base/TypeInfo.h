/*****************************************************************//**
 * \file   TypeInfo.h
 * \brief  �^���
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <iostream>

/// @brief �^���t��
#define DECLARE_TYPE_INFO(T) 						   \
public:												   \
	static constexpr std::string_view getTypeName()	   \
	{												   \
		return #T;									   \
	}												   \
	static constexpr std::size_t getTypeHash()		   \
	{												   \
		auto fnvOffsetBasis = 14695981039346656037ULL; \
		constexpr auto cFnvPrime = 1099511628211ULL;   \
													   \
		for (auto idx : #T)							   \
		{											   \
			fnvOffsetBasis ^= static_cast<size_t>(idx);\
			fnvOffsetBasis *= cFnvPrime;			   \
		}											   \
		return fnvOffsetBasis;						   \
	}												   \
	void _dumyFunction() = delete


/// @brief �^�𕶎���ɕϊ�
#define TypeToString(Type) #Type

/// @brief �^���̍ő啶����
constexpr int MAX_TYPE_NAME = 256;


namespace type
{
	/// @brief getTypeHash�������Ă��邩 (���^)
	/// @tparam T getTypeHash()��ێ����Ă���^
	template <typename T>
	struct HasGetTypeHash
	{
	private:
		/// @brief getTypeHash�������͔����
		/// @tparam U 
		/// @param  0�Œ�
		/// @return getTypeHash�����Ȃ�true �Ȃ����false_type���g��
		template<typename U>
		static auto Test(int)
			-> decltype(U::getTypeHash(), std::true_type());

		/// @brief getTypeHash�������Ă��Ȃ��ꍇ
		/// @tparam U 
		/// @param  (...)�͌^��肵�Ȃ��悤�ɁH
		/// @return false_type
		template<typename U>
		static auto Test(...)
			-> decltype(std::false_type());
	public:
		/// @brief �ێ�����
		using Check = decltype(Test<T>(0));
	};

	/// @brief getTypeHash�������Ă��邩����
	/// @tparam T getTypeHash()��ێ����Ă���^(true_type)
	template<typename T>
	constexpr bool has_get_type_hash = HasGetTypeHash<T>::Check::value;
}

/// @class TypeInfo
/// @brief �^���N���X
class TypeInfo
{
	/// @brief �v���C�x�[�g�R���X�g���N�^
	/// @param typeHash �^�̃n�b�V���l
	/// @param size		�^�̃������T�C�Y
	constexpr explicit TypeInfo(const std::size_t hash, const std::size_t size, std::string_view name)
		: m_typeHash(hash), m_typeSize(size), m_typeName()
	{
		for (int i = 0; name.data()[i] != '\0'; ++i) {
			m_typeName[i] = name.data()[i];
		}
	}

public:
	/// @brief �f�t�H���g�R���X�g���N�^
	constexpr TypeInfo() :m_typeHash(-1), m_typeSize(0), m_typeName() {}

	/// @brief �^��v��r
	/// @param other ���̌^���
	/// @return ��v�Ȃ�True
	constexpr bool operator==(const TypeInfo& other) const
	{
		return m_typeHash == other.m_typeHash;
	}

	/// @brief �^�s��v��r
	/// @param other ���̌^���
	/// @return �s��v�Ȃ�True
	constexpr bool operator!=(const TypeInfo& other) const
	{
		return !(*this == other);
	}

	/// @brief �^�ŗL�̃n�b�V���l
	[[nodiscard]] constexpr std::size_t getHash() const
	{
		return m_typeHash;
	}

	/// @brief  �^�̃������T�C�Y
	[[nodiscard]] constexpr std::size_t getSize() const
	{
		return m_typeSize;
	}

	/// @brief  �^�̖��O
	[[nodiscard]] constexpr std::string_view getName() const
	{
		return m_typeName;
	}

	/// @brief �e���v���[�g�ŐV�����^��񐶐�
	/// @tparam T getTypeHash()��ێ����Ă���^
	/// @return �V�����^���
	template<class T, typename = std::enable_if_t<type::has_get_type_hash<T>>>
	static constexpr TypeInfo create()
	{
		return TypeInfo(T::getTypeHash(), sizeof(T), T::getTypeName());
	}

	/// @brief �����̃n�b�V���l�ƃT�C�Y����^���𐶐�
	/// @param hash �^�̃n�b�V���l
	/// @param size �^�̃������T�C�Y
	/// @return �^���
	static constexpr TypeInfo create(const std::size_t hash, const std::size_t size, std::string_view name)
	{
		return TypeInfo(hash, size, name);
	}

private:
	/// @brief �^�ŗL�̃n�b�V���l
	std::size_t m_typeHash;
	/// @brief  �^�̃������T�C�Y
	std::size_t m_typeSize;
	/// @brief �^��
	char m_typeName[MAX_TYPE_NAME];
};