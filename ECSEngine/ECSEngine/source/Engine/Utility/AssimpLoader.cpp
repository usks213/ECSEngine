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
#include <assimp/matrix4x4.h>

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc141-mtd")
#pragma comment(lib, "IrrXMLd")
#pragma comment(lib, "zlibstaticd")
#else
#pragma comment(lib, "assimp-vc141-mt")
#pragma comment(lib, "IrrXML")
#pragma comment(lib, "zlibstatic")
#endif

#define aiProcessPreset_TargetRealtime_Quality ( \
    aiProcess_CalcTangentSpace              |  \
    aiProcess_GenSmoothNormals              |  \
    aiProcess_JoinIdenticalVertices         |  \
    aiProcess_ImproveCacheLocality          |  \
    aiProcess_LimitBoneWeights              |  \
    aiProcess_RemoveRedundantMaterials      |  \
    aiProcess_SplitLargeMeshes              |  \
    aiProcess_Triangulate                   |  \
    aiProcess_GenUVCoords                   |  \
    aiProcess_SortByPType                   |  \
    aiProcess_FindDegenerates               |  \
    aiProcess_FindInvalidData               |  \
    0 )

#define aiProcessPreset_TargetRealtime_MaxQuality ( \
    aiProcessPreset_TargetRealtime_Quality   |  \
    aiProcess_FindInstances                  |  \
    aiProcess_ValidateDataStructure          |  \
    aiProcess_OptimizeMeshes                 |  \
    0 )

#define aiProcess_ConvertToLeftHanded ( \
    aiProcess_MakeLeftHanded     | \
    aiProcess_FlipUVs            | \
    aiProcess_FlipWindingOrder   | \
    0 )


namespace
{
	Matrix aiMatrix4x4ToMatrix(aiMatrix4x4 aiMe)
	{
		aiMe.Transpose();
		return Matrix(aiMe.a1, aiMe.a2, aiMe.a3, aiMe.a4,
			aiMe.b1, aiMe.b2, aiMe.b3, aiMe.b4,
			aiMe.c1, aiMe.c2, aiMe.c3, aiMe.c4,
			aiMe.d1, aiMe.d2, aiMe.d3, aiMe.d4);
	}
};



//===== �v���g�^�C�v�錾 =====
// �m�[�h�ǂݍ��݁E�쐬
void LoadNode(aiScene* scene, aiNode* node, Model& model, Model::NodeInfo& self);
// ���b�V���̓ǂݍ���
void LoadMesh(aiMesh* mesh, RendererManager* renderer, Model& model, Model::MeshInfo& meshInfo);
// �A�j���[�V�����̓ǂݍ���
void loadAnimation(aiAnimation* pAnime, RendererManager* renderer, Model::AnimationInfo& animInfo);
// �}�e���A���̓ǂݍ���
void loadMaterial(aiScene* scene, aiMaterial* mat, RendererManager* renderer, Model::MaterialInfo& matInfo);
// �e�N�X�`���̓ǂݍ���
TextureID LoadTexture(aiScene* scene, aiString& szPath, RendererManager* renderer);
// ���[�g�{�[������
bool FindRootBone(Model& model, Model::NodeInfo nodeInfo, Model::MeshInfo& meshInfo);
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
	aiSetImportPropertyInteger(props, AI_CONFIG_GLOB_MEASURE_TIME, 1);
	aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
	aiSetImportPropertyInteger(props, AI_CONFIG_PP_SBP_REMOVE, 0);
	aiSetImportPropertyFloat(props, AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 60.0f);
	aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
	auto scene = (aiScene*)aiImportFileExWithProperties(path.c_str(),
		aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded
		//ppsteps |
		//aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
		//aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
		//aiProcess_Triangulate | // triangulate polygons with more than 3 edges
		//aiProcess_MakeLeftHanded | // convert everything to D3D left handed space
		//aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
		, NULL, props);
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

	// ���f����
	out.m_name = fname;

	// �m�[�h�̍쐬
	LoadNode(scene, scene->mRootNode, out, out.m_rootNode);
	out.m_rootNode.name = fname;

	// ���b�V���̓ǂݍ���
	if (scene->HasMeshes())
	{
		out.m_meshList.resize(scene->mNumMeshes);
		for (std::uint32_t i = 0; i < scene->mNumMeshes; ++i)
		{
			LoadMesh(scene->mMeshes[i], renderer, out, out.m_meshList[i]);
		}
	}

	// �A�j���[�V�����̓ǂݍ���
	if (scene->HasAnimations())
	{
		out.m_animationList.resize(scene->mNumAnimations);
		for (std::uint32_t i = 0; i < scene->mNumAnimations; ++i)
		{
			loadAnimation(scene->mAnimations[i], renderer, out.m_animationList[i]);
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

	// �A�o�^�[�\�z
	auto avatar = std::make_unique<Model::AvatarInfo>();
	avatar->name = out.m_name;
	for (auto& mesh : out.m_meshList)
	{
		for (auto& bone : mesh.boneTable)
		{
			auto itr = avatar->boneTable.find(bone.first);
			if (avatar->boneTable.end() != itr) continue;
			// �A�o�^�[�Ƀ{�[���̒ǉ�
			avatar->boneTable[bone.first] = bone.second;
		}
	}
	out.m_avatar = std::move(avatar);


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
	self.transform = aiMatrix4x4ToMatrix(node->mTransformation);

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
void LoadMesh(aiMesh* mesh, RendererManager* renderer, Model& model, Model::MeshInfo& meshInfo)
{
	// ���b�V���̐���
	auto meshID = renderer->createMesh(mesh->mName.C_Str());
	auto pMesh = renderer->getMesh(meshID);
	pMesh->m_topology = PrimitiveTopology::TRIANGLE_LIST;

	// ���b�V�����ǉ�
	meshInfo.meshID = meshID;
	meshInfo.materialIndex = mesh->mMaterialIndex;

	// �S���_�̃E�F�C�g�����W(�������̂��߃`�F�b�N��)
	std::vector<std::vector<aiVertexWeight> >weightsPerVertex;
	if (mesh->HasBones())
	{
		// �{�[�����i�[
		pMesh->m_boneCount = mesh->mNumBones;
		pMesh->m_calcBoneBuffer.resize(pMesh->m_boneCount);
		pMesh->m_invMatrixData.resize(pMesh->m_boneCount);
		weightsPerVertex.resize(mesh->mNumVertices);
		for (unsigned int a = 0; a < mesh->mNumBones; a++)
		{
			const aiBone* bone = mesh->mBones[a];
			Matrix matrix = aiMatrix4x4ToMatrix(bone->mOffsetMatrix);
			meshInfo.boneTable[bone->mName.C_Str()] = a;
				
			pMesh->m_invMatrixData[a] = matrix;
			pMesh->m_boneTabel[bone->mName.C_Str()] = a;

			for (unsigned int b = 0; b < bone->mNumWeights; b++)
				weightsPerVertex[bone->mWeights[b].mVertexId].push_back(aiVertexWeight(a, bone->mWeights[b].mWeight));
		}
		// �K�w�f�[�^����ŏ�ʊK�w(rootBone)�̃{�[������T��
		FindRootBone(model, model.m_rootNode, meshInfo);
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
			tex.y = (float)mesh->mTextureCoords[0][i].y;
			vertex.texcoord0s.push_back(tex);
		}
		// �e�N�X�`�����W1
		if (mesh->mTextureCoords[1]) {
			Vector2 tex;
			tex.x = (float)mesh->mTextureCoords[1][i].x;
			tex.y = (float)mesh->mTextureCoords[1][i].y;
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

// �A�j���[�V�����̓ǂݍ���
void loadAnimation(aiAnimation* pAnime, RendererManager* renderer, Model::AnimationInfo& animInfo)
{
	// �A�j���[�V�����쐬
	animInfo.name = pAnime->mName.C_Str();
	animInfo.animationID = renderer->createAnimation(animInfo.name);
	auto* anime = renderer->getAnimation(animInfo.animationID);

	anime->m_duration = pAnime->mDuration;
	anime->m_ticksPerSecond = pAnime->mTicksPerSecond;

	// �L�[�t���[���i�[
	anime->m_numChannels = pAnime->mNumChannels;
	anime->m_channelList.resize(anime->m_numChannels);
	anime->m_channelTable.reserve(anime->m_numChannels);
	for(int i = 0; i < anime->m_numChannels; ++i)
	{
		auto* pChannel = pAnime->mChannels[i];
		auto& channel = anime->m_channelList[i];
		// �Ή�����m�[�h��
		channel.boneName = pChannel->mNodeName.C_Str();
		anime->m_channelTable[channel.boneName] = i;
		// �ʒu
		channel.numPosKeys = pChannel->mNumPositionKeys;
		channel.posKeys.resize(channel.numPosKeys);
		std::memcpy(channel.posKeys.data(), pChannel->mPositionKeys, 
			sizeof(aiVectorKey) * channel.numPosKeys);
		// ��]
		channel.numRotKeys = pChannel->mNumRotationKeys;
		channel.rotKeys.resize(channel.numRotKeys);
		for (int j = 0; j < channel.numRotKeys; ++j)
		{
			channel.rotKeys[j].value.x = pChannel->mRotationKeys[j].mValue.w;
			channel.rotKeys[j].value.y = pChannel->mRotationKeys[j].mValue.z;
			channel.rotKeys[j].value.z = pChannel->mRotationKeys[j].mValue.y;
			channel.rotKeys[j].value.w = pChannel->mRotationKeys[j].mValue.x;
			channel.rotKeys[j].time = pChannel->mRotationKeys[j].mTime;
		}
		//std::memcpy(channel.rotKeys.data(), pChannel->mRotationKeys,
		//	sizeof(aiQuatKey) * channel.numRotKeys);
		// �g�k
		channel.numScaKeys = pChannel->mNumScalingKeys;
		channel.scaKeys.resize(channel.numScaKeys);
		std::memcpy(channel.scaKeys.data(), pChannel->mScalingKeys,
			sizeof(aiVectorKey) * channel.numScaKeys);
	}
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

// ���[�g�{�[������
bool FindRootBone(Model& model, Model::NodeInfo nodeInfo, Model::MeshInfo& meshInfo)
{
	auto itr = meshInfo.boneTable.find(nodeInfo.name);
	if (meshInfo.boneTable.end() != itr)
	{
		// ��������
		meshInfo.rootBoneName = nodeInfo.name;
		return true;
	}
	// �q�m�[�h�T��
	for (auto& childID : nodeInfo.childs)
	{
		if (FindRootBone(model, model.m_nodeMap[childID], meshInfo))
		{
			return true;
		}
	}

	return false;
}
