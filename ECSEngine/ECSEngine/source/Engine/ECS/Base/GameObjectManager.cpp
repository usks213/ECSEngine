/*****************************************************************//**
 * \file   GameObjectManager.h
 * \brief  �Q�[���I�u�W�F�N�g�}�l�[�W���[
 *
 * \author USAMI KOSHI
 * \date   2021/07/13
 *********************************************************************/

#include "GameObjectManager.h"
#include <Engine/ECS/Base/SystemBase.h>
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

	// �������X�g�Ɋi�[
	m_createList.push_back(id);

	return id;
}

/// @brief �Q�[���I�u�W�F�N�g�̍폜
/// @param gameObjectID �폜����Q�[���I�u�W�F�N�gID
void GameObjectManager::destroyGameObject(const GameObjectID& gameObjectID)
{
	auto* obj = getGameObject(gameObjectID);
	if (obj == nullptr) return;

	m_destroyList.push_back(gameObjectID);
	for(auto& child : obj->GetChildsID())
	{
		destroyGameObject(child);
	}
}

/// @brief �������X�g�̎��s
void GameObjectManager::StartUp()
{
	// �X�^�[�g�A�b�v
	for (const auto& id : m_createList)
	{
		// �X�V�O�R�[���o�b�N
		for (auto&& system : m_pWorld->getSystemList())
		{
			system->onStartGameObject(id);
		}
	}

	// �N���A
	m_createList.clear();
}

/// @brief �폜���X�g�̎��s
void GameObjectManager::CleanUp()
{
	// �N���[���A�b�v
	for (const auto& id : m_destroyList)
	{
		cleanUpGameObject(id);
	}

	// �N���A
	m_destroyList.clear();
}

/// @brief �e�̐ݒ�
/// @param gameObjectID ���g
/// @param parentID �e
void GameObjectManager::SetParent(const GameObjectID& gameObjectID, const GameObjectID& parentID)
{
	if (getGameObject(gameObjectID) == nullptr) return;
	if (getGameObject(parentID) == nullptr) return;

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
	if (getGameObject(gameObjectID) == nullptr) return NONE_GAME_OBJECT_ID;
	return m_game0bjectMap[gameObjectID]->GetParentID();
}
/// @brief �e�q�֌W�̍폜
/// @param gameObjectID ���g
void GameObjectManager::ResetParent(const GameObjectID& gameObjectID)
{
	if (getGameObject(gameObjectID) == nullptr) return;

	// �e�̎擾
	auto parent = m_game0bjectMap[gameObjectID]->GetParentID();
	if (parent == NONE_GAME_OBJECT_ID) return;
	if (getGameObject(parent) == nullptr) return;


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
	if (getGameObject(gameObjectID) == nullptr) return;
	if (getGameObject(childID) == nullptr) return;

	m_game0bjectMap[gameObjectID]->RemoveChildID(childID);
	m_game0bjectMap[childID]->ResetParentID();
	m_rootList.push_back(childID);
	sortRootList();
}

void GameObjectManager::cleanUpGameObject(const GameObjectID& gameObjectID)
{
	// ����
	auto itr = m_game0bjectMap.find(gameObjectID);
	if (m_game0bjectMap.end() == itr) return;

	// �폜���R�[���o�b�N
	for (auto&& system : m_pWorld->getSystemList())
	{
		system->onDestroyGameObject(gameObjectID);
	}

	// �`�����N
	auto& chunk = m_pWorld->getChunkList()[itr->second->m_entity.m_chunkIndex];
	auto curSize = chunk.getSize() - 1;

	//// ���g�����̃I�u�W�F�N�g�̃C���f�b�N�X���l�߂�
	//for (auto i = itr->second->m_index + 1; i < curSize; ++i)
	//{
	//	auto* transform = chunk.getComponentData<Transform>(i);
	//	auto* other = m_game0bjectMap[transform->id].get();
	//	other->m_index--;
	//}

	// �Ō���Ɏ��g�̃C���f�b�N�X���㏑��
	auto* transform = chunk.getComponentData<Transform>(curSize);
	auto* other = getGameObject(transform->id);
	other->m_entity.m_index = itr->second->m_entity.m_index;

	// �G���e�B�e�B�̍폜
	chunk.destroyEntity(Entity(itr->second->m_entity));

	// �e�q�֌W����
	ResetParent(gameObjectID);
	auto itr2 = findRootList(gameObjectID);
	if (itr2 != m_rootList.end() && *itr2 == gameObjectID)
	{
		//--- ���g�����[�g�������ꍇ�͍폜
		m_rootList.erase(itr2);
	}

	// �}�b�v����폜
	m_game0bjectMap.erase(itr);
}
