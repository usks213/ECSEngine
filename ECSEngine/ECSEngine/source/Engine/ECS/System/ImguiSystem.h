/*****************************************************************//**
 * \file   ImguiSystem.h
 * \brief  imgui�\���V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/07/05
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>

namespace ecs {

	class ImguiSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(ImguiSystem);
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit ImguiSystem(World* pWorld):
			m_MainCamera(nullptr), 
			m_MaimCameraTransform(nullptr), 
			m_selectObjectID(NONE_GAME_OBJECT_ID),
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~ImguiSystem() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
		void onUpdate() override;

	private:
		/// @brief GUI�\���E����
		/// @param typeName �^��
		/// @param data �f�[�^
		void DispGui(std::string_view typeName, void* data);

		void DispChilds(const GameObjectID parentID);

		void EditTransform(Camera& camera, Transform& transform);


		Camera*			m_MainCamera;
		Transform*		m_MaimCameraTransform;
		GameObjectID	m_selectObjectID;
	};
}