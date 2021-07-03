/*****************************************************************//**
 * \file   Geometry.h
 * \brief  ジオメトリ
 * 
 * \author USAMI KOSHI
 * \date   2021/07/03
 *********************************************************************/
#pragma once

#include "Mesh.h"

class Geometry
{
public:
	static void Cube(Mesh& out);
	static void Sphere(Mesh& out, int nSplit, float fSize, float fTexSize);

};
