/*****************************************************************//**
 * \file   AssimpLoader.h
 * \brief  AssimpModelÇÃÉçÅ[Éh
 * 
 * \author USAMI KOSHI
 * \date   2021/09/19
 *********************************************************************/
#pragma once

#include "Singleton.h"
#include <string>

class RendererManager;
class Model;

class AssimpLoader final : public Singleton<AssimpLoader>
{
public:
	bool LoadModel(RendererManager* renderer, std::string path, Model& out);

private:


};

