/*****************************************************************//**
 * \file   DevelopSystem.h
 * \brief  �J���p�V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/09/01
 *********************************************************************/

#include "DevelopSystem.h"
#include <Engine/Engine.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/RenderingComponentData.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>

#include "imgui.h"
#include "../Model.h"

using namespace ecs;
//
////--- OpenFileDialog�𗘗p����
//void OpenFBXFile(Model::FBXModelData& out)
//{
//	// �t�@�C���̎w��A�ǂݍ���
//	char filename[MAX_PATH] = "";
//	OPENFILENAME ofn;
//	ZeroMemory(&ofn, sizeof(ofn));
//	ofn.lStructSize = sizeof(ofn);
//	ofn.hwndOwner = GetActiveWindow();
//	// �t�@�C�����̊i�[��
//	ofn.lpstrFile = filename;
//	ofn.nMaxFile = sizeof(filename);
//	// �t�B���^�[
//	ofn.lpstrFilter =
//		"All Files\0*.*\0.fbx\0*.fbx\0";
//	ofn.nFilterIndex = 2;
//	// �t���O
//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//
//	/* �_�C�A���O�̕\��
//	* �_�C�A���O�����܂ł͏�����
//	* �Ԃ�Ȃ��B�u�J���v�������Ɩ߂�l��
//	* TRUE���Ԃ�B����ȊO��FALSE
//	*/
//	if (TRUE == GetOpenFileName(&ofn))
//	{
//		// FBX�t�@�C���̓ǂݍ���
//		Model::LoadFBXModel(ofn.lpstrFile, out);
//	}
//}

 /// @brief ������
void DevelopSystem::onCreate()
{
}

/// @brief �폜��
void DevelopSystem::onDestroy()
{
}

/// @brief �Q�[���I�u�W�F�N�g�������R�[���o�b�N
void DevelopSystem::onStartGameObject(const GameObjectID& id)
{
}

/// @brief �X�V
void DevelopSystem::onUpdate()
{

}

//void CreateFBX()
//{
//	ImGui::Begin("Test");
//
//	if (ImGui::Button("Load FBX"))
//	{
//		// ���f�����[�h
//		Model::FBXModelData fbx;
//		OpenFBXFile(fbx);
//
//		auto* renderer = Engine::get().getRendererManager();
//
//		ShaderDesc desc;
//		desc.m_name = "Lit";
//		desc.m_stages = ShaderStageFlags::VS | ShaderStageFlags::PS;
//		ShaderID shaderID = renderer->createShader(desc);
//		MaterialID matID = renderer->createMaterial("FBXMaterial", shaderID);
//		auto* pMat = renderer->getMaterial(matID);
//		pMat->setTexture("_MainTexture", fbx.textureID);
//
//		RenderData rd;
//		rd.materialID = matID;
//		rd.meshID = fbx.meshID;
//
//		// �I�u�W�F�N�g����
//		Archetype archetype = Archetype::create<DynamicType, Transform, RenderData>();
//		auto goID = getGameObjectManager()->createGameObject("FBXModel", archetype);
//
//		getGameObjectManager()->setComponentData(goID, Transform(goID, Vector3(0, 5, 0)));
//		getGameObjectManager()->setComponentData(goID, rd);
//	}
//
//
//	ImGui::End();
//}
