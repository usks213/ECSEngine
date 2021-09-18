/*****************************************************************//**
 * \file   Model.h
 * \brief  ���f��(���Ȃ�)
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

	// ���b�V�����X�g(���[�g�ȊO)
	std::unordered_map<MeshNodeID, std::unique_ptr<MeshNode>>	m_meshNodes;
	// ���[�g���b�V��
	MeshNode m_rootMesh;

	// �}�e���A��
	std::vector<MaterialID> m_materialList;

	// �{�[��

	// �A�j���[�V����


private:
	MeshNodeID m_MeshInfoIDCount = 0;

public:
	/// @brief �R���X�g���N�^
	Model()
	{
	}

	/// @brief �f�X�g���N�^
	~Model() = default;


	MeshNodeID getMeshInfoID() {
		return ++m_MeshInfoIDCount;
	}
};
