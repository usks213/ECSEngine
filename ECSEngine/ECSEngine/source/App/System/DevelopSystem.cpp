/*****************************************************************//**
 * \file   DevelopSystem.h
 * \brief  開発用システム
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
////--- OpenFileDialogを利用した
//void OpenFBXFile(Model::FBXModelData& out)
//{
//	// ファイルの指定、読み込み
//	char filename[MAX_PATH] = "";
//	OPENFILENAME ofn;
//	ZeroMemory(&ofn, sizeof(ofn));
//	ofn.lStructSize = sizeof(ofn);
//	ofn.hwndOwner = GetActiveWindow();
//	// ファイル名の格納先
//	ofn.lpstrFile = filename;
//	ofn.nMaxFile = sizeof(filename);
//	// フィルター
//	ofn.lpstrFilter =
//		"All Files\0*.*\0.fbx\0*.fbx\0";
//	ofn.nFilterIndex = 2;
//	// フラグ
//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//
//	/* ダイアログの表示
//	* ダイアログを閉じるまでは処理が
//	* 返らない。「開く」を押すと戻り値で
//	* TRUEが返る。それ以外はFALSE
//	*/
//	if (TRUE == GetOpenFileName(&ofn))
//	{
//		// FBXファイルの読み込み
//		Model::LoadFBXModel(ofn.lpstrFile, out);
//	}
//}

 /// @brief 生成時
void DevelopSystem::onCreate()
{
}

/// @brief 削除時
void DevelopSystem::onDestroy()
{
}

/// @brief ゲームオブジェクト生成時コールバック
void DevelopSystem::onStartGameObject(const GameObjectID& id)
{
}

/// @brief 更新
void DevelopSystem::onUpdate()
{

}

//void CreateFBX()
//{
//	ImGui::Begin("Test");
//
//	if (ImGui::Button("Load FBX"))
//	{
//		// モデルロード
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
//		// オブジェクト生成
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
