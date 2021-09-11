/*****************************************************************//**
 * \file   Model.h
 * \brief  ƒ‚ƒfƒ‹
 * 
 * \author USAMI KOSHI
 * \date   2021/07/07
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/RendererManager.h>


class Model
{
public:
	struct FBXModelData
	{
		std::string fileName;
		MeshID meshID = NONE_MESH_ID;
		TextureID textureID = NONE_TEXTURE_ID;
	};

public:
	static bool LoadFBXModel(const char* fileName, FBXModelData& out);

};

