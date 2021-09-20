/*****************************************************************//**
 * \file   Model.h
 * \brief  モデル(骨なし)
 * 
 * \author USAMI KOSHI
 * \date   2021/09/19
 *********************************************************************/
#pragma once

#include "Mesh.h"
#include <unordered_map>


class Model
{
public:
	// インデックス
	using Index = std::uint32_t;
	static constexpr Index NONE_INDEX = std::numeric_limits<Index>::max();

	// メッシュ情報
	struct MeshInfo {
		MeshID meshID = NONE_MESH_ID;
		Index materialIndex = NONE_INDEX;
		// BoneData
		// BoneIndex		// どの骨格か
		// BoneIndexList;	// その骨格で対応する骨リスト
	};
	// Bone情報
	struct BoneInfo
	{
		Matrix matrix;	// デフォルト姿勢
		Quaternion preRotation;
	};

	// マテリアル情報
	struct MaterialInfo
	{
		std::string name;
		// テクスチャ
		TextureID DiffuseTex;
		TextureID NormalTex;
		TextureID HeightTex;
		TextureID MRATex;	// メタリック、ラフネス、アンビエントオクルージョン
	};

	// アニメーション情報
	struct AnimationInfo
	{

	};

	// 階層構造
	using NodeID = std::uint32_t;
	struct NodeInfo
	{
		NodeID					self = 0;
		std::string				name;
		std::vector<NodeID>		childs;
		Matrix					transform;

		std::vector<Index>		meshIndexes;
	};

public:
	// ルートノード
	NodeInfo								m_rootNode;
	// ノードリスト(ルート以外)
	std::unordered_map<NodeID, NodeInfo>	m_nodeMap;

	// メッシュリスト
	std::vector<MeshInfo>					m_meshList;

	// ボーン


	// アニメーション

	// マテリアル
	std::vector<MaterialInfo>					m_materialList;

	// テクスチャ





private:
	NodeID m_nodeCount = 0;

public:
	/// @brief コンストラクタ
	Model()
	{
	}

	/// @brief デストラクタ
	~Model() = default;


	NodeID getMeshInfoID() {
		return ++m_nodeCount;
	}
};
