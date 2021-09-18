/*****************************************************************//**
 * \file   AssimpLoader.h
 * \brief  AssimpModel�̃��[�h
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
	///// @brief �R���X�g���N�^
	//AssimpLoader();
	///// @brief �f�X�g���N�^
	//~AssimpLoader();

public:

	bool LoadModel(RendererManager* renderer, std::string path, Model& out);

};

