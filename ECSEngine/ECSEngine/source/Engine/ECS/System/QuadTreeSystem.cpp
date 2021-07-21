/*****************************************************************//**
 * \file   QuadTreeSystem.h
 * \brief  ��ԕ����V�X�e��(�l����)
 *
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/

#include "QuadTreeSystem.h"
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>

#include <imgui.h>


using namespace ecs;


/// @brief �R���X�g���N�^
/// @param pWorld ���[���h
QuadTreeSystem::QuadTreeSystem(World* pWorld) :
	SystemBase(pWorld),
	m_uiLevel(0),
	m_uiMaxCell(0),
	m_fUnit_W(0.0f),
	m_fUnit_H(0.0f),
	m_fLeft(0.0f),
	m_fTop(0.0f),
	m_fWidth(0.0f),
	m_fHeight(0.0f)
{
	// �K�w���Ƃ̋�Ԑ�
	m_iPow[0] = 1;
	for (std::uint32_t i = 1; i < MAX_LEVEL + 1; i++)
		m_iPow[i] = m_iPow[i - 1] * 4;

}

 /// @brief ������
void QuadTreeSystem::onCreate()
{
	// ��ԃ��x��
	SetLevel(3); // 3=85, 4=341,
	// ��ԃT�C�Y
	SetQuadSize(32, 32);
	// ��Ԃ̒[
	SetLeftTop(-m_fWidth / 2, -m_fHeight / 2);

	// �������m��
	m_dynamicMainList.resize(m_uiMaxCell);
	m_dynamicSubList.resize(m_uiMaxCell);
	m_staticMainList.resize(m_uiMaxCell);
	m_staticSubList.resize(m_uiMaxCell);
	for (unsigned int i = 0; i < m_uiMaxCell; ++i)
	{
		m_dynamicMainList[i].reserve(10);
		m_dynamicSubList[i].reserve(10);
		m_staticMainList[i].reserve(10);
		m_staticSubList[i].reserve(10);
	}
}

/// @brief �폜��
void QuadTreeSystem::onDestroy()
{

}

/// @brief �X�V
void QuadTreeSystem::onUpdate()
{
	// �O��̃f�[�^���폜(����������͂��Ȃ�)
	for (unsigned int i = 0; i < m_uiMaxCell; ++i)
	{
		m_dynamicMainList[i].clear();
		m_dynamicSubList[i].clear();
		m_staticMainList[i].clear();
		m_staticSubList[i].clear();
	}

	// ���[�g���ԍ�
	std::uint32_t Def = 0;
	std::uint32_t wLeftTop = 0;
	std::uint32_t wRightDown = 0;

	// �J�������W
	Transform* cameraTrans = nullptr;
	foreach<Camera, Transform>(
		[&cameraTrans](Camera& camera, Transform& transform)
		{
			cameraTrans = &transform;
		});

	// �J�������W����̑��΍��W
	if (cameraTrans)
	{
		//Vector3 pos = Vector3::Transform(cameraTrans->translation, cameraTrans->globalMatrix);
		Vector3 pos = cameraTrans->translation;
		// ��Ԓ[
		SetLeftTop(pos.x - m_fWidth / 2, pos.y - m_fHeight / 2);
	}
	else
	{
		// ��Ԓ[
		SetLeftTop(-m_fWidth / 2, -m_fHeight / 2);
	}

	// �A�[�L�^�C�v�w��
	auto staticObject = Archetype::create<Transform, StaticType>();
	auto dynamicObject = Archetype::create<Transform, DynamicType>();

	// �`�����N���[�v
	int chunkIndex = 0;
	for (auto&& chunk : m_pWorld->getChunkList())
	{
		// �ÓI�I�u�W�F�N�g
		if (staticObject.isIn(chunk.getArchetype()))
		{
			auto transforms = chunk.getComponentArray<Transform>();
			for (std::uint32_t index = 0; index < chunk.getSize(); ++index)
			{
				// AABB
				Vector3 min(-0.5f, -0.5f, -0.5f);
				Vector3 max(0.5f, 0.5f, 0.5f);
				min = Vector3::Transform(min, transforms[index].globalMatrix);
				max = Vector3::Transform(max, transforms[index].globalMatrix);

				// �����ŋ�Ԃ̓o�^������
				// ����ƉE�����o��
				wLeftTop = GetPointElem(min.x, min.y);
				wRightDown = GetPointElem(max.x, max.y);
				// ��ԊO
				if (wLeftTop >= m_uiMaxCell - 1 || wRightDown >= m_uiMaxCell - 1)
				{
					continue;
				}

				// XOR���Ƃ�	
				Def = wLeftTop ^ wRightDown;
				unsigned int HiLevel = 0;
				unsigned int i;
				for (i = 0; i < m_uiLevel; ++i)
				{
					std::uint32_t Check = (Def >> (i * 2)) & 0x3;
					if (Check != 0)
						HiLevel = i + 1;
				}
				std::uint32_t SpaceNum = wRightDown >> (HiLevel * 2);
				std::uint32_t AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 3;
				SpaceNum += AddNum;	// ���ꂪ��������

				// ��ԊO�͂͂���
				if (SpaceNum > m_uiMaxCell - 1)
				{
					continue;
				}

				// �������Ԃ̃��C�����X�g�Ɋi�[
				m_staticMainList[SpaceNum].push_back(Entity(chunkIndex, index));

				// �������Ԃ̐e�̃T�u�Ɋi�[
				while (SpaceNum > 0)
				{
					SpaceNum--;
					SpaceNum /= 4;
					m_staticSubList[SpaceNum].push_back(Entity(chunkIndex, index));
				}
			}
		}
		// ���I�I�u�W�F�N�g
		if (dynamicObject.isIn(chunk.getArchetype()))
		{
			auto transforms = chunk.getComponentArray<Transform>();
			for (std::uint32_t index = 0; index < chunk.getSize(); ++index)
			{
				// AABB
				Vector3 min(-0.5f, -0.5f, -0.5f);
				Vector3 max(0.5f, 0.5f, 0.5f);
				min = Vector3::Transform(min, transforms[index].globalMatrix);
				max = Vector3::Transform(max, transforms[index].globalMatrix);

				// �����ŋ�Ԃ̓o�^������
				// ����ƉE�����o��
				wLeftTop = GetPointElem(min.x, min.y);
				wRightDown = GetPointElem(max.x, max.y);
				// ��ԊO
				if (wLeftTop >= m_uiMaxCell - 1 || wRightDown >= m_uiMaxCell - 1)
				{
					continue;
				}

				// XOR���Ƃ�	
				Def = wLeftTop ^ wRightDown;
				unsigned int HiLevel = 0;
				unsigned int i;
				for (i = 0; i < m_uiLevel; ++i)
				{
					std::uint32_t Check = (Def >> (i * 2)) & 0x3;
					if (Check != 0)
						HiLevel = i + 1;
				}
				std::uint32_t SpaceNum = wRightDown >> (HiLevel * 2);
				std::uint32_t AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 3;
				SpaceNum += AddNum;	// ���ꂪ��������

				// ��ԊO�͂͂���
				if (SpaceNum > m_uiMaxCell - 1)
				{
					continue;
				}

				// �������Ԃ̃��C�����X�g�Ɋi�[
				m_dynamicMainList[SpaceNum].push_back(Entity(chunkIndex, index));

				// �������Ԃ̐e�̃T�u�Ɋi�[
				while (SpaceNum > 0)
				{
					SpaceNum--;
					SpaceNum /= 4;
					m_dynamicSubList[SpaceNum].push_back(Entity(chunkIndex, index));
				}
			}
		}
		chunkIndex++;
	}

	//ImGui::Begin("QuadTree");
	//int i = 0;
	//for (const auto& list : m_staticSubList)
	//{
	//	if (ImGui::TreeNodeEx(std::to_string(i).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		for (const auto& entity : list)
	//		{
	//			ImGui::Text(std::to_string(entity.m_index).c_str());
	//		}
	//		ImGui::TreePop();
	//	}
	//	i++;
	//}

	//ImGui::End();
}
