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
#include <Engine/Renderer/D3D11/D3D11Material.h>
#include <Engine/Renderer/D3D11/D3D11RenderBuffer.h>
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

struct VERTEX_3D_TEST
{
	Vector3 vtx;		// ���_���W
	Vector2 tex;		// �e�N�X�`�����W
	Vector4 diffuse;	// �g�U���ˌ�
};

struct CBuffer
{
	XMMATRIX g_mWorld;
	XMMATRIX g_mView;
	XMMATRIX g_mProjection;
	XMMATRIX g_mTexture;
};

struct RenderData
{
	MaterialID		matID;
	RenderBufferID	rdID;
};


RenderData g_rd;


void WorldManager::initialize()
{
	auto* renderer = static_cast<D3D11RendererManager*>(m_pEngine->getRendererManager());
	auto* device = renderer->m_d3dDevice.Get();
	auto* context = renderer->m_d3dContext.Get();

	// �V�F�[�_�ǂݍ���
	ShaderDesc shaderDesc;
	shaderDesc.m_name = "Test2D";
	shaderDesc.m_stages = EShaderStageFlags::VS | EShaderStageFlags::PS;
	ShaderID shaderID = renderer->createShader(shaderDesc);
	
	// �}�e���A���̍쐬
	g_rd.matID = renderer->createMaterial("TestMat", shaderID);

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

	// ���b�V���̐���
	MeshID meshID = renderer->createMesh("TestMesh");
	auto* pMesh = renderer->getMesh(meshID);

	pMesh->m_vertexCount = 4;
	for (int i = 0; i < 4; ++i)
	{
		pMesh->m_vertexData.positions.push_back(pVtx[i].vtx);
		pMesh->m_vertexData.normals.push_back(pVtx[i].nor);
		pMesh->m_vertexData.texcoord0s.push_back(pVtx[i].tex);
		pMesh->m_vertexData.colors.push_back(pVtx[i].diffuse);
	}

	// �����_�[�o�b�t�@�̐���
	g_rd.rdID = renderer->createRenderBuffer(shaderID, meshID);

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
	auto* renderer = static_cast<D3D11RendererManager*>(m_pEngine->getRendererManager());
	auto* context = renderer->m_d3dContext.Get();

	// �f�[�^�̎擾
	auto* material = static_cast<D3D11Material*>(renderer->getMaterial(g_rd.matID));
	auto* shader = static_cast<D3D11Shader*>(renderer->getShader(material->m_shaderID));
	auto* renderBuffer = static_cast<D3D11RenderBuffer*>(renderer->getRenderBuffer(g_rd.rdID));


	// �v���~�e�B�u�w��
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// �u�����h�X�e�[�g
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(renderer->m_blendStates[(size_t)EBlendState::ALPHA].Get(), 
		blendFactor, 0xffffffff);
	// ���X�^���C�U�X�e�[�g
	context->RSSetState(renderer->m_rasterizeStates[(size_t)ERasterizeState::CULL_NONE].Get());
	// �[�x�X�e���V���X�e�[�g
	context->OMSetDepthStencilState(renderer->m_depthStencilStates[
		(size_t)EDepthStencilState::DISABLE_TEST_AND_DISABLE_WRITE].Get(), 0);

	// �V�F�[�_�[
	context->VSSetShader(shader->vs, nullptr, 0);
	context->PSSetShader(shader->ps, nullptr, 0);
	context->IASetInputLayout(shader->m_inputLayout.Get());

	// ���_�o�b�t�@���Z�b�g
	UINT stride = static_cast<UINT>(renderBuffer->m_vertexData.size);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, renderBuffer->m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	if (renderBuffer->m_indexData.count > 0) {
		context->IASetIndexBuffer(renderBuffer->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	// �R���X�^���g�o�b�t�@
	// �g�k
	XMMATRIX mWorld = XMMatrixScaling(1,1,1);
	// ��]
	static float rad;
	rad += 1.0f;
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(0),
		XMConvertToRadians(rad), XMConvertToRadians(0));
	// �ړ�
	mWorld *= XMMatrixTranslation(0,0,0);

	XMMATRIX mtxView, mtxProj, mtxTex;
	mtxView = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	float asoect = (float)m_pEngine->getWindowWidth() / m_pEngine->getWindowHeight();
	mtxProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45), asoect, 1.0f, 1000.0f);

	mtxTex = XMMatrixIdentity();
	CBuffer cb;
	cb.g_mWorld = XMMatrixTranspose(mWorld);
	cb.g_mView = XMMatrixTranspose(mtxView);
	cb.g_mProjection = XMMatrixTranspose(mtxProj);
	cb.g_mTexture = XMMatrixTranspose(mtxTex);

	// �R���X�^���g�o�b�t�@�X�V
	context->UpdateSubresource(material->m_d3dCbuffer[0][0].Get(), 0, nullptr,&cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, material->m_d3dCbuffer[0][0].GetAddressOf());

	// �|���S���̕`��
	if (renderBuffer->m_indexData.count > 0)
	{
		context->DrawIndexed(renderBuffer->m_indexData.count, 0, 0);
	}
	else
	{
		context->Draw(renderBuffer->m_vertexData.count, 0);
	}
}


