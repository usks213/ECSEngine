/*****************************************************************//**
 * \file   AssimpLoader.h
 * \brief  AssimpModelのロード
 *
 * \author USAMI KOSHI
 * \date   2021/09/19
 *********************************************************************/

#include "AssimpLoader.h"
#include <Engine/Renderer/Base/RendererManager.h>
#include <Engine/Renderer/Base/Model.h>


 // Assimpライブラリ
#include <tuple>
#include <time.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc141-mtd")
#pragma comment(lib, "IrrXMLd")
#pragma comment(lib, "zlibstaticd")
#else
#pragma comment(lib, "assimp-vc141-mt")
#pragma comment(lib, "IrrXML")
#pragma comment(lib, "zlibstatic")
#endif

//===== プロトタイプ宣言 =====
// ノード探索
void processNode(aiScene* scene, aiNode* node, RendererManager* renderer, Model& model, Model::MeshNode& self);
// メッシュ展開
void processMesh(aiScene* scene, aiMesh* mesh, RendererManager* renderer, Model::MeshNode& self);


///// @brief コンストラクタ
//AssimpLoader::AssimpLoader()
//{
//}
//
///// @brief デストラクタ
//AssimpLoader::~AssimpLoader()
//{
//}


bool AssimpLoader::LoadModel(RendererManager* renderer, std::string path, Model& out)
{
	// default pp steps
	static unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
		aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
		aiProcess_ValidateDataStructure | // perform a full validation of the loader's output
		aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
		aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
		aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
		aiProcess_LimitBoneWeights | // limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes | // join small meshes, if possible;
		aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
		0;

	aiPropertyStore* props = aiCreatePropertyStore();
	aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
	//aiSetImportPropertyFloat(props, AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, g_smoothAngle);
	//aiSetImportPropertyInteger(props, AI_CONFIG_PP_SBP_REMOVE, nopointslines ? aiPrimitiveType_LINE | aiPrimitiveType_POINT : 0);
	aiSetImportPropertyInteger(props, AI_CONFIG_GLOB_MEASURE_TIME, 1);
	auto scene = (aiScene*)aiImportFileExWithProperties(path.c_str(),
		ppsteps |
		aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
		aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
		aiProcess_Triangulate | // triangulate polygons with more than 3 edges
		//aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space
		aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
		0, NULL, props);
	aiReleasePropertyStore(props);
	if (!scene)
	{
		// シーン解放
		aiReleaseImport(scene);
		return false;
	}

	// パス
	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
	_splitpath_s(path.c_str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);
	std::string directory = szDrive;
	directory += szDir;
	std::string fname = szFName;
	std::string ext = szExt;

	// ルートノードからメッシュの読み込み
	if (scene->HasMeshes())
	{
		processNode(scene, scene->mRootNode, renderer, out, out.m_rootMesh);
	}

	// シーン解放
	aiReleaseImport(scene);

	return true;
}

// ノード探索
void processNode(aiScene* scene, aiNode* node, RendererManager* renderer, Model& model, Model::MeshNode& self)
{
	// ノード名
	self.name = node->mName.C_Str();
	// 事前トランスフォーム
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; ++j)
			self.transform.m[i][j] = node->mTransformation[i][j];

	// 自身のメッシュ読み込み
	for (UINT i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		// メッシュ読み込み
		processMesh(scene, mesh, renderer, self);
	}

	// 子のメッシュ読み込み
	for (UINT i = 0; i < node->mNumChildren; ++i) {

		// 子の生成
		auto childNode = std::make_unique<Model::MeshNode>();
		auto* pChild = childNode.get();
		childNode->self = model.getMeshInfoID();
		self.childMeshes.push_back(childNode->self);
		model.m_meshNodes[childNode->self] = std::move(childNode);
		// 子を親として探索
		processNode(scene, node->mChildren[i], renderer, model, *pChild);
	}

}

// メッシュ展開
void processMesh(aiScene* scene, aiMesh* mesh, RendererManager* renderer, Model::MeshNode& self)
{
	// メッシュの生成
	auto meshID = renderer->createMesh(mesh->mName.C_Str());
	auto pMesh = renderer->getMesh(meshID);
	pMesh->m_topology = PrimitiveTopology::TRIANGLE_LIST;

	// メッシュ情報追加
	Model::MeshNode::MeshInfo meshInfo;
	meshInfo.meshID = meshID;
	meshInfo.materialIndex = mesh->mMaterialIndex;
	self.meshList.push_back(meshInfo);

	// 全頂点のウェイトを収集(高速化のためチェック無)
	std::vector<std::vector<aiVertexWeight> >weightsPerVertex(mesh->mNumVertices);
	for (unsigned int a = 0; a < mesh->mNumBones; a++) {
		const aiBone* bone = mesh->mBones[a];
		for (unsigned int b = 0; b < bone->mNumWeights; b++)
			weightsPerVertex[bone->mWeights[b].mVertexId].push_back(aiVertexWeight(a, bone->mWeights[b].mWeight));
	}

	// メッシュの各頂点をウォークスルー
	auto& vertex = pMesh->m_vertexData;
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		// 座標
		Vector3 pos;
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		vertex.positions.push_back(pos);
		// 法線
		if (mesh->mNormals) {
			Vector3 norm;
			norm.x = mesh->mNormals[i].x;
			norm.y = mesh->mNormals[i].y;
			norm.z = mesh->mNormals[i].z;
			vertex.normals.push_back(norm);
		}
		// テクスチャ座標0
		if (mesh->mTextureCoords[0]) {
			Vector2 tex;
			tex.x = (float)mesh->mTextureCoords[0][i].x;
			tex.y = (float)mesh->mTextureCoords[0][i].y;
			vertex.texcoord0s.push_back(tex);
		}
		// テクスチャ座標1
		if (mesh->mTextureCoords[1]) {
			Vector2 tex;
			tex.x = (float)mesh->mTextureCoords[1][i].x;
			tex.y = (float)mesh->mTextureCoords[1][i].y;
			vertex.texcoord1s.push_back(tex);
		}
		// ボーンのインデックスとウェイト
		if (mesh->HasBones()) {
			VectorUint4 boneIndices;
			float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			for (unsigned int a = 0; a < weightsPerVertex[i].size() && a < 4; a++) {
				boneIndices.n[a] = weightsPerVertex[i][a].mVertexId;
				boneWeights[a] = weightsPerVertex[i][a].mWeight;
			}
			Vector4 weights(boneWeights[0], boneWeights[1], boneWeights[2], boneWeights[3]);
			vertex.boneIndexs.push_back(boneIndices);
			vertex.boneWeights.push_back(weights);
		}
	}
	pMesh->m_vertexCount = pMesh->m_vertexData.positions.size();


	// インデックス格納
	for (UINT i = 0; i < mesh->mNumFaces; ++i) 
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; ++j)
		{
			pMesh->m_indexData.push_back(face.mIndices[j]);
		}
	}
	pMesh->m_indexCount = pMesh->m_indexData.size();

}