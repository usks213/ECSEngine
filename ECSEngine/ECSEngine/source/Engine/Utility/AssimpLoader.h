/*****************************************************************//**
 * \file   AssimpLoader.h
 * \brief  AssimpModelのロード
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
private:
	///// @brief コンストラクタ
	//AssimpLoader();
	///// @brief デストラクタ
	//~AssimpLoader();

public:

	bool LoadModel(RendererManager* renderer, std::string path, Model& out);

};

