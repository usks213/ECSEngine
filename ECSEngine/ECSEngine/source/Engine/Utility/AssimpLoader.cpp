/*****************************************************************//**
 * \file   AssimpLoader.h
 * \brief  AssimpModel�̃��[�h
 *
 * \author USAMI KOSHI
 * \date   2021/09/19
 *********************************************************************/

#include "AssimpLoader.h"
#include <Engine/Renderer/Base/RendererManager.h>
#include <Engine/Renderer/Base/Model.h>


 // Assimp���C�u����
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

//===== �v���g�^�C�v�錾 =====
// �m�[�h�ǂݍ��݁E�쐬
void LoadNode(aiScene* scene, aiNode* node, Model& model, Model::NodeInfo& self);
// ���b�V���̓ǂݍ���
void LoadMesh(aiMesh* mesh, RendererManager* renderer, Model::MeshInfo& meshInfo);
// �}�e���A���̓ǂݍ���
void loadMaterial(aiScene* scene, aiMaterial* mat, RendererManager* renderer, Model::MaterialInfo& matInfo);
// �e�N�X�`���̓ǂݍ���
TextureID LoadTexture(aiScene* scene, aiString& szPath, RendererManager* renderer);

///// @brief �R���X�g���N�^
//AssimpLoader::AssimpLoader()
//{
//}
//
///// @brief �f�X�g���N�^
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
		return false;
	}

	// �p�X
	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
	_splitpath_s(path.c_str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);
	std::string directory = szDrive;
	directory += szDir;
	std::string fname = szFName;
	std::string ext = szExt;

	// ���b�V���̓ǂݍ���
	if (scene->HasMeshes())
	{
		out.m_meshList.resize(scene->mNumMeshes);
		for (std::uint32_t i = 0; i < scene->mNumMeshes; ++i)
		{
			LoadMesh(scene->mMeshes[i], renderer, out.m_meshList[i]);
		}
	}

	// �}�e���A���ǂݍ���
	if (scene->HasMaterials())
	{
		out.m_materialList.resize(scene->mNumMaterials);
		for (std::uint32_t i = 0; i < scene->mNumMaterials; ++i)
		{
			loadMaterial(scene, scene->mMaterials[i], renderer, out.m_materialList[i]);
		}
	}

	// �m�[�h�̍쐬
	LoadNode(scene, scene->mRootNode, out, out.m_rootNode);
	out.m_rootNode.name = fname;

	// �V�[�����
	aiReleaseImport(scene);

	return true;
}

// �m�[�h�T��
void LoadNode(aiScene* scene, aiNode* node, Model& model, Model::NodeInfo& self)
{
	// �m�[�h��
	self.name = node->mName.C_Str();

	// ���O�g�����X�t�H�[��
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; ++j)
			self.transform.m[i][j] = node->mTransformation[i][j];

	// �Ή����b�V���i�[
	self.meshIndexes.resize(node->mNumMeshes);
	for (UINT i = 0; i < node->mNumMeshes; ++i) 
	{
		self.meshIndexes[i] = node->mMeshes[i];
	}

	// �q�̃��b�V���ǂݍ���
	for (UINT i = 0; i < node->mNumChildren; ++i) 
	{
		// �q�̐���
		auto id = model.getMeshInfoID();
		auto& childNode = model.m_nodeMap[id];
		childNode.self = id;
		self.childs.push_back(id);
		// �q��e�Ƃ��ĒT��
		LoadNode(scene, node->mChildren[i], model, childNode);
	}

}

// ���b�V���W�J
void LoadMesh(aiMesh* mesh, RendererManager* renderer, Model::MeshInfo& meshInfo)
{
	// ���b�V���̐���
	auto meshID = renderer->createMesh(mesh->mName.C_Str());
	auto pMesh = renderer->getMesh(meshID);
	pMesh->m_topology = PrimitiveTopology::TRIANGLE_LIST;

	// ���b�V�����ǉ�
	meshInfo.meshID = meshID;
	meshInfo.materialIndex = mesh->mMaterialIndex;

	// �S���_�̃E�F�C�g�����W(�������̂��߃`�F�b�N��)
	std::vector<std::vector<aiVertexWeight> >weightsPerVertex(mesh->mNumVertices);
	for (unsigned int a = 0; a < mesh->mNumBones; a++) {
		const aiBone* bone = mesh->mBones[a];
		for (unsigned int b = 0; b < bone->mNumWeights; b++)
			weightsPerVertex[bone->mWeights[b].mVertexId].push_back(aiVertexWeight(a, bone->mWeights[b].mWeight));
	}

	// ���b�V���̊e���_���E�H�[�N�X���[
	auto& vertex = pMesh->m_vertexData;
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		// ���W
		Vector3 pos;
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		vertex.positions.push_back(pos);
		// �@��
		if (mesh->mNormals) {
			Vector3 norm;
			norm.x = mesh->mNormals[i].x;
			norm.y = mesh->mNormals[i].y;
			norm.z = mesh->mNormals[i].z;
			vertex.normals.push_back(norm);
		}
		// �e�N�X�`�����W0
		if (mesh->mTextureCoords[0]) {
			Vector2 tex;
			tex.x = (float)mesh->mTextureCoords[0][i].x;
			tex.y = 1.0f - (float)mesh->mTextureCoords[0][i].y;
			vertex.texcoord0s.push_back(tex);
		}
		// �e�N�X�`�����W1
		if (mesh->mTextureCoords[1]) {
			Vector2 tex;
			tex.x = (float)mesh->mTextureCoords[1][i].x;
			tex.y = 1.0f - (float)mesh->mTextureCoords[1][i].y;
			vertex.texcoord1s.push_back(tex);
		}
		// �{�[���̃C���f�b�N�X�ƃE�F�C�g
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


	// �C���f�b�N�X�i�[
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

// �}�e���A���Ǎ�
void loadMaterial(aiScene* scene, aiMaterial* mat, RendererManager* renderer, Model::MaterialInfo& matInfo)
{
	aiColor4D color;
	HRESULT hr = S_OK;

	//// DIFFUSE COLOR
	//if (AI_SUCCESS != aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) {
	//	color.r = 1.0f;
	//	color.g = 1.0f;
	//	color.b = 1.0f;
	//	color.a = material.Kd.w;
	//}
	//material.Kd = XMFLOAT4(color.r, color.g, color.b, color.a);

	//// SPECULAR COLOR
	//if (AI_SUCCESS != aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) {
	//	color.r = 0.0f;
	//	color.g = 0.0f;
	//	color.b = 0.0f;
	//	color.a = 1.0f;
	//}
	//material.Ks = XMFLOAT4(color.r, color.g, color.b, material.Ks.w);

	//// AMBIENT COLOR
	//if (AI_SUCCESS != aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) {
	//	color.r = 0.0f;
	//	color.g = 0.0f;
	//	color.b = 0.0f;
	//	color.a = 0.0f;
	//}
	//material.Ka = XMFLOAT4(color.r, color.g, color.b, color.a);

	//// EMISSIVE COLOR
	//if (AI_SUCCESS != aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &color)) {
	//	color.r = 0.0f;
	//	color.g = 0.0f;
	//	color.b = 0.0f;
	//	color.a = 0.0f;
	//}
	//material.Ke = XMFLOAT4(color.r, color.g, color.b, color.a);

	//// Opacity
	//if (AI_SUCCESS != aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &material.Kd.w)) {
	//	//material.Kd.w = 1.0f;
	//}

	//// Shininess
	//if (AI_SUCCESS != aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &material.Ks.w)) {
	//	//material.Ks.w = 1.0f;
	//}

	aiString szPath;
	aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	bool bib = false;

	// DIFFUSE TEXTURE
	if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_DIFFUSE(0), &szPath)) {
		matInfo.DiffuseTex = LoadTexture(scene, szPath, renderer);

		aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_DIFFUSE(0), (int*)&mapU);
		aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_DIFFUSE(0), (int*)&mapV);
	}

	// NORMAL TEXTURE
	if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_NORMALS(0), &szPath)) {
		matInfo.NormalTex = LoadTexture(scene, szPath, renderer);
	}

	// SPECULAR TEXTURE
	if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_SPECULAR(0), &szPath)) {
		//matInfo.DiffuseTex = LoadTexture(scene, szPath, renderer);
	}

	// EMISSIVE TEXTURE
	if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_EMISSIVE(0), &szPath)) {
		//matInfo.DiffuseTex = LoadTexture(scene, szPath, renderer);
	}

	// OPACITY TEXTURE
	if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_OPACITY(0), &szPath)) {
		//matInfo.DiffuseTex = LoadTexture(scene, szPath, renderer);
	}
	else {
		int flags = 0;
		aiGetMaterialInteger(mat, AI_MATKEY_TEXFLAGS_DIFFUSE(0), &flags);

		//// try to find out whether the diffuse texture has any
		//// non-opaque pixels. If we find a few, use it as opacity texture
		//if (material.pTexture && !(flags & aiTextureFlags_IgnoreAlpha) && HasAlphaPixels(material.pTexture)) {
		//	int iVal;

		//	// NOTE: This special value is set by the tree view if the user
		//	// manually removes the alpha texture from the view ...
		//	if (AI_SUCCESS != aiGetMaterialInteger(mat, "no_a_from_d", 0, 0, &iVal)) {
		//		material.pTexTransparent = material.pTexture;
		//		material.pTexTransparent->AddRef();
		//	}
		//}
	}
}

// �e�N�X�`�� �C���f�b�N�X�擾
int getTextureIndex(aiString* str)
{
	std::string tistr;
	tistr = str->C_Str();
	tistr = tistr.substr(1);
	return stoi(tistr);
}

// �e�N�X�`�� �^�C�v����
std::string determineTextureType(aiScene* scene ,aiString& szPath)
{
	std::string textypeteststr = szPath.C_Str();
	if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2" || textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5") {
		if (scene->mTextures[0]->mHeight == 0) {
			return "embedded compressed texture";
		}
		else {
			return "embedded non-compressed texture";
		}
	}
	if (textypeteststr.find('.') != std::string::npos) {
		return "textures are on disk";
	}
	return "";
}

// ���f�� �f�[�^����e�N�X�`���Ǎ�
TextureID getTextureFromModel(aiScene* scene ,RendererManager* renderer, int textureindex)
{
	HRESULT hr = S_OK;
	std::string name = scene->mTextures[textureindex]->mFilename.C_Str();
	int* size = reinterpret_cast<int*>(&scene->mTextures[textureindex]->mWidth);
	return renderer->createTextureFromMemory(name, reinterpret_cast<unsigned char*>(scene->mTextures[textureindex]->pcData), *size);
}

// �e�N�X�`���Ǎ�
TextureID LoadTexture(aiScene* scene ,aiString& szPath, RendererManager* renderer)
{
	std::string filename = std::string(szPath.C_Str());

	if (determineTextureType(scene, szPath) == "embedded compressed texture") {
		int textureindex = getTextureIndex(&szPath);
		return getTextureFromModel(scene, renderer, textureindex);
	}
	else {
		// �p�X
		char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
		_splitpath_s(filename.c_str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);
		std::string directory = szDrive;
		directory += szDir;
		std::string fname = szFName;
		std::string ext = szExt;

		// �e�N�X�`���̓ǂݍ���
		filename = directory + filename;
		TextureID id = renderer->createTextureFromFile(filename.c_str());

		if (id == NONE_TEXTURE_ID) {
			char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
			_splitpath_s(szPath.C_Str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);
			filename = directory + szFName + szExt;
			id = renderer->createTextureFromFile(filename.c_str());

			if (id == NONE_TEXTURE_ID) {
				if (ext == ".fbx") {
					filename = directory + fname + ".fbm/" + szFName + szExt;
					id = renderer->createTextureFromFile(filename.c_str());
				}
			}
		}
		if (id == NONE_TEXTURE_ID) {
			MessageBoxA(NULL, "Texture couldn't be loaded", "Error!", MB_ICONERROR | MB_OK);
		}

		return id;
	}
}