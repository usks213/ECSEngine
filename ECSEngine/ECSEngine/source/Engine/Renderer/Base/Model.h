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
	using MeshNodeID = std::uint32_t;
	struct MeshNode
	{
		MeshNodeID				self = 0;
		std::string				name;
		std::vector<MeshNodeID>	childMeshes;
		Matrix					transform;
		struct MeshInfo {
			MeshID meshID = NONE_MESH_ID;
			std::uint32_t materialIndex = 0;
		};
		std::vector<MeshInfo>		meshList;
	};

	// メッシュリスト(ルート以外)
	std::unordered_map<MeshNodeID, std::unique_ptr<MeshNode>>	m_meshNodes;
	// ルートメッシュ
	MeshNode m_rootMesh;

	// マテリアル
	std::vector<MaterialID> m_materialList;

	// ボーン

	// アニメーション


private:
	MeshNodeID m_MeshInfoIDCount = 0;

public:
	/// @brief コンストラクタ
	Model()
	{
	}

	/// @brief デストラクタ
	~Model() = default;


	MeshNodeID getMeshInfoID() {
		return ++m_MeshInfoIDCount;
	}
};
