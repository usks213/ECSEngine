/*****************************************************************//**
 * \file   FBXParser.h
 * \brief  FBXモデルロード
 * 
 * \author USAMI KOSHI
 * \date   2021/06/09
 *********************************************************************/
#pragma once

#include <vector>
#include <string>

/**
 * FBXファイルとは
 * Autodesk社が開発したファイルフォーマット
 * 3Dモデルのデータも含まれてはいるが、
 * 別々のソフトで扱えるようにするため
 * 
 * 〇コーンモデル
 * 頂点座標
 * 法線
 * UV座標
 * 頂点数 8
 * 
 * 一頂点あたりのデータ数
 * 頂点座標	- float * 3
 * 法線		- float * 3
 * UV座標	- float * 2
 *  → 32byte * 8頂点 = 256byte
 * 
 * FBXファイルのデータサイズ → 15,871byte
 * 60倍のデータサイズ！！
 * 
 * 必要なデータが格納されている位置を探し出し、
 * 必要なサイズだけを確保しながら読み込む
 * → 解析(Parser:パーサー)
 */

/// @brief 
class FBXParser
{
public:
	FBXParser();
	~FBXParser();

	bool Load(const char* filename);
	void Release();

	// 頂点
	float* getVertexList() { return m_vtxList.data(); }
	std::uint32_t getVertexNum() { return m_nVtxNum; }

	// インデックス
	int* getIndexList() { return m_idxList.data(); }
	std::uint32_t getIndexNum() { return m_nIdxNum; }

	// 法線
	float* getNormalList() { return m_nrmList.data(); }
	std::uint32_t getNormalNum() { return m_nNrmNum; }

	// UV
	float* getUVList() { return m_uvList.data(); }
	int getUVNum() { return m_nUVNum; }
	int* getUVIndexList() { return m_uvIdxList.data(); }

	// テクスチャ
	std::string* getTextureList() { return m_texList.data(); }
	std::uint32_t getTextureNum() { return m_texList.size(); }

private:

	/// @brief 特定の文字列を検索する
	/// @param ptr 探索開始位置
	/// @param key 探索文字列
	/// @return 見つかったアドレス
	[[nodiscard]] char* FindKeyword(char* ptr, const char* key);

private:
	/// @brief 頂点数
	std::uint32_t m_nVtxNum;
	/// @brief 頂点リスト
	std::vector<float> m_vtxList;
	/// @brief インデックス数
	std::uint32_t m_nIdxNum;
	/// @brief インデックスデータ
	std::vector<int> m_idxList;
	/// @brief 法線数
	std::uint32_t m_nNrmNum;
	/// @brief 法線データ
	std::vector<float> m_nrmList;
	/// @brief UV数
	std::uint32_t		m_nUVNum;
	/// @brief UVデータ
	std::vector<float>	m_uvList;
	/// @brief UVインデックスデータ
	std::vector<int>	m_uvIdxList;
	/// @brief
	//std::uint32_t		m_nTexNum;
	/// @brief テクスチャデータ
	std::vector<std::string> m_texList;
};

