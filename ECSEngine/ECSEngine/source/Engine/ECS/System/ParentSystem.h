/*****************************************************************//**
 * \file   ParentSystem.h
 * \brief  �y�A�����g�t�H�[���V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/07/12
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>


namespace ecs {

	class ParentSystem final : public SystemBase
	{
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit ParentSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~ParentSystem() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
		void onUpdate() override;

	private:
		void updateChild(const GameObjectID& parent, const Transform& parentTransform);
	};
}