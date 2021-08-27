/*****************************************************************//**
 * \file   Model.h
 * \brief  モデル
 *
 * \author USAMI KOSHI
 * \date   2021/07/07
 *********************************************************************/

#include "Model.h"
#include <Engine/Engine.h>
#include <Engine/Utility/FBXParser.h>


bool Model::LoadFBXModel(const char* fileName, FBXModelData& out)
{
	bool result = true;
	FBXParser fbx;
	result = fbx.Load(fileName);
	if (!result) return result;

	// メッシュの作成
	auto* renderer = Engine::get().getRendererManager();
	auto meshID = renderer->createMesh(fileName);
	auto* pMesh = renderer->getMesh(meshID);
	pMesh->m_topology = PrimitiveTopology::TRIANGLE_LIST;
	out.meshID = meshID;

	//----- 頂点作成
	// 法線とUVのデータはインデックスに結びついている
	// そのデータを頂点のデータに結びつけるのは
	// 難しいので、逆に頂点のデータに合わせる
	int nVtxNum = fbx.getIndexNum();
	int* pIdx = fbx.getIndexList();
	float* pPos = fbx.getVertexList();
	float* pNrm = fbx.getNormalList();
	float* pUV = fbx.getUVList();
	int* pUVIdx = fbx.getUVIndexList();

	pMesh->m_vertexCount = nVtxNum;
	pMesh->m_vertexData.positions.resize(nVtxNum);
	pMesh->m_vertexData.normals.resize(nVtxNum);
	pMesh->m_vertexData.colors.resize(nVtxNum);
	pMesh->m_vertexData.texcoord0s.resize(nVtxNum);

	for (int i = 0; i < nVtxNum; ++i)
	{
		// 座標
		int idx = pIdx[i] * 3;
		if (pPos)
		{
			Vector3 pos;
			pos.x = pPos[idx + 0];
			pos.y = pPos[idx + 1];
			pos.z = pPos[idx + 2];
			pMesh->m_vertexData.positions[nVtxNum - 1 - i] = pos;
		}
		// 法線
		if (pNrm)
		{
			Vector3 nrm;
			nrm.x = pNrm[idx + 0];
			nrm.y = pNrm[idx + 1];
			nrm.z = pNrm[idx + 2];
			pMesh->m_vertexData.normals[nVtxNum - 1 - i] = nrm;
		}
		// カラー
		Vector4 col(1,1,1,1);
		pMesh->m_vertexData.colors[nVtxNum - 1 - i] = col;
		// UV
		if (pUV && pUVIdx)
		{
			idx = pUVIdx[i] * 2;
			Vector2 uv;
			uv.x = pUV[idx + 0];
			uv.y = 1.0f - pUV[idx + 1];
			pMesh->m_vertexData.texcoord0s[nVtxNum - 1 - i] = uv;
		}
	}

	// インデックスなし
	pMesh->m_indexCount = 0;

	// テクスチャの読み込み
	if (fbx.getTextureNum() > 0)
	{
		char localPath[MAX_PATH] = {};
		const char* fbxPath = fbx.getTextureList()[0].c_str();
		// ファイルネームからパスを取得
		int pathEnd = 0;
		int fileLen = strlen(fileName);
		for (int i = fileLen - 1; i >= 0; --i)
		{
			if (fileName[i] == '/')
			{
				pathEnd = i + 1;
				break;
			}
		}
		memcpy(localPath, fileName, pathEnd);
		// fbxのパスからファイル名を抜き出す
		int fbxPathLen = strlen(fbxPath);
		int fbxPathEnd = 0;
		for(int i = fbxPathLen - 1; i >= 0; --i)
		{ 
			if (fbxPath[i] == '/' || fbxPath[i] == '\\')
			{
				fbxPathEnd = i + 1;
				break;
			}
		}
		memcpy(localPath + pathEnd, fbxPath + fbxPathEnd, (fbxPathLen - fbxPathEnd) + 1);


		out.textureID = renderer->createTextureFromFile(localPath);
	}

	fbx.Release();
	return result;
}

