/*****************************************************************//**
 * \file   Mesh.h
 * \brief  メッシュ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/25
 *********************************************************************/
#pragma once

#include <Engine/Utility/Mathf.h>
#include <vector>
#include <string>
#include "CommonState.h"


/// @brief メッシュID
using MeshID = std::uint32_t;
/// @brief 存在しないメッシュID
constexpr MeshID NONE_MESH_ID = std::numeric_limits<MeshID>::max();


struct Mesh
{
public:
	explicit Mesh(const MeshID& id, const std::string& name):
		m_name(name),
		m_id(id),
		m_vertexCount(0),
		m_indexCount(0),
		m_topology(PrimitiveTopology::TRIANGLE_STRIP)
	{}

public:
	struct VertexInfo
	{
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector3> tangents;
		std::vector<Vector3> binormals;
		std::vector<Vector4> colors;
		std::vector<Vector2> texcoord0s;
		std::vector<Vector2> texcoord1s;
		std::vector<Vector4> boneWeights;
		std::vector<VectorUint4> boneIndexs;
	};
	struct SubMesh
	{
		std::uint32_t start = 0;
		std::uint32_t count = 0;
	};

	std::string					m_name;
	MeshID						m_id;
	std::vector<SubMesh>		m_subMeshes;

	std::uint32_t				m_vertexCount;
	VertexInfo					m_vertexData;
	std::uint32_t				m_indexCount;
	std::vector<std::uint32_t>  m_indexData;

	std::uint32_t				m_boneCount;
	std::vector<Matrix>			m_invMatrixData;
	std::vector<Matrix>			m_calcBoneBuffer;
	std::unordered_map<std::string, std::uint32_t>	m_boneTabel;

	std::uint32_t				m_heightWidth;
	std::vector<float>			m_heightData;

	AABB						m_aabb;
	PrimitiveTopology			m_topology;
};
