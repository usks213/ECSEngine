/*****************************************************************//**
 * \file   FBXParser.h
 * \brief  FBX���f�����[�h
 *
 * \author USAMI KOSHI
 * \date   2021/06/09
 *********************************************************************/

#include "FBXParser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory>

/// @brief �R���X�g���N�^
FBXParser::FBXParser()
	: m_nIdxNum(0), m_nVtxNum(0),m_nNrmNum(0),m_nUVNum(0)
{
}

/// @brief �f�X�g���N�^
FBXParser::~FBXParser()
{
	Release();
}

void FBXParser::Release()
{
	m_nVtxNum = 0;
	m_nIdxNum = 0;
	m_nNrmNum = 0;
	m_nUVNum  = 0;
	std::vector<float>	tempV;
	std::vector<int>	tempI;
	std::vector<float>	tempN;
	std::vector<float>	tempU;
	std::vector<int>	tempUI;
	std::vector<std::string> tempT;
	m_vtxList.swap(tempV);
	m_idxList.swap(tempI);
	m_nrmList.swap(tempN);
	m_uvList.swap(tempU);
	m_uvIdxList.swap(tempUI);
	m_texList.swap(tempT);
}

/// @brief FBX���[�h�֐�
/// @param filename �p�X
/// @return ���� true
bool FBXParser::Load(const char* filename)
{
	// ���������
	Release();

	/* �o�C�i���`���ƃe�L�X�g�`���̃f�[�^�̈Ⴂ
	�Ⴆ�Ή~����(3.141592)��ۑ�����ꍇ
	�o�C�i���ł� float(4byte)
	�e�L�X�g�ł� "3.141592"(char[8] 8byte)�ŕۑ������
	*/
	printf("%s:�t�@�C���̓ǂݍ���\n", filename);
	FILE* fp;
	fopen_s(&fp, filename, "rt");
	if (fp == NULL)
	{
		printf("%s:�t�@�C���I�[�v�����s\n", filename);
		return false;
	}

	// �f�[�^���ꊇ�œǂݍ���(���蕶���܂Ői�߂邽��)
	// (�I�����[�h/onload) or (�X�g���[�~���O/streaming)

	// �t�@�C���̃f�[�^�T�C�Y�𒲂ׂ�
	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	// �������m��
	char* pData = new char[fileSize];
	fread(pData, fileSize, 1, fp);
	fclose(fp);

	char* ptr = pData;
	// ������uGeometry�v�܂œǂݐi�߂�
	ptr = FindKeyword(ptr, "Geometry:");
	//--- ���_�ǂݍ���
	ptr = FindKeyword(ptr, "Vertices: *");
	// �����񂩂琔���ɕϊ�
	m_nVtxNum = atoi(ptr);
	if (m_nVtxNum > 0)
	{
		// ���_�f�[�^�ǂݎ��
		char* ptrVtx = ptr;
		m_vtxList.reserve(m_nVtxNum);
		ptrVtx = FindKeyword(ptrVtx, "a: ");
		for (auto i = 0; i < m_nVtxNum; ++i)
		{
			m_vtxList.push_back(atof(ptrVtx));
			ptrVtx = FindKeyword(ptrVtx, ",");
		}
		m_nVtxNum /= 3;
	}
	//--- �C���f�b�N�X�ǂݍ���
	ptr = FindKeyword(ptr, "PolygonVertexIndex: *");
	int idxNum = atoi(ptr);
	std::vector<int> tempIdx;
	if (idxNum > 0)
	{
		char* ptrIdx = ptr;
		// �C���f�b�N�X�̓ǂݍ���
		
		tempIdx.reserve(idxNum);
		ptrIdx = FindKeyword(ptrIdx, "a: ");
		for (int i = 0; i < idxNum; ++i)
		{
			tempIdx.push_back(atoi(ptrIdx));
			ptrIdx = FindKeyword(ptrIdx, ",");
		}
		// FBX�t�@�C���͑��p�`�̖ʃf�[�^���܂܂��
		// �S�Ă̖ʂ��O�p�`�ɕ���
		// �@������̃C���f�b�N�X�����v�Z
		int splitIdxNum = 0;
		// �ʂ���؂�C���f�b�N�X�̓}�C�i�X
		int minusIdxPoint = 0;
		for(int i = 0; i < idxNum; ++i)
		{
			++minusIdxPoint;
			if (tempIdx[i] < 0)
			{
				// ���p�`�̒��_�� - 2��
				// ������̎O�p�`�̐�
				splitIdxNum += minusIdxPoint - 2;
				minusIdxPoint = 0;
			}
		}
		printf("������̖ʐ�:%d\n", splitIdxNum);
		// �A���������ʂ̐�����C���f�b�N�X�f�[�^���m��
		m_nIdxNum = splitIdxNum * 3;
		m_idxList.resize(m_nIdxNum);
		// �B���̃C���f�b�N�X�f�[�^�𕪊����Ȃ���i�[
		int storageIdx = 0;
		minusIdxPoint = 0;
		for (int i = 0; i < idxNum; ++i)
		{
			++minusIdxPoint;
			if (minusIdxPoint <= 3)
			{
				// �O�p�`�͂��̂܂܊i�[
				m_idxList[storageIdx] = tempIdx[i];
				storageIdx++;
			}
			else
			{
				// �l�p�`�ȏ�͊i�[�ς݂̃C���f�b�N�X
				// ���R�s�[���Ȃ���i�[
				m_idxList[storageIdx] = m_idxList[storageIdx - 3];
				storageIdx++;
				m_idxList[storageIdx] = m_idxList[storageIdx - 2];
				storageIdx++;
				m_idxList[storageIdx] = tempIdx[i];
				storageIdx++;
			}

			if (tempIdx[i] < 0)
			{
				// �ʂ̋�؂�
				minusIdxPoint = 0;
				// �}�C�i�X�̃C���f�b�N�X���r�b�g���]
				m_idxList[storageIdx - 1] = ~m_idxList[storageIdx - 1];
			}
		}

	}

	//--- �@���̓ǂݍ���
	ptr = FindKeyword(ptr, "LayerElementNormal:");
	if (ptr != NULL)
	{
		char* nrmPtr = ptr;
		// MappingInformationType��"ByPolygonVertex"�̂ݑΉ�
		// ByPolygonVertex...�ʂɑΉ������@��
		// ByControlPoint...���_�ɑΉ������@��
		nrmPtr = FindKeyword(nrmPtr, "MappingInformationType: \"");
		if (strstr(nrmPtr, "ByPolygonVertex") == nrmPtr)
		{
			// �@�f�[�^����ǂݍ���
			nrmPtr = FindKeyword(nrmPtr, "Normals: *");
			int nrmNum = atoi(nrmPtr);

			// �A�@���̃f�[�^��ǂݍ���
			std::vector<float> tempNrm;
			tempNrm.reserve(nrmNum);
			nrmPtr = FindKeyword(nrmPtr, "a: ");
			for (int i = 0; i < nrmNum; ++i)
			{
				tempNrm.push_back(atof(nrmPtr));
				nrmPtr = FindKeyword(nrmPtr, ",");
			}

			// �B�C���f�b�N�X�f�[�^�����ɕ���
			// ������̖@����	xyz��
			m_nNrmNum = m_nIdxNum * 3;
			m_nrmList.resize(m_nNrmNum);

			// �C������̃f�[�^�������o�ϐ��Ɋi�[
			int storageIdx = 0;
			int minusIdxPoint = 0;
			for (int i = 0; i < idxNum; ++i)
			{
				++minusIdxPoint;
				if (minusIdxPoint <= 3)
				{
					// �O�p�`�͂��̂܂܊i�[
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 0];
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 1];
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 2];
				}
				else
				{
					// �l�p�`�ȏ�͊i�[�ς݂̖@��
					// ���R�s�[���Ȃ���i�[
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 9];
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 9];
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 9];

					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 6];
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 6];
					m_nrmList[storageIdx++] = m_nrmList[storageIdx - 6];

					m_nrmList[storageIdx++] = tempNrm[i * 3 + 0];
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 1];
					m_nrmList[storageIdx++] = tempNrm[i * 3 + 2];
				}

				if (tempIdx[i] < 0)
				{
					// �ʂ̋�؂�
					minusIdxPoint = 0;
					// �}�C�i�X�̃C���f�b�N�X���r�b�g���]
				}
			}
		}
	}

	// UV�f�[�^�ǂݍ���
	ptr = FindKeyword(ptr, "LayerElementUV");
	if (ptr != nullptr)
	{
		char* pUV = ptr;
		// UV�̃f�[�^�ݒ���@���m�F
		pUV = FindKeyword(pUV, "MappingInformationType: \"");
		if (strstr(pUV, "ByPolygonVertex") == pUV)
		{
			pUV = FindKeyword(pUV, "ReferenceInformationType: \"");
			if (strstr(pUV, "IndexToDirect") == pUV)
			{
				// UV�f�[�^�ǂݍ���
				char* pUVPtrRecord = pUV;
				pUV = FindKeyword(pUV, "UV: *");
				m_nUVNum = atoi(pUV);
				m_uvList.resize(m_nUVNum);
				pUV = FindKeyword(pUV, "a: ");
				for (int i = 0; i < m_nUVNum; ++i)
				{
					m_uvList[i] = atof(pUV);
					pUV = FindKeyword(pUV, ",");
				}
				// UV�Q�ƃf�[�^�ǂݍ���
				pUV = pUVPtrRecord;
				pUV = FindKeyword(pUV, "UVIndex: *");
				int nUVIdx = atoi(pUV);
				std::vector<int> tempUVIdx(nUVIdx);
				pUV = FindKeyword(pUV, "a: ");
				for (int i = 0; i < nUVIdx; ++i)
				{
					tempUVIdx[i] = atoi(pUV);
					pUV = FindKeyword(pUV, ",");
				}

				// UV�Q�ƃf�[�^�𕪊�
				// �C���f�b�N�X�f�[�^�����ɕ���
				m_uvIdxList.resize(m_nIdxNum);

				// ������̃f�[�^�������o�ϐ��Ɋi�[
				int storageIdx = 0;
				int minusIdxPoint = 0;
				for (int i = 0; i < idxNum; ++i)
				{
					++minusIdxPoint;
					if (minusIdxPoint <= 3)
					{
						// �O�p�`�͂��̂܂܊i�[
						m_uvIdxList[storageIdx++] = tempUVIdx[i];
					}
					else
					{
						// �l�p�`�ȏ�͊i�[�ς݂̖@��
						// ���R�s�[���Ȃ���i�[
						m_uvIdxList[storageIdx++] = m_uvIdxList[storageIdx - 3];

						m_uvIdxList[storageIdx++] = m_uvIdxList[storageIdx - 2];

						m_uvIdxList[storageIdx++] = tempUVIdx[i];
					}

					if (tempIdx[i] < 0)
					{
						// �ʂ̋�؂�
						minusIdxPoint = 0;
					}
				}
			}
		}
	}

	// �e�N�X�`���̓ǂݍ���
	if (ptr != nullptr)
	{
		ptr = FindKeyword(ptr, "Texture:");
		if (ptr != nullptr)
		{
			char* pTex = ptr;
			pTex = FindKeyword(pTex, "FileName: \"");
			// ���������邩�v�Z
			char* pEnd = FindKeyword(pTex, "\"") - 1;
			int textLen = pEnd - pTex;
			// �������m��
			std::unique_ptr<char[]> pData = std::make_unique<char[]>(textLen + 1);
			memcpy(pData.get(), pTex, textLen);
			pData[textLen] = '\0';
			// �i�[
			m_texList.push_back(pData.get());

		}
	}


	delete[] pData;
	return true;
}

char* FBXParser::FindKeyword(char* ptr, const char* key)
{
	// strstr �����񒆂���w�肳�ꂽ�����Ɉ�v���镶���̃A�h���X��
	// �Ԃ��B�Ȃ����NULL
	ptr = strstr(ptr, key);
	if (NULL != ptr)
	{
		ptr += strlen(key);
	}

	return ptr;
}
