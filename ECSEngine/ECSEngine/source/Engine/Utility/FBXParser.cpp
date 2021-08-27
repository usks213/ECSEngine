/*****************************************************************//**
 * \file   FBXParser.h
 * \brief  FBXモデルロード
 *
 * \author USAMI KOSHI
 * \date   2021/06/09
 *********************************************************************/

#include "FBXParser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory>

/// @brief コンストラクタ
FBXParser::FBXParser()
	: m_nIdxNum(0), m_nVtxNum(0),m_nNrmNum(0),m_nUVNum(0)
{
}

/// @brief デストラクタ
FBXParser::~FBXParser()
{
	Release();
}

void FBXParser::Release()
{
	m_nVtxNum = 0;
	m_nIdxNum = 0;
	m_nNrmNum = 0;
	m_nUVNum  = 0;
	std::vector<float>	tempV;
	std::vector<int>	tempI;
	std::vector<float>	tempN;
	std::vector<float>	tempU;
	std::vector<int>	tempUI;
	std::vector<std::string> tempT;
	m_vtxList.swap(tempV);
	m_idxList.swap(tempI);
	m_nrmList.swap(tempN);
	m_uvList.swap(tempU);
	m_uvIdxList.swap(tempUI);
	m_texList.swap(tempT);
}

/// @brief FBXロード関数
/// @param filename パス
/// @return 成功 true
bool FBXParser::Load(const char* filename)
{
	// メモリ解放
	Release();

	/* バイナリ形式とテキスト形式のデータの違い
	例えば円周率(3.141592)を保存する場合
	バイナリでは float(4byte)
	テキストでは "3.141592"(char[8] 8byte)で保存される
	*/
	printf("%s:ファイルの読み込み\n", filename);
	FILE* fp;
	fopen_s(&fp, filename, "rt");
	if (fp == NULL)
	{
		printf("%s:ファイルオープン失敗\n", filename);
		return false;
	}

	// データを一括で読み込む(特定文字まで進めるため)
	// (オンロード/onload) or (ストリーミング/streaming)

	// ファイルのデータサイズを調べる
	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	// メモリ確保
	char* pData = new char[fileSize];
	fread(pData, fileSize, 1, fp);
	fclose(fp);

	char* ptr = pData;
	// 文字列「Geometry」まで読み進める
	ptr = FindKeyword(ptr, "Geometry:");
	//--- 頂点読み込み
	ptr = FindKeyword(ptr, "Vertices: *");
	// 文字列から数字に変換
	m_nVtxNum = atoi(ptr);
	if (m_nVtxNum > 0)
	{
		// 頂点データ読み取り
		char* ptrVtx = ptr;
		m_vtxList.reserve(m_nVtxNum);
		ptrVtx = FindKeyword(ptrVtx, "a: ");
		for (auto i = 0; i < m_nVtxNum; ++i)
		{
			m_vtxList.push_back(atof(ptrVtx));
			ptrVtx = FindKeyword(ptrVtx, ",");
		}
		m_nVtxNum /= 3;
	}
	//--- インデックス読み込み
	ptr = FindKeyword(ptr, "PolygonVertexIndex: *");
	int idxNum = atoi(ptr);
	std::vector<int> tempIdx;
	if (idxNum > 0)
	{
		char* ptrIdx = ptr;
		// インデックスの読み込み
		
		tempIdx.reserve(idxNum);
		ptrIdx = FindKeyword(ptrIdx, "a: ");
		for (int i = 0; i < idxNum; ++i)
		{
			tempIdx.push_back(atoi(ptrIdx));
			ptrIdx = FindKeyword(ptrIdx, ",");
		}
		// FBXファイルは多角形の面データが含まれる
		// 全ての面を三角形に分割
		// ①分割後のインデックス数を計算
		int splitIdxNum = 0;
		// 面を区切るインデックスはマイナス
		int minusIdxPoint = 0;
		for(int i = 0; i < idxNum; ++i)
		{
			++minusIdxPoint;
			if (tempIdx[i] < 0)
			{
				// 多角形の頂点数 - 2が
				// 分割後の三角形の数
				splitIdxNum += minusIdxPoint - 2;
				minusIdxPoint = 0;
			}
		}
		printf("分割後の面数:%d\n", splitIdxNum);
		// ②分割した面の数からインデックスデータを確保
		m_nIdxNum = splitIdxNum * 3;
		m_idxList.resize(m_nIdxNum);
		// ③元のインデックスデータを分割しながら格納
		int storageIdx = 0;
		minusIdxPoint = 0;
		for (int i = 0; i < idxNum; ++i)
		{
			++minusIdxPoint;
			if (minusIdxPoint <= 3)
			{
				// 三角形はそのまま格納
				m_idxList[storageIdx] = tempIdx[i];
				storageIdx++;
			}
			else
			{
				// 四角形以上は格納済みのインデックス
				// をコピーしながら格納
				m_idxList[storageIdx] = m_idxList[storageIdx - 3];
				storageIdx++;
				m_idxList[storageIdx] = m_idxList[storageIdx - 2];
				storageIdx++;
				m_idxList[storageIdx] = tempIdx[i];
				storageIdx++;
			}

			if (tempIdx[i] < 0)
			{
				// 面の区切り
				minusIdxPoint = 0;
				// マイナスのインデックスをビット反転
				m_idxList[storageIdx - 1] = ~m_idxList[storageIdx - 1];
			}
		}

	}

	//--- 法線の読み込み
	ptr = FindKeyword(ptr, "LayerElementNormal:");
	if (ptr != NULL)
	{
		char* nrmPtr = ptr;
		// MappingInformationTypeが"ByPolygonVertex"のみ対応
		// ByPolygonVertex...面に対応した法線
		// ByControlPoint...頂点に対応した法線
		nrmPtr = FindKeyword(nrmPtr, "MappingInformationType: \"");
		if (strstr(nrmPtr, "ByPolygonVertex") == nrmPtr)
		{
			// ①データ数を読み込む
			nrmPtr = FindKeyword(nrmPtr, "Normals: *");
			int nrmNum = atoi(nrmPtr);

			// ②法線のデータを読み込む
			std::vector<float> tempNrm;
			tempNrm.reserve(nrmNum);
			nrmPtr = FindKeyword(nrmPtr, "a: ");
			for (int i = 0; i < nrmNum; ++i)
			{
				tempNrm.push_back(atof(nrmPtr));
				nrmPtr = FindKeyword(nrmPtr, ",");
			}

			// ③インデックスデータを元に分割
			// 分割後の法線数	xyz分
			m_nNrmNum = m_nIdxNum * 3;
			m_nrmList.resize(m_nNrmNum);

			// ④分割後のデータをメンバ変数に格納
			int storageIdx = 0;
			int minusIdxPoint = 0;
			for (int i = 0; i < idxNum; ++i)
			{
				++minusIdxPoint;
				if (minusIdxPoint <= 3)
				{
					// 三角形はそのまま格納
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 0];
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 1];
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 2];
				}
				else
				{
					// 四角形以上は格納済みの法線
					// をコピーしながら格納
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 9];
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 9];
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 9];

					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 6];
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 6];
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 6];

					m_nrmList[storageIdx++] = tempNrm[i * 3 + 0];
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 1];
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 2];
				}

				if (tempIdx[i] < 0)
				{
					// 面の区切り
					minusIdxPoint = 0;
					// マイナスのインデックスをビット反転
				}
			}
		}
	}

	// UVデータ読み込み
	ptr = FindKeyword(ptr, "LayerElementUV");
	if (ptr != nullptr)
	{
		char* pUV = ptr;
		// UVのデータ設定方法を確認
		pUV = FindKeyword(pUV, "MappingInformationType: \"");
		if (strstr(pUV, "ByPolygonVertex") == pUV)
		{
			pUV = FindKeyword(pUV, "ReferenceInformationType: \"");
			if (strstr(pUV, "IndexToDirect") == pUV)
			{
				// UVデータ読み込み
				char* pUVPtrRecord = pUV;
				pUV = FindKeyword(pUV, "UV: *");
				m_nUVNum = atoi(pUV);
				m_uvList.resize(m_nUVNum);
				pUV = FindKeyword(pUV, "a: ");
				for (int i = 0; i < m_nUVNum; ++i)
				{
					m_uvList[i] = atof(pUV);
					pUV = FindKeyword(pUV, ",");
				}
				// UV参照データ読み込み
				pUV = pUVPtrRecord;
				pUV = FindKeyword(pUV, "UVIndex: *");
				int nUVIdx = atoi(pUV);
				std::vector<int> tempUVIdx(nUVIdx);
				pUV = FindKeyword(pUV, "a: ");
				for (int i = 0; i < nUVIdx; ++i)
				{
					tempUVIdx[i] = atoi(pUV);
					pUV = FindKeyword(pUV, ",");
				}

				// UV参照データを分割
				// インデックスデータを元に分割
				m_uvIdxList.resize(m_nIdxNum);

				// 分割後のデータをメンバ変数に格納
				int storageIdx = 0;
				int minusIdxPoint = 0;
				for (int i = 0; i < idxNum; ++i)
				{
					++minusIdxPoint;
					if (minusIdxPoint <= 3)
					{
						// 三角形はそのまま格納
						m_uvIdxList[storageIdx++] = tempUVIdx[i];
					}
					else
					{
						// 四角形以上は格納済みの法線
						// をコピーしながら格納
						m_uvIdxList[storageIdx++] = m_uvIdxList[storageIdx - 3];

						m_uvIdxList[storageIdx++] = m_uvIdxList[storageIdx - 2];

						m_uvIdxList[storageIdx++] = tempUVIdx[i];
					}

					if (tempIdx[i] < 0)
					{
						// 面の区切り
						minusIdxPoint = 0;
					}
				}
			}
		}
	}

	// テクスチャの読み込み
	if (ptr != nullptr)
	{
		ptr = FindKeyword(ptr, "Texture:");
		if (ptr != nullptr)
		{
			char* pTex = ptr;
			pTex = FindKeyword(pTex, "FileName: \"");
			// 何文字あるか計算
			char* pEnd = FindKeyword(pTex, "\"") - 1;
			int textLen = pEnd - pTex;
			// メモリ確保
			std::unique_ptr<char[]> pData = std::make_unique<char[]>(textLen + 1);
			memcpy(pData.get(), pTex, textLen);
			pData[textLen] = '\0';
			// 格納
			m_texList.push_back(pData.get());

		}
	}


	delete[] pData;
	return true;
}

char* FBXParser::FindKeyword(char* ptr, const char* key)
{
	// strstr 文字列中から指定された文字に一致する文字のアドレスを
	// 返す。なければNULL
	ptr = strstr(ptr, key);
	if (NULL != ptr)
	{
		ptr += strlen(key);
	}

	return ptr;
}
