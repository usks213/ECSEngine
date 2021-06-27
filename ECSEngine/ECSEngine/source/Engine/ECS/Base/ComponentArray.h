/*****************************************************************//**
 * \file   ComponentArray.h
 * \brief  コンポーネント配列
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

namespace ecs {

	/// @class ComponentArray
	/// @brief 指定した型をチャンクからコンポーネント配列として扱う
	/// @tparam T 型
	template<class T>
	class ComponentArray
	{
	public:
		/// @brief コンストラクタ
		/// @param pBegin チャンク内の型の先頭アドレス
		/// @param size 配列のサイズ
		explicit ComponentArray(T* pBegin, const std::uint32_t size) :
			m_pBegin(pBegin), m_Size(size)
		{
		}

		/// @brief 添え字アクセス
		/// @param index インデックス(index < n_Size)
		/// @return T型データの参照
		T& operator[](const int index) { return m_pBegin[index]; }

		/// @brief 配列の先頭アドレス
		/// @return T型 先頭アドレス
		T* begin() { return m_pBegin; }

		/// @brief 配列の末尾アドレス
		/// @return T型 末尾アドレス
		T* end() { return m_pBegin + sizeof(T) * m_Size; }

		/// @brief 配列の要素数
		/// @return 要素数
		std::uint32_t Count() { return m_Size; }

	private:
		T* m_pBegin = nullptr;
		std::uint32_t m_Size = 0;
	};
}