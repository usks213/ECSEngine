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
		m_indexCount(0)
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

	std::string					m_name;
	MeshID						m_id;

	std::uint32_t				m_vertexCount;
	VertexInfo					m_vertexData;
	std::uint32_t				m_indexCount;
	std::vector<std::uint32_t>  m_indexData;
	AABB						m_aabb;
};
