/*****************************************************************//**
 * \file   Model.h
 * \brief  モデル(骨なし)
 * 
 * \author USAMI KOSHI
 * \date   2021/09/19
 *********************************************************************/
#pragma once

#include "Mesh.h"
#include "Animation.h"
#include <unordered_map>
#include <map>


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
		// ボーン情報
		std::string rootBoneName;
		std::unordered_map<std::string, Index> boneTable;
		// BoneIndex		// どの骨格か
		// BoneIndexList;	// その骨格で対応する骨リスト
	};

	// アバター情報
	struct AvatarInfo
	{
		std::string name;
		std::unordered_map<std::string, Index> boneTable;
	};

	// アニメーション情報
	struct AnimationInfo
	{
		std::string name;
		AnimationID animationID = NONE_ANIMATION_ID;
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
	// モデル名
	std::string								m_name;

	// ルートノード
	NodeInfo								m_rootNode;
	// ノードリスト(ルート以外)
	std::unordered_map<NodeID, NodeInfo>	m_nodeMap;

	// メッシュリスト
	std::vector<MeshInfo>					m_meshList;

	// アバター
	std::unique_ptr<AvatarInfo>				m_avatar;

	// アニメーション
	std::vector<AnimationInfo>				m_animationList;

	// マテリアル
	std::vector<MaterialInfo>				m_materialList;

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
