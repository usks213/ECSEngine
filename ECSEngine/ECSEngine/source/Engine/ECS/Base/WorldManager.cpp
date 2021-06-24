/*****************************************************************//**
 * \file   WorldManager.cpp
 * \brief  ���[���h�}�l�[�W���[�̃x�[�X�N���X
 *
 * \author USAMI KOSHI
 * \date   2021/06/14
 *********************************************************************/

#include "WorldManager.h"
#include <Engine/Engine.h>

#include <Engine/Renderer/D3D11/D3D11RendererManager.h>
#include <Engine/Renderer/D3D11/D3D11Shader.h>
#include <Engine/Renderer/Base/Vertex.h>
#include <Engine/Utility/HashUtil.h>
#include <string>

using namespace DirectX;
struct VERTEX_3D 
{
	Vector3 vtx;		// ���_���W
	Vector3 nor;		// �@���x�N�g��
	Vector4 diffuse;	// �g�U���ˌ�
	Vector2 tex;		// �e�N�X�`�����W
} ;

void WorldManager::initialize()
{
	auto* renderer = static_cast<D3D11RendererManager*>(m_pEngine->getRendererManager());
	auto* device = renderer->m_d3dDevice.Get();

	// �V�F�[�_�ǂݍ���
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Test";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;
	D3D11Shader shader(device, shaderDesc);
	
	// ���_���W�̐ݒ�
	VERTEX_3D pVtx[4];
	// ���_���W�̐ݒ�
	pVtx[0].vtx = Vector3(-1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[1].vtx = Vector3(-1.0f / 2, 1.0f / 2, 0.0f);
	pVtx[2].vtx = Vector3(1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[3].vtx = Vector3(1.0f / 2, 1.0f / 2, 0.0f);
	// �@���̐ݒ�
	pVtx[0].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = Vector3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = Vector3(0.0f, 0.0f, -1.0f);
	// ���ˌ��̐ݒ�
	pVtx[0].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = Vector2(0.0f, 1.0f);
	pVtx[1].tex = Vector2(0.0f, 0.0f);
	pVtx[2].tex = Vector2(1.0f, 1.0f);
	pVtx[3].tex = Vector2(1.0f, 0.0f);

	// ���_��񐶐�
	VertexData verData(shader, 8);
	for (int i = 0; i < 4; ++i)
	{
		verData.setPosition(pVtx[i].vtx, i);
		verData.setNormal(pVtx[i].nor, i);
		verData.setColor(pVtx[i].diffuse, i);
		verData.setTexCoord(pVtx[i].tex, 0, i);
	}

	D3D11_BUFFER_DESC vertex;

	//renderer->m_d3dDevice->CreateBuffer();
}

void WorldManager::finalize()
{

}

void WorldManager::fixedUpdate()
{

}

void WorldManager::update()
{

}

void WorldManager::render()
{

}


