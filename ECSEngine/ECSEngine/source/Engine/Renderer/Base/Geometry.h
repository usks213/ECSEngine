/*****************************************************************//**
 * \file   Geometry.h
 * \brief  ÉWÉIÉÅÉgÉä
 * 
 * \author USAMI KOSHI
 * \date   2021/07/03
 *********************************************************************/
#pragma once

#include "Mesh.h"
#include "SubResource.h"

class Geometry
{
public:
	static void Plane(Mesh& out, int split = 10, float size = 1.0f, float texSize = 0.1f);
	static void Cube(Mesh& out);
	static void Sphere(Mesh& out, int nSplit, float fSize, float fTexSize);
	static void SkyDome(Mesh& out, int nSegment, float fTexSplit);
	static void Terrain(Mesh& out, int split, float height, float texSize = 1.0f);
};
