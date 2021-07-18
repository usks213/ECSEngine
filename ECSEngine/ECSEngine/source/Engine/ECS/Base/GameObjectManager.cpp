/*****************************************************************//**
 * \file   GameObjectManager.h
 * \brief  �Q�[���I�u�W�F�N�g�}�l�[�W���[
 *
 * \author USAMI KOSHI
 * \date   2021/07/13
 *********************************************************************/

#include "GameObjectManager.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>

using namespace ecs;


/// @brief �Q�[���I�u�W�F�N�g�̐���
/// @param archetype �A�[�L�^�C�v
/// @return ���������Q�[���I�u�W�F�N�gID
GameObjectID GameObjectManager::createGameObject(std::string_view name, const Archetype& archetype)
{
	auto entity = m_pWorld->getEntityManager()->createEntity(archetype);
	GameObjectID id = NONE_GAME_OBJECT_ID;

	while (true)
	{
		id =  static_cast<GameObjectID>(rand());
		const auto& itr = m_game0bjectMap.find(id);
		if (m_game0bjectMap.end() == itr) break;
	}

	m_game0bjectMap[id] = std::make_unique<GameObject>(id, name, entity);
	m_rootList.push_back(id);
	sortRootList();

	return id;
}

/// @brief �Q�[���I�u�W�F�N�g�̍폜
/// @param gameObjectID �폜����Q�[���I�u�W�F�N�gID
void GameObjectManager::destroyGameObject(const GameObjectID& gameObjectID)
{

}


/// @brief �e�̐ݒ�
/// @param gameObjectID ���g
/// @param parentID �e
void GameObjectManager::SetParent(const GameObjectID& gameObjectID, const GameObjectID& parentID)
{
	// ���g�̎q�ɐe��ݒ肷��ꍇ�͂ł��Ȃ�
	if (searchParent(parentID, gameObjectID) == gameObjectID)
	{
		return;
	}
	// �����e��������X�V���Ȃ�
	if (GetParent(gameObjectID) == parentID)
	{
		return;
	}

	// ���g�̐e
	auto parent = m_game0bjectMap[gameObjectID]->GetParentID();
	auto transform = getComponentData<Transform>(gameObjectID);
	auto parentTrans = getComponentData<Transform>(parentID);

	auto itr = findRootList(gameObjectID);
	if (itr != m_rootList.end() && *itr == gameObjectID)
	{
		//--- ���g�����[�g�������ꍇ�͍폜
		m_rootList.erase(itr);
	}
	else
	{
		//--- �Ⴄ�ꍇ�͐e�̎q���玩�g���폜
		// �e�̎q���玩�g���폜
		m_game0bjectMap[parent]->RemoveChildID(gameObjectID);
		// �e��������
		m_game0bjectMap[gameObjectID]->ResetParentID();
		// �s��Čv�Z
		auto global = transform->globalMatrix;
		// �ʒu
		transform->translation = global.Translation();
		// ��]
		Matrix invSca = Matrix::CreateScale(transform->globalScale);
		invSca = invSca.Invert();
		transform->rotation = Quaternion::CreateFromRotationMatrix(invSca * global);
		// �g�k
		transform->scale = transform->globalScale;
		// �}�g���b�N�X�X�V
		// �g�k
		transform->localMatrix = Matrix::CreateScale(transform->scale);
		// ��]
		transform->localMatrix *= Matrix::CreateFromQuaternion(transform->rotation);
		// �ړ�
		transform->localMatrix *= Matrix::CreateTranslation(transform->translation);
	}

	// ���g�ɐe��ݒ�
	m_game0bjectMap[gameObjectID]->SetParentID(parentID);

	// �e�̎q�Ɏ��g��ݒ�
	m_game0bjectMap[parentID]->AddChildID(gameObjectID);


	// �e�̋t�s��𔽉f
	auto invParent = (parentTrans->globalMatrix).Invert();
	auto invParentScale = Vector3(1,1,1) / parentTrans->globalScale;
	auto localMatrix = transform->localMatrix * invParent;
	// �ʒu
	transform->translation = localMatrix.Translation();
	// ��]
	Matrix invSca = Matrix::CreateScale(transform->scale * invParentScale);
	invSca = invSca.Invert();
	transform->rotation = Quaternion::CreateFromRotationMatrix(invSca * localMatrix);
	// �g�k
	transform->scale = transform->scale * invParentScale;
	// �}�g���b�N�X�X�V
	// �g�k
	transform->localMatrix = Matrix::CreateScale(transform->scale);
	// ��]
	transform->localMatrix *= Matrix::CreateFromQuaternion(transform->rotation);
	// �ړ�
	transform->localMatrix *= Matrix::CreateTranslation(transform->translation);
}
/// @brief �e�̎擾
/// @param gameObjectID ���g
/// @retrun �eID
GameObjectID GameObjectManager::GetParent(const GameObjectID& gameObjectID)
{
	return m_game0bjectMap[gameObjectID]->GetParentID();
}
/// @brief �e�q�֌W�̍폜
/// @param gameObjectID ���g
void GameObjectManager::ResetParent(const GameObjectID& gameObjectID)
{
	// �e�̎擾
	auto parent = m_game0bjectMap[gameObjectID]->GetParentID();
	if (parent == NONE_GAME_OBJECT_ID) return;

	// �s��Čv�Z
	auto transform = getComponentData<Transform>(gameObjectID);
	auto global = transform->globalMatrix;
	// �ʒu
	transform->translation = global.Translation();
	// ��]
	Matrix invSca = Matrix::CreateScale(transform->globalScale);
	invSca = invSca.Invert();
	transform->rotation = Quaternion::CreateFromRotationMatrix(invSca * global);
	// �g�k
	transform->scale = transform->globalScale;
	// �}�g���b�N�X�X�V
	// �g�k
	transform->localMatrix = Matrix::CreateScale(transform->scale);
	// ��]
	transform->localMatrix *= Matrix::CreateFromQuaternion(transform->rotation);
	// �ړ�
	transform->localMatrix *= Matrix::CreateTranslation(transform->translation);

	// �e�̎q���玩�g���폜
	m_game0bjectMap[parent]->RemoveChildID(gameObjectID);
	// �e��������
	m_game0bjectMap[gameObjectID]->ResetParentID();
	m_rootList.push_back(gameObjectID);
	sortRootList();
}

/// @brief �q�̒ǉ�
/// @param gameObjectID ���g
/// @param childID �q
void GameObjectManager::AddChild(const GameObjectID& gameObjectID, const GameObjectID& childID)
{
	SetParent(childID, gameObjectID);
}
/// @brief �q���X�g�̎擾
/// @param gameObjectID ���g
/// @return �q���X�g
const std::vector<GameObjectID>& GameObjectManager::GetChilds(const GameObjectID& gameObjectID)
{
	return m_game0bjectMap[gameObjectID]->GetChildsID();
}
/// @brief �q�̍폜
/// @param gameObjectID ���g 
/// @param childID �q
void GameObjectManager::RemoveChild(const GameObjectID& gameObjectID, const GameObjectID& childID)
{
	m_game0bjectMap[gameObjectID]->RemoveChildID(childID);
	m_game0bjectMap[childID]->ResetParentID();
	m_rootList.push_back(childID);
	sortRootList();
}
