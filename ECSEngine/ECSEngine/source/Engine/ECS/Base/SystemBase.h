/*****************************************************************//**
 * \file   SystemBase.h
 * \brief  システムベース
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "EntityManager.h"
#include "GameObjectManager.h"


namespace ecs {

	/// @class SystemBase
	/// @brief システムベース
	class SystemBase
	{
	public:
		/// @brief コピーコンストラクタ削除
		SystemBase(SystemBase&) = delete;
		/// @brief ムーブコンストラクタ削除
		SystemBase(SystemBase&&) = delete;

		/// @brief コンストラクタ
		explicit SystemBase(World* pWorld);
		/// @brief デストラクタ
		virtual ~SystemBase() = default;

		/// @brief 生成時
		virtual void onCreate();
		/// @brief 削除時
		virtual void onDestroy();
		/// @brief 更新
		virtual void onUpdate() = 0;

		/// @brief 更新順番号取得
		[[nodiscard]] int getExecutionOrder() const;

	protected:

		/// @brief エンティティマネージャー取得
		/// @return エンティティマネージャー
		[[nodiscard]] EntityManager* getEntityManager() const;

		/// @brief ゲームオブジェクトマネージャー取得
		/// @return ゲームオブジェクトマネージャー
		[[nodiscard]] GameObjectManager* getGameObjectManager() const;

		/// @brief 更新順設定
		/// @param 更新番号
		void setExecutionOrder(const int executionOrder);

		/// @brief 指定したコンポーネントデータをフェッチして処理する
		/// @tparam T1 フェッチするコンポーネントデータ型
		/// @tparam Func 関数オブジェクト
		/// @param func 処理の関数オブジェクト
		template<class T1, typename Func>
		void foreach(Func&& func);

		/// @brief 指定したコンポーネントデータをフェッチして処理する
		/// @tparam T1 フェッチするコンポーネントデータ型
		/// @tparam T2 フェッチするコンポーネントデータ型
		/// @tparam Func 関数オブジェクト
		/// @param func 処理の関数オブジェクト
		template<class T1, class T2, typename Func>
		void foreach(Func&& func);

		/// @brief 指定したコンポーネントデータをフェッチして処理する
		/// @tparam T1 フェッチするコンポーネントデータ型
		/// @tparam T2 フェッチするコンポーネントデータ型
		/// @tparam T3 フェッチするコンポーネントデータ型
		/// @tparam Func 関数オブジェクト
		/// @param func 処理の関数オブジェクト
		template<class T1, class T2, class T3, typename Func>
		void foreach(Func&& func);

		/// @brief 指定したコンポーネントデータをフェッチして処理する
		/// @tparam T1 フェッチするコンポーネントデータ型
		/// @tparam T2 フェッチするコンポーネントデータ型
		/// @tparam T3 フェッチするコンポーネントデータ型
		/// @tparam T4 フェッチするコンポーネントデータ型
		/// @tparam Func 関数オブジェクト
		/// @param func 処理の関数オブジェクト
		template<class T1, class T2, class T3, class T4, typename Func>
		void foreach(Func&& func);

	protected:

		/// @brief 指定したコンポーネントデータをフェッチして処理する
		/// @tparam Func 関数オブジェクト
		/// @tparam ...Args フェッチするコンポーネントデータ型
		/// @param pChunk チャンク
		/// @param func 処理の関数オブジェクト
		/// @param ...args フェッチするコンポーネントデータ
		template<typename Func, class... Args>
		static void foreachImplements(Chunk* pChunk, Func&& func, Args... args);

		/// @brief 所属するワールド
		World* m_pWorld = nullptr;
		/// @brief
		int m_nExecutionOrder = 0;
	};

	/// @brief 指定したコンポーネントデータをフェッチして処理する
	/// @tparam T1 フェッチするコンポーネントデータ型
	/// @tparam Func 関数オブジェクト
	/// @param func 処理の関数オブジェクト
	template<class T1, typename Func>
	void SystemBase::foreach(Func&& func)
	{
		auto pChunkList = getEntityManager()->getChunkList<T1>();
		for (auto&& pChunk : pChunkList)
		{
			auto arg1 = pChunk->template getComponentArray<T1>();
			foreachImplements(pChunk, func, arg1);
		}
	}

	/// @brief 指定したコンポーネントデータをフェッチして処理する
	/// @tparam T1 フェッチするコンポーネントデータ型
	/// @tparam T2 フェッチするコンポーネントデータ型
	/// @tparam Func 関数オブジェクト
	/// @param func 処理の関数オブジェクト
	template<class T1, class T2, typename Func>
	void SystemBase::foreach(Func&& func)
	{
		auto pChunkList = getEntityManager()->getChunkList<T1, T2>();
		for (auto&& pChunk : pChunkList)
		{
			auto arg1 = pChunk->template getComponentArray<T1>();
			auto arg2 = pChunk->template getComponentArray<T2>();
			foreachImplements(pChunk, func, arg1, arg2);
		}
	}

	/// @brief 指定したコンポーネントデータをフェッチして処理する
	/// @tparam T1 フェッチするコンポーネントデータ型
	/// @tparam T2 フェッチするコンポーネントデータ型
	/// @tparam T3 フェッチするコンポーネントデータ型
	/// @tparam Func 関数オブジェクト
	/// @param func 処理の関数オブジェクト
	template<class T1, class T2, class T3, typename Func>
	void SystemBase::foreach(Func&& func)
	{
		auto pChunkList = getEntityManager()->getChunkList<T1, T2, T3>();
		for (auto&& pChunk : pChunkList)
		{
			auto arg1 = pChunk->template getComponentArray<T1>();
			auto arg2 = pChunk->template getComponentArray<T2>();
			auto arg3 = pChunk->template getComponentArray<T3>();
			foreachImplements(pChunk, func, arg1, arg2, arg3);
		}
	}

	/// @brief 指定したコンポーネントデータをフェッチして処理する
	/// @tparam T1 フェッチするコンポーネントデータ型
	/// @tparam T2 フェッチするコンポーネントデータ型
	/// @tparam T3 フェッチするコンポーネントデータ型
	/// @tparam T4 フェッチするコンポーネントデータ型
	/// @tparam Func 関数オブジェクト
	/// @param func 処理の関数オブジェクト
	template<class T1, class T2, class T3, class T4, typename Func>
	void SystemBase::foreach(Func&& func)
	{
		auto pChunkList = getEntityManager()->getChunkList<T1, T2, T3, T4>();
		for (auto&& pChunk : pChunkList)
		{
			auto arg1 = pChunk->template getComponentArray<T1>();
			auto arg2 = pChunk->template getComponentArray<T2>();
			auto arg3 = pChunk->template getComponentArray<T3>();
			auto arg4 = pChunk->template getComponentArray<T4>();
			foreachImplements(pChunk, func, arg1, arg2, arg3, arg4);
		}
	}

	/// @brief 指定したコンポーネントデータをフェッチして処理する
	/// @tparam Func 関数オブジェクト
	/// @tparam ...Args フェッチするコンポーネントデータ型
	/// @param pChunk チャンク
	/// @param func 処理の関数オブジェクト
	/// @param ...args フェッチするコンポーネントデータ
	template<typename Func, class... Args>
	void SystemBase::foreachImplements(Chunk* pChunk, Func&& func, Args... args)
	{
		for (std::uint32_t i = 0; i < pChunk->getSize(); ++i)
		{
			func(args[i]...);
		}
	}
}