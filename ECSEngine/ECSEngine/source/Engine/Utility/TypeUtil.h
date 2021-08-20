/*****************************************************************//**
 * \file   TypeUtil.h
 * \brief  Œ^
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include <iostream>
#include <string>


 /// @brief Œ^î•ñ•t‰Á
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
		std::string_view typeName = #T;				   \
		for (auto idx : typeName)					   \
		{											   \
			fnvOffsetBasis ^= static_cast<size_t>(idx);\
			fnvOffsetBasis *= cFnvPrime;			   \
		}											   \
		return fnvOffsetBasis;						   \
	}												   \
	void _dumyFunction() = delete



constexpr std::size_t TypeNameToTypeHash(std::string_view typeName)
{
	auto fnvOffsetBasis = 14695981039346656037ULL;
	constexpr auto cFnvPrime = 1099511628211ULL;

	for (auto idx : typeName)
	{
		fnvOffsetBasis ^= static_cast<size_t>(idx);
		fnvOffsetBasis *= cFnvPrime;
	}
	return fnvOffsetBasis;
}


/// @brief Œ^‚ğ•¶š—ñ‚É•ÏŠ·
#define TypeToString(Type) #Type

/// @brief Œ^‚ğƒnƒbƒVƒ…’l‚É•ÏŠ·
#define TypeToHash(Type) TypeNameToTypeHash(#Type)

/// @brief Œ^‚ğID‚É•ÏŠ·
#define TypeToID(Type) TypeNameToTypeHash(#Type)
