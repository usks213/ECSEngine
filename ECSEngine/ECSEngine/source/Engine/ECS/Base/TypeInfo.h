/*****************************************************************//**
 * \file   TypeInfo.h
 * \brief  型情報
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <iostream>

/// @brief 型情報付加
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


/// @brief 型を文字列に変換
#define TypeToString(Type) #Type

/// @brief 型名の最大文字数
constexpr int MAX_TYPE_NAME = 256;


namespace type
{
	/// @brief getTypeHashを持っているか (メタ)
	/// @tparam T getTypeHash()を保持している型
	template <typename T>
	struct HasGetTypeHash
	{
	private:
		/// @brief getTypeHash持ちかは判定後
		/// @tparam U 
		/// @param  0固定
		/// @return getTypeHash持ちならtrue なければfalse_typeを使う
		template<typename U>
		static auto Test(int)
			-> decltype(U::getTypeHash(), std::true_type());

		/// @brief getTypeHashを持っていない場合
		/// @tparam U 
		/// @param  (...)は型被りしないように？
		/// @return false_type
		template<typename U>
		static auto Test(...)
			-> decltype(std::false_type());
	public:
		/// @brief 保持判定
		using Check = decltype(Test<T>(0));
	};

	/// @brief getTypeHashを持っているか判定
	/// @tparam T getTypeHash()を保持している型(true_type)
	template<typename T>
	constexpr bool has_get_type_hash = HasGetTypeHash<T>::Check::value;
}

/// @class TypeInfo
/// @brief 型情報クラス
class TypeInfo
{
	/// @brief プライベートコンストラクタ
	/// @param typeHash 型のハッシュ値
	/// @param size		型のメモリサイズ
	constexpr explicit TypeInfo(const std::size_t hash, const std::size_t size, std::string_view name)
		: m_typeHash(hash), m_typeSize(size), m_typeName()
	{
		for (int i = 0; name.data()[i] != '\0'; ++i) {
			m_typeName[i] = name.data()[i];
		}
	}

public:
	/// @brief デフォルトコンストラクタ
	constexpr TypeInfo() :m_typeHash(-1), m_typeSize(0), m_typeName() {}

	/// @brief 型一致比較
	/// @param other 他の型情報
	/// @return 一致ならTrue
	constexpr bool operator==(const TypeInfo& other) const
	{
		return m_typeHash == other.m_typeHash;
	}

	/// @brief 型不一致比較
	/// @param other 他の型情報
	/// @return 不一致ならTrue
	constexpr bool operator!=(const TypeInfo& other) const
	{
		return !(*this == other);
	}

	/// @brief 型固有のハッシュ値
	[[nodiscard]] constexpr std::size_t getHash() const
	{
		return m_typeHash;
	}

	/// @brief  型のメモリサイズ
	[[nodiscard]] constexpr std::size_t getSize() const
	{
		return m_typeSize;
	}

	/// @brief  型の名前
	[[nodiscard]] constexpr std::string_view getName() const
	{
		return m_typeName;
	}

	/// @brief テンプレートで新しい型情報生成
	/// @tparam T getTypeHash()を保持している型
	/// @return 新しい型情報
	template<class T, typename = std::enable_if_t<type::has_get_type_hash<T>>>
	static constexpr TypeInfo create()
	{
		return TypeInfo(T::getTypeHash(), sizeof(T), T::getTypeName());
	}

	/// @brief 既存のハッシュ値とサイズから型情報を生成
	/// @param hash 型のハッシュ値
	/// @param size 型のメモリサイズ
	/// @return 型情報
	static constexpr TypeInfo create(const std::size_t hash, const std::size_t size, std::string_view name)
	{
		return TypeInfo(hash, size, name);
	}

private:
	/// @brief 型固有のハッシュ値
	std::size_t m_typeHash;
	/// @brief  型のメモリサイズ
	std::size_t m_typeSize;
	/// @brief 型名
	char m_typeName[MAX_TYPE_NAME];
};