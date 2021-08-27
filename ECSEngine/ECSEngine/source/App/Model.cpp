/*****************************************************************//**
 * \file   Model.h
 * \brief  ���f��
 *
 * \author USAMI KOSHI
 * \date   2021/07/07
 *********************************************************************/

#include "Model.h"
#include <Engine/Engine.h>
#include <Engine/Utility/FBXParser.h>


bool Model::LoadFBXModel(const char* fileName, FBXModelData& out)
{
	bool result = true;
	FBXParser fbx;
	result = fbx.Load(fileName);
	if (!result) return result;

	// ���b�V���̍쐬
	auto* renderer = Engine::get().getRendererManager();
	auto meshID = renderer->createMesh(fileName);
	auto* pMesh = renderer->getMesh(meshID);
	pMesh->m_topology = PrimitiveTopology::TRIANGLE_LIST;
	out.meshID = meshID;

	//----- ���_�쐬
	// �@����UV�̃f�[�^�̓C���f�b�N�X�Ɍ��т��Ă���
	// ���̃f�[�^�𒸓_�̃f�[�^�Ɍ��т���̂�
	// ����̂ŁA�t�ɒ��_�̃f�[�^�ɍ��킹��
	int nVtxNum = fbx.getIndexNum();
	int* pIdx = fbx.getIndexList();
	float* pPos = fbx.getVertexList();
	float* pNrm = fbx.getNormalList();
	float* pUV = fbx.getUVList();
	int* pUVIdx = fbx.getUVIndexList();

	pMesh->m_vertexCount = nVtxNum;
	pMesh->m_vertexData.positions.resize(nVtxNum);
	pMesh->m_vertexData.normals.resize(nVtxNum);
	pMesh->m_vertexData.colors.resize(nVtxNum);
	pMesh->m_vertexData.texcoord0s.resize(nVtxNum);

	for (int i = 0; i < nVtxNum; ++i)
	{
		// ���W
		int idx = pIdx[i] * 3;
		if (pPos)
		{
			Vector3 pos;
			pos.x = pPos[idx + 0];
			pos.y = pPos[idx + 1];
			pos.z = pPos[idx + 2];
			pMesh->m_vertexData.positions[nVtxNum - 1 - i] = pos;
		}
		// �@��
		if (pNrm)
		{
			Vector3 nrm;
			nrm.x = pNrm[idx + 0];
			nrm.y = pNrm[idx + 1];
			nrm.z = pNrm[idx + 2];
			pMesh->m_vertexData.normals[nVtxNum - 1 - i] = nrm;
		}
		// �J���[
		Vector4 col(1,1,1,1);
		pMesh->m_vertexData.colors[nVtxNum - 1 - i] = col;
		// UV
		if (pUV && pUVIdx)
		{
			idx = pUVIdx[i] * 2;
			Vector2 uv;
			uv.x = pUV[idx + 0];
			uv.y = 1.0f - pUV[idx + 1];
			pMesh->m_vertexData.texcoord0s[nVtxNum - 1 - i] = uv;
		}
	}

	// �C���f�b�N�X�Ȃ�
	pMesh->m_indexCount = 0;

	// �e�N�X�`���̓ǂݍ���
	if (fbx.getTextureNum() > 0)
	{
		char localPath[MAX_PATH] = {};
		const char* fbxPath = fbx.getTextureList()[0].c_str();
		// �t�@�C���l�[������p�X���擾
		int pathEnd = 0;
		int fileLen = strlen(fileName);
		for (int i = fileLen - 1; i >= 0; --i)
		{
			if (fileName[i] == '/')
			{
				pathEnd = i + 1;
				break;
			}
		}
		memcpy(localPath, fileName, pathEnd);
		// fbx�̃p�X����t�@�C�����𔲂��o��
		int fbxPathLen = strlen(fbxPath);
		int fbxPathEnd = 0;
		for(int i = fbxPathLen - 1; i >= 0; --i)
		{ 
			if (fbxPath[i] == '/' || fbxPath[i] == '\\')
			{
				fbxPathEnd = i + 1;
				break;
			}
		}
		memcpy(localPath + pathEnd, fbxPath + fbxPathEnd, (fbxPathLen - fbxPathEnd) + 1);


		out.textureID = renderer->createTextureFromFile(localPath);
	}

	fbx.Release();
	return result;
}

