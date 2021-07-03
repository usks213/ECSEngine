/*****************************************************************//**
 * \file   Geometry.cpp
 * \brief  ジオメトリ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/03
 *********************************************************************/

#include "Geometry.h"

struct VERTEX_3D 
{
	XMFLOAT3 vtx;		// 頂点座標
	XMFLOAT3 nor;		// 法線ベクトル
	XMFLOAT4 diffuse;	// 拡散反射光
	XMFLOAT2 tex;		// テクスチャ座標
};


void Geometry::Cube(Mesh& out)
{
	const float	SIZE_X = (0.5f); // 立方体のサイズ(X方向)
	const float	SIZE_Y = (0.5f); // 立方体のサイズ(Y方向)
	const float	SIZE_Z = (0.5f); // 立方体のサイズ(Z方向)

	const int CUBE_VERTEX = (24);
	const int CUBE_INDEX = (36);

	// プリミティブ設定
	//m_mesh->primitiveType = PT_TRIANGLE;

	VERTEX_3D	vertexWk[CUBE_VERTEX];	// 頂点情報格納ワーク
	int			indexWk[CUBE_INDEX];	// インデックス格納ワーク

	// 頂点座標の設定
	// 前
	vertexWk[0].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[1].vtx = XMFLOAT3(SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[2].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[3].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, -SIZE_Z);
	// 右
	vertexWk[4].vtx = XMFLOAT3(SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[5].vtx = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[6].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[7].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, SIZE_Z);
	// 上
	vertexWk[8].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[9].vtx = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[10].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[11].vtx = XMFLOAT3(SIZE_X, SIZE_Y, -SIZE_Z);
	// 後
	vertexWk[12].vtx = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[13].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[14].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[15].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, SIZE_Z);
	// 左
	vertexWk[16].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[17].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[18].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[19].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, -SIZE_Z);
	// 下
	vertexWk[20].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[21].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[22].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[23].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, -SIZE_Z);


	// 法線ベクトルの設定
	// 前
	vertexWk[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertexWk[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertexWk[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertexWk[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	// 右
	vertexWk[4].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexWk[5].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexWk[6].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexWk[7].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	// 上
	vertexWk[8].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexWk[9].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexWk[10].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexWk[11].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	// 後
	vertexWk[12].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertexWk[13].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertexWk[14].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertexWk[15].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	// 左
	vertexWk[16].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertexWk[17].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertexWk[18].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertexWk[19].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	// 下
	vertexWk[20].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertexWk[21].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertexWk[22].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertexWk[23].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);


	// 拡散反射光の設定
	for (int i = 0; i < CUBE_VERTEX; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	for (int i = 0; i < CUBE_VERTEX; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// インデックス配列の設定
	for (int i = 0, j = 0; i < CUBE_INDEX; i += 6, j += 4)
	{
		indexWk[0 + i] = 0 + j;
		indexWk[1 + i] = 1 + j;
		indexWk[2 + i] = 2 + j;
		indexWk[3 + i] = 2 + j;
		indexWk[4 + i] = 1 + j;
		indexWk[5 + i] = 3 + j;
	}

	// 頂点生成
	out.m_vertexCount = CUBE_VERTEX;
	for (int i = 0; i < out.m_vertexCount; ++i)
	{
		out.m_vertexData.positions.push_back(vertexWk[i].vtx);
		out.m_vertexData.normals.push_back(vertexWk[i].nor);
		out.m_vertexData.texcoord0s.push_back(vertexWk[i].tex);
		out.m_vertexData.colors.push_back(vertexWk[i].diffuse);
	}

	// インデックス
	out.m_indexCount = CUBE_INDEX;
	for (int i = 0; i < out.m_indexCount; ++i)
	{
		out.m_indexData.push_back(static_cast<std::uint32_t>(indexWk[i]));
	}
}

void Geometry::Sphere(Mesh& out, int nSplit, float fSize, float fTexSize)
{
	float nNumBlockX = nSplit;
	float nNumBlockY = nSplit;

	// プリミティブ種別設定
	//m_mesh->primitiveType = PT_TRIANGLESTRIP;
	// 頂点数の設定
	int nNumVertex = (nNumBlockX + 1) * (nNumBlockY + 1);
	// インデックス数の設定(縮退ポリゴン用を考慮する)
	int nNumIndex = (nNumBlockX + 1) * 2 * nNumBlockY + (nNumBlockY - 1) * 2;
	// 頂点配列の作成
	VERTEX_3D* pVertexWk = new VERTEX_3D[nNumVertex];
	// インデックス配列の作成
	int* pIndexWk = new int[nNumIndex];
	// 頂点配列の中身を埋める
	VERTEX_3D* pVtx = pVertexWk;

	for (int y = 0; y < nNumBlockY + 1; ++y) {
		for (int x = 0; x < nNumBlockX + 1; ++x) {

			// 頂点座標の設定
			pVtx->vtx.x = 0.0f;
			pVtx->vtx.y = 1.0f;
			pVtx->vtx.z = 0.0f;
			// 角度に対する回転マトリックスを求める
			XMMATRIX mR = XMMatrixRotationX(XMConvertToRadians(-x * (-180.0f / nNumBlockX)));
			mR *= XMMatrixRotationY(XMConvertToRadians(-y * (360.0f / nNumBlockY)));
			// 座標を回転マトリックスで回転させる
			XMVECTOR v = XMLoadFloat3(&pVtx->vtx);
			v = XMVector3TransformCoord(v, mR);
			v = XMVector3Normalize(v);
			XMStoreFloat3(&pVtx->vtx, v);

			// 法線の設定
			pVtx->nor = pVtx->vtx;

			// 大きさ
			pVtx->vtx.x *= fSize;
			pVtx->vtx.y *= fSize;
			pVtx->vtx.z *= fSize;

			// 位置
			//pVtx->vtx.x += fPosX;
			//pVtx->vtx.y += fPosY;
			//pVtx->vtx.z += fPosZ;

			// 反射光の設定
			pVtx->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			// テクスチャ座標の設定
			pVtx->tex.x = fTexSize * x;
			pVtx->tex.y = fTexSize * y;
			++pVtx;
		}
	}
	//インデックス配列の中身を埋める
	int* pIdx = pIndexWk;
	for (int z = 0; z < nNumBlockY; ++z) {
		if (z > 0) {
			// 縮退ポリゴンのためのダブりの設定
			*pIdx++ = (z + 1) * (nNumBlockX + 1);
		}
		for (int x = 0; x < nNumBlockX + 1; ++x) {
			*pIdx++ = (z + 1) * (nNumBlockX + 1) + x;
			*pIdx++ = z * (nNumBlockX + 1) + x;
		}
		if (z < nNumBlockY - 1) {
			// 縮退ポリゴンのためのダブりの設定
			*pIdx++ = z * (nNumBlockX + 1) + nNumBlockX;
		}
	}

	// 頂点バッファ/インデックス バッファ生成
	// 頂点生成
	out.m_vertexCount = nNumVertex;
	for (int i = 0; i < out.m_vertexCount; ++i)
	{
		out.m_vertexData.positions.push_back(pVertexWk[i].vtx);
		out.m_vertexData.normals.push_back(pVertexWk[i].nor);
		out.m_vertexData.texcoord0s.push_back(pVertexWk[i].tex);
		out.m_vertexData.colors.push_back(pVertexWk[i].diffuse);
	}

	// インデックス
	out.m_indexCount = nNumIndex;
	for (int i = 0; i < out.m_indexCount; ++i)
	{
		out.m_indexData.push_back(pIndexWk[i]);
	}

	// 一時配列の解放
	delete[] pVertexWk;
	delete[] pIndexWk;
}
