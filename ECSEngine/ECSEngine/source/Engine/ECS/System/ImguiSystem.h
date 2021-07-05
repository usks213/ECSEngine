/*****************************************************************//**
 * \file   ImguiSystem.h
 * \brief  imgui�\���V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/07/05
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>

namespace ecs {

	class ImguiSystem final : public SystemBase
	{
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit ImguiSystem(World* pWorld):
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
	};
}