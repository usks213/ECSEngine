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
	// �C���f�b�N�X
	using Index = std::uint32_t;
	static constexpr Index NONE_INDEX = std::numeric_limits<Index>::max();

	// ���b�V�����
	struct MeshInfo {
		MeshID meshID = NONE_MESH_ID;
		Index materialIndex = NONE_INDEX;
		// BoneData
		// BoneIndex		// �ǂ̍��i��
		// BoneIndexList;	// ���̍��i�őΉ����鍜���X�g
	};
	// Bone���
	struct BoneInfo
	{
		Matrix matrix;	// �f�t�H���g�p��
		Quaternion preRotation;
	};

	// �}�e���A�����
	struct MaterialInfo
	{
		std::string name;
		// �e�N�X�`��
		TextureID DiffuseTex;
		TextureID NormalTex;
		TextureID HeightTex;
		TextureID MRATex;	// ���^���b�N�A���t�l�X�A�A���r�G���g�I�N���[�W����
	};

	// �A�j���[�V�������
	struct AnimationInfo
	{

	};

	// �K�w�\��
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
	// ���[�g�m�[�h
	NodeInfo								m_rootNode;
	// �m�[�h���X�g(���[�g�ȊO)
	std::unordered_map<NodeID, NodeInfo>	m_nodeMap;

	// ���b�V�����X�g
	std::vector<MeshInfo>					m_meshList;

	// �{�[��


	// �A�j���[�V����

	// �}�e���A��
	std::vector<MaterialInfo>					m_materialList;

	// �e�N�X�`��





private:
	NodeID m_nodeCount = 0;

public:
	/// @brief �R���X�g���N�^
	Model()
	{
	}

	/// @brief �f�X�g���N�^
	~Model() = default;


	NodeID getMeshInfoID() {
		return ++m_nodeCount;
	}
};
