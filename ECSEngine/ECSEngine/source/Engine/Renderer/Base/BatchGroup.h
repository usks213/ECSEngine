/*****************************************************************//**
 * \file   BatchGroup.h
 * \brief  バッチ描画データ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/07
 *********************************************************************/
#pragma once

#include "Material.h"
#include "Mesh.h"

 /// @brief バッチ描画ID
using BatchGroupID = std::uint32_t;

/// @class BatchGroup
/// @brief バッチ描画データ
class BatchGroup
{
public:
	explicit BatchGroup(const BatchGroupID& id, const MaterialID& materialID, const MeshID& meshID) :
		m_id(id), m_materialID(materialID), m_meshID(meshID)
	{}

	~BatchGroup() = default;

public:
	BatchGroupID	m_id;
	MaterialID		m_materialID;
	MeshID			m_meshID;
};