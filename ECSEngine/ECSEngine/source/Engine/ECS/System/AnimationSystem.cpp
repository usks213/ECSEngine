/*****************************************************************//**
 * \file   AnimationSystem.cpp
 * \brief  �A�j���[�V�����V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/09/30
 *********************************************************************/

#include "AnimationSystem.h"
#include <Engine/Engine.h>
#include <Engine/ECS/ComponentData/AnimationComponentData.h>
#include <Engine/ECS/ComponentData/BasicComponentData.h>

using namespace ecs;

void calcAnim(GameObjectManager* goMgr, Animation& animation, const GameObjectID& nodeID, double time);
void Evaluate(Transform& transform, Animation& anim, Animation::NodeAnimInfo& channel, double time);

 /// @brief ������
void AnimationSystem::onCreate()
{
}

/// @brief �폜��
void AnimationSystem::onDestroy()
{
}

/// @brief �Q�[���I�u�W�F�N�g�������R�[���o�b�N
void AnimationSystem::onStartGameObject(const GameObjectID& id)
{
	auto* animator = getGameObjectManager()->getComponentData<Animator>(id);
	if (animator)
	{
	}
}

/// @brief �X�V
void AnimationSystem::onUpdate()
{
	auto* goMgr = m_pWorld->getGameObjectManager();
	auto* renderer = m_pWorld->getWorldManager()->getEngine()->getRendererManager();
	static float TestTime;
	TestTime += 1.0f / 60;

	// �A�j���[�V�����X�V
	foreach<Transform, Animator, DynamicType>(
		[&goMgr, &renderer](Transform& transform, Animator& animator, DynamicType& type)
		{
			animator.time = TestTime;
			Animation& animation = *renderer->getAnimation(animator.animationID);

			// 1�b������̃e�B�b�N�𒊏o����B�w�肳��Ă��Ȃ��ꍇ�͊���l��z��B
			double ticksPerSecond = animation.m_ticksPerSecond != 0.0 ? animation.m_ticksPerSecond : 25.0;
			// �ȍ~�̖���̌v�Z�̓e�B�b�N�P�ʂōs��
			animator.time *= ticksPerSecond;
			// �A�j���̎������ԂɊ��蓖��
			double time = 0.0f;
			if (animation.m_duration > 0.0) {
				time = fmod(animator.time, animation.m_duration);
			}
			// �v�Z
			calcAnim(goMgr, animation, transform.id, time);


			//mLastTime = time;
		});
}

void calcAnim(GameObjectManager* goMgr, Animation& animation, const GameObjectID& nodeID, double time)
{
	// �Q�[���I�u�W�F�N�g�擾
	auto* go = goMgr->getGameObject(nodeID);
	if (go)
	{
		auto itr = animation.m_channelTable.find(go->getName().data());
		if (animation.m_channelTable.end() != itr)
		{
			auto* trans = goMgr->getComponentData<Transform>(nodeID);
			// �L�[�t���[���v�Z
			Evaluate(*trans, animation, animation.m_channelList[itr->second], time);
		}
	}
	// �q�m�[�h
	for (auto child : goMgr->GetChilds(nodeID))
	{
		calcAnim(goMgr, animation, child, time);
	}
}

// �w�肳�ꂽ�����̃A�j���[�V�����g���b�N��]��
void Evaluate(Transform& transform, Animation& anim, Animation::NodeAnimInfo& channel, double time)
{
	// �e�A�j���[�V�����`�����l���̕ϊ��}�g���b�N�X���Z�o

	// ******** ���W ********
	Vector3 presentPosition(0, 0, 0);
	if (channel.numPosKeys > 0) {
		// ���݂̃t���[���ԍ���T���B���Ԃ��Ō�̎��Ԃ���̏ꍇ�͍Ō�̈ʒu���猟�����A�����łȂ��ꍇ�͍ŏ����猟������B
		// �ŏ�����T�����ϓI�Ȏ�����A�����ƍ����ɏ�������K�v������B
		unsigned int frame = 0;//(time >= mLastTime) ? std::get<0>(mLastPositions[a]) : 0;
		while (frame < channel.numPosKeys - 1) {
			if (time < channel.posKeys[frame + 1].time) {
				break;
			}
			++frame;
		}

		// ���̃t���[���̒l�Ǝ��̃t���[���̒l�̊Ԃ���
		unsigned int nextFrame = (frame + 1) % channel.numPosKeys;
		const auto& key = channel.posKeys[frame];
		const auto& nextKey = channel.posKeys[nextFrame];
		double diffTime = nextKey.time - key.time;
		if (diffTime < 0.0) {
			diffTime += anim.m_duration;
		}
		if (diffTime > 0) {
			float factor = float((time - key.time) / diffTime);
			presentPosition = key.value + (nextKey.value - key.value) * factor;
		}
		else {
			presentPosition = key.value;
		}

		//std::get<0>(mLastPositions[a]) = frame;
	}

	// ******** ��] ********
	Quaternion presentRotation(0, 0, 0, 1);
	if (channel.numRotKeys > 0) {
		unsigned int frame = 0;//(time >= mLastTime) ? std::get<1>(mLastPositions[a]) : 0;
		while (frame < channel.numRotKeys - 1) {
			if (time < channel.rotKeys[frame + 1].time) {
				break;
			}
			++frame;
		}

		// ���̃t���[���̒l�Ǝ��̃t���[���̒l�̊Ԃ���
		unsigned int nextFrame = (frame + 1) % channel.numRotKeys;
		const auto& key = channel.rotKeys[frame];
		const auto& nextKey = channel.rotKeys[nextFrame];
		double diffTime = nextKey.time - key.time;
		if (diffTime < 0.0) {
			diffTime += anim.m_duration;
		}
		if (diffTime > 0) {
			float factor = float((time - key.time) / diffTime);
			presentRotation.Slerp(key.value, nextKey.value, factor);
		}
		else {
			presentRotation = key.value;
		}

		//std::get<1>(mLastPositions[a]) = frame;
	}

	// ******** �g�k ********
	Vector3 presentScaling(1, 1, 1);
	if (channel.numScaKeys > 0) {
		unsigned int frame = 0;//(time >= mLastTime) ? std::get<2>(mLastPositions[a]) : 0;
		while (frame < channel.numScaKeys - 1) {
			if (time < channel.scaKeys[frame + 1].time) {
				break;
			}
			++frame;
		}

		// TODO: ��Ԃ��K�v�ł�? �����ł͐��`��ԂłȂ��ΐ������B
		presentScaling = channel.scaKeys[frame].value;
		//std::get<2>(mLastPositions[a]) = frame;
	}

	// �ϊ��}�g���b�N�X���\�z
	transform.translation = presentPosition;
	transform.rotation = presentRotation;
	transform.scale = presentScaling;

}