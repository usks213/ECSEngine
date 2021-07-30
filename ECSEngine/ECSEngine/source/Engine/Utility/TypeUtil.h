/*****************************************************************//**
 * \file   TypeUtil.h
 * \brief  å^
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include <iostream>
#include <string>


 /// @brief å^èÓïÒïtâ¡
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


/// @brief å^Çï∂éöóÒÇ…ïœä∑
#define TypeToString(Type) #Type


/// @brief å^ÇIDÇ…ïœä∑
#define TypeToID(Type) TypeNameToTypeHash(#Type)
