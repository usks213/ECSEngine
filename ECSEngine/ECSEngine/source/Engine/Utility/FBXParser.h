/*****************************************************************//**
 * \file   FBXParser.h
 * \brief  FBX���f�����[�h
 * 
 * \author USAMI KOSHI
 * \date   2021/06/09
 *********************************************************************/
#pragma once

#include <vector>
#include <string>

/**
 * FBX�t�@�C���Ƃ�
 * Autodesk�Ђ��J�������t�@�C���t�H�[�}�b�g
 * 3D���f���̃f�[�^���܂܂�Ă͂��邪�A
 * �ʁX�̃\�t�g�ň�����悤�ɂ��邽��
 * 
 * �Z�R�[�����f��
 * ���_���W
 * �@��
 * UV���W
 * ���_�� 8
 * 
 * �꒸�_������̃f�[�^��
 * ���_���W	- float * 3
 * �@��		- float * 3
 * UV���W	- float * 2
 *  �� 32byte * 8���_ = 256byte
 * 
 * FBX�t�@�C���̃f�[�^�T�C�Y �� 15,871byte
 * 60�{�̃f�[�^�T�C�Y�I�I
 * 
 * �K�v�ȃf�[�^���i�[����Ă���ʒu��T���o���A
 * �K�v�ȃT�C�Y�������m�ۂ��Ȃ���ǂݍ���
 * �� ���(Parser:�p�[�T�[)
 */

/// @brief 
class FBXParser
{
public:
	FBXParser();
	~FBXParser();

	bool Load(const char* filename);
	void Release();

	// ���_
	float* getVertexList() { return m_vtxList.data(); }
	std::uint32_t getVertexNum() { return m_nVtxNum; }

	// �C���f�b�N�X
	int* getIndexList() { return m_idxList.data(); }
	std::uint32_t getIndexNum() { return m_nIdxNum; }

	// �@��
	float* getNormalList() { return m_nrmList.data(); }
	std::uint32_t getNormalNum() { return m_nNrmNum; }

	// UV
	float* getUVList() { return m_uvList.data(); }
	int getUVNum() { return m_nUVNum; }
	int* getUVIndexList() { return m_uvIdxList.data(); }

	// �e�N�X�`��
	std::string* getTextureList() { return m_texList.data(); }
	std::uint32_t getTextureNum() { return m_texList.size(); }

private:

	/// @brief ����̕��������������
	/// @param ptr �T���J�n�ʒu
	/// @param key �T��������
	/// @return ���������A�h���X
	[[nodiscard]] char* FindKeyword(char* ptr, const char* key);

private:
	/// @brief ���_��
	std::uint32_t m_nVtxNum;
	/// @brief ���_���X�g
	std::vector<float> m_vtxList;
	/// @brief �C���f�b�N�X��
	std::uint32_t m_nIdxNum;
	/// @brief �C���f�b�N�X�f�[�^
	std::vector<int> m_idxList;
	/// @brief �@����
	std::uint32_t m_nNrmNum;
	/// @brief �@���f�[�^
	std::vector<float> m_nrmList;
	/// @brief UV��
	std::uint32_t		m_nUVNum;
	/// @brief UV�f�[�^
	std::vector<float>	m_uvList;
	/// @brief UV�C���f�b�N�X�f�[�^
	std::vector<int>	m_uvIdxList;
	/// @brief
	//std::uint32_t		m_nTexNum;
	/// @brief �e�N�X�`���f�[�^
	std::vector<std::string> m_texList;
};

