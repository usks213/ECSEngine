/*****************************************************************//**
 * \file   RenderBuffer.h
 * \brief  描画データバッファ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/25
 *********************************************************************/
#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Vertex.h"

/// @brief 64 = 32:ShaderID + 32:MeshID
using RenderBufferID = std::uint32_t;
/// @brief 存在しないレンダーバッファID
constexpr RenderBufferID NONE_RENDERBUFFER_ID = std::numeric_limits<RenderBufferID>::max();

struct IndexData
{
	IndexData(const std::size_t& count, const std::size_t& size) :
		count(count), size(size)
	{}
public:
	std::size_t	count;
	std::size_t	size;
	// インデックスのデータタイプ? 今は固定
	// IndexDataType
};

class RenderBuffer
{
public:
	explicit RenderBuffer(const RenderBufferID id, const Shader& shader, const Mesh& mesh) :
		m_id(id),
		m_shaderID(shader.m_id),
		m_meshID(mesh.m_id),
		m_vertexData(shader, mesh.m_vertexCount),
		m_indexData(mesh.m_indexCount, sizeof(std::uint32_t)),
		m_topology(mesh.m_topology)
	{
		// 頂点データの生成
		const auto& verData = mesh.m_vertexData;
		if(m_vertexData.hasVertexVariable(SEMANTIC_NAME::POSITION))
		for (std::uint32_t i = 0; i < verData.positions.size(); ++i) {
			m_vertexData.setPosition(verData.positions[i], i);
		}
		if (m_vertexData.hasVertexVariable(SEMANTIC_NAME::NORMAL))
		for (std::uint32_t i = 0; i < verData.normals.size(); ++i) {
			m_vertexData.setNormal(verData.normals[i], i);
		}
		if (m_vertexData.hasVertexVariable(SEMANTIC_NAME::TANGENT))
		for (std::uint32_t i = 0; i < verData.tangents.size(); ++i) {
			m_vertexData.setTangent(verData.tangents[i], i);
		}
		if (m_vertexData.hasVertexVariable(SEMANTIC_NAME::BINORMAL))
		for (std::uint32_t i = 0; i < verData.binormals.size(); ++i) {
			m_vertexData.setBinormal(verData.binormals[i], i);
		}
		if (m_vertexData.hasVertexVariable(SEMANTIC_NAME::COLOR))
		for (std::uint32_t i = 0; i < verData.colors.size(); ++i) {
			m_vertexData.setColor(verData.colors[i], i);
		}
		if (m_vertexData.hasVertexVariable(SEMANTIC_NAME::TEXCOORD, 0))
		for (std::uint32_t i = 0; i < verData.texcoord0s.size(); ++i) {
			m_vertexData.setTexCoord(verData.texcoord0s[i], 0, i);
		}
		if (m_vertexData.hasVertexVariable(SEMANTIC_NAME::TEXCOORD, 1))
		for (std::uint32_t i = 0; i < verData.texcoord1s.size(); ++i) {
			m_vertexData.setTexCoord(verData.texcoord1s[i], 1, i);
		}
		if (m_vertexData.hasVertexVariable(SEMANTIC_NAME::BONE_WEIGHT))
		for (std::uint32_t i = 0; i < verData.boneWeights.size(); ++i) {
			m_vertexData.setBoneWeight(verData.boneWeights[i], i);
		}
		if (m_vertexData.hasVertexVariable(SEMANTIC_NAME::BONE_INDEX))
		for (std::uint32_t i = 0; i < verData.boneIndexs.size(); ++i) {
			m_vertexData.setBoneIndex(verData.boneIndexs[i], i);
		}
	}


public:
	RenderBufferID		m_id;
	ShaderID			m_shaderID;
	MeshID				m_meshID;

	VertexData			m_vertexData;
	IndexData			m_indexData;
	PrimitiveTopology	m_topology;
};
