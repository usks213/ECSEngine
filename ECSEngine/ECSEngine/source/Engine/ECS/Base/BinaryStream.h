/*****************************************************************//**
 * \file   BinaryStream.h
 * \brief  バイナリストリーム
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <vector>

/// @class BinaryStream
/// @brief バイナリストリーム
class BinaryStream
{
public:

	/// @brief コンストラクタ
	/// @param isBigSize 大きいバッファサイズが必要か
	explicit BinaryStream(bool isBigSize = true)
	{
		// 大きいサイズが必要な場合が多いのであらかじめ1MB確保
		if (isBigSize)
			m_Buffer.reserve(1024 * 1024);
	}

	/// @brief 指定された値をバイナリバッファに書き出す(型指定)
	/// is_trivially_copyable_v → std::memcpy()可能な型
	/// @tparam T 型
	/// @tparam enable_if_t std::memcpy()可能な型
	/// @param value 値
	template<class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	void write(const T& value)
	{
		write(&value, sizeof(T));
	}

	/// @brief 指定された値をバイナリバッファに書き出す(汎用ポイント)
	/// @param pBuff 書き出し元の汎用ポイント
	/// @param size 書き出すサイズ
	void write(const void* pBuff, std::size_t size)
	{
		const auto pByte = static_cast<const std::byte*>(pBuff);
		m_Buffer.insert(m_Buffer.end(), pByte, pByte + size);
	}

	/// @brief 型サイズ分バイトバッファから読み取る
	/// @tparam T 型
	/// @tparam enable_if_t std::memcpy()可能な型
	/// @param pValue コピー先のバッファ
	template<class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	void read(T* pValue)
	{
		read(pValue, sizeof(T));
	}

	/// @brief 指定サイズ分バイトバッファから読み取る
	/// @param pBuff コピー先のバッファ
	/// @param size 読み取るサイズ
	void read(void* pBuff, const std::size_t size)
	{
		std::memcpy(pBuff, m_Buffer.data() + m_Count, size);
		m_Count += size;
	}

	/// @brief (const char*)バイトバッファの取得
	/// ofstream::write用
	/// @return 先頭アドレス
	[[nodiscard]] const char* getBuffer() const
	{
		return reinterpret_cast<const char*>(m_Buffer.data());
	}

	/// @brief バイトバッファの大きさ
	/// @return バイト数
	[[nodiscard]] std::size_t getSize() const
	{
		return m_Buffer.size();
	}

private:
	/// @brief バイトバッファ
	std::vector<std::byte> m_Buffer;
	/// @brief 現在の読み取り位置指示子
	std::size_t m_Count = 0;

};