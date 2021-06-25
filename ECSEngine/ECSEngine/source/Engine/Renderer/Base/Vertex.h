/*****************************************************************//**
 * \file   Vertex.h
 * \brief  頂点データ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/24
 *********************************************************************/
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <Engine/Utility/Mathf.h>
#include "Shader.h"

struct VertexData
{
	std::size_t count;						// 頂点数
	std::size_t size;						// 一頂点サイズ
	std::unique_ptr<std::byte[]> buffer;	// 全頂点データ
	ShaderID shaderID;
	std::vector<Shader::InputLayoutVariable> inputLayoutVariableList;

	VertexData(const Shader& shader, std::size_t vertexNum) :
		size(0),count(0),shaderID(0)
	{
		/*inputLayoutVariableList.reserve(shader.m_inputLayoutVariableList.size());
		std::copy(inputLayoutVariableList.begin(), inputLayoutVariableList.end(),
			std::back_inserter(shader.m_inputLayoutVariableList));*/
		inputLayoutVariableList = shader.m_inputLayoutVariableList;

		for (auto& var : inputLayoutVariableList) {
			size += static_cast<std::size_t>(var.formatSize) * sizeof(float);
		}
		count = vertexNum;
		shaderID = shader.m_id;
		buffer = std::make_unique<std::byte[]>(size * count);
	}

	void setPosition(const Vector3& data, const std::uint32_t& index) {
		setVertexData<Vector3>(SEMANTIC_NAME::POSITION, 0, data, index);
	}
	void setNormal(const Vector3& data, const std::uint32_t& index) {
		setVertexData<Vector3>(SEMANTIC_NAME::NORMAL, 0, data, index);
	}
	void setTangent(const Vector3& data, const std::uint32_t& index) {
		setVertexData<Vector3>(SEMANTIC_NAME::TANGENT, 0, data, index);
	}
	void setBinormal(const Vector3& data, const std::uint32_t& index) {
		setVertexData<Vector3>(SEMANTIC_NAME::BINORMAL, 0, data, index);
	}
	void setColor(const Vector4& data, const std::uint32_t& index) {
		setVertexData<Vector4>(SEMANTIC_NAME::COLOR, 0, data, index);
	}
	void setTexCoord(const Vector2& data, const std::uint32_t& semanticIndex,
		const std::uint32_t& index) {
		setVertexData<Vector2>(SEMANTIC_NAME::TEXCOORD, semanticIndex, data, index);
	}
	void setBoneWeight(const Vector4& data, const std::uint32_t& index) {
		setVertexData<Vector4>(SEMANTIC_NAME::BONE_WEIGHT, 0, data, index);
	}
	void setBoneIndex(const VectorUint4& data, const std::uint32_t& index) {
		setVertexData<VectorUint4>(SEMANTIC_NAME::BONE_INDEX, 0, data, index);
	}

	template<class T>
	void setVertexData(std::string_view semanticName, const std::uint32_t& semanticIndex,
		const T& data, const std::uint32_t& index) {
		if (hasVertexVariable(semanticName, semanticIndex)) {
			std::memcpy(buffer.get() + size * index + var.offset, &data,
				static_cast<std::size_t>(var.formatSize) * sizeof(float));
		}
	}

	bool hasVertexVariable(std::string_view semanticName, const std::uint32_t& semanticIndex = 0) {
		for (auto& var : inputLayoutVariableList) {
			if (var.semanticName == semanticName && var.semanticIndex == semanticIndex) {
				return true;
			}
		}
		return false;
	}
};



