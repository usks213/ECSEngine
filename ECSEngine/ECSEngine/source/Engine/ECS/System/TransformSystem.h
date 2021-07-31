/*****************************************************************//**
 * \file   TransformSystem.h
 * \brief  �g�����X�t�H�[���V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <Engine/ECS/Base/GameObjectManager.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>


namespace ecs {

	class TransformSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(TransformSystem);
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit TransformSystem(World* pWorld):
			SystemBase(pWorld)
		{}
		/// �f�X�g���N�^
		~TransformSystem() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
		void onUpdate() override;

		/// @brief �Q�[���I�u�W�F�N�g�X�V�O�Ɉ�x�����Ă΂��R�[���o�b�N
		void onStartGameObject(const GameObjectID& id) override;

		static inline void updateTransform(Transform& transform);
		static inline void updateHierarchy(GameObjectManager* mgr, std::vector<GameObjectID>& rootList);
		static inline void updateChildsTransform(GameObjectManager* mgr, const GameObjectID& parent, const Matrix& globalMatrix, const Vector3& globalScale);

	private:

	};
}