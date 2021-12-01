/*****************************************************************//**
 * \file   AnimationSystem.cpp
 * \brief  アニメーションシステム
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

 /// @brief 生成時
void AnimationSystem::onCreate()
{
}

/// @brief 削除時
void AnimationSystem::onDestroy()
{
}

/// @brief ゲームオブジェクト生成時コールバック
void AnimationSystem::onStartGameObject(const GameObjectID& id)
{
	auto* animator = getGameObjectManager()->getComponentData<Animator>(id);
	if (animator)
	{
	}
}

/// @brief 更新
void AnimationSystem::onUpdate()
{
	auto* goMgr = m_pWorld->getGameObjectManager();
	auto* renderer = m_pWorld->getWorldManager()->getEngine()->getRendererManager();
	static float TestTime;
	TestTime += 1.0f / 60;

	// アニメーション更新
	foreach<Transform, Animator, DynamicType>(
		[&goMgr, &renderer](Transform& transform, Animator& animator, DynamicType& type)
		{
			animator.time = TestTime;
			Animation& animation = *renderer->getAnimation(animator.animationID);

			// 1秒あたりのティックを抽出する。指定されていない場合は既定値を想定。
			double ticksPerSecond = animation.m_ticksPerSecond != 0.0 ? animation.m_ticksPerSecond : 25.0;
			// 以降の毎回の計算はティック単位で行う
			animator.time *= ticksPerSecond;
			// アニメの持続時間に割り当て
			double time = 0.0f;
			if (animation.m_duration > 0.0) {
				time = fmod(animator.time, animation.m_duration);
			}
			// 計算
			calcAnim(goMgr, animation, transform.id, time);


			//mLastTime = time;
		});
}

void calcAnim(GameObjectManager* goMgr, Animation& animation, const GameObjectID& nodeID, double time)
{
	// ゲームオブジェクト取得
	auto* go = goMgr->getGameObject(nodeID);
	if (go)
	{
		auto itr = animation.m_channelTable.find(go->getName().data());
		if (animation.m_channelTable.end() != itr)
		{
			auto* trans = goMgr->getComponentData<Transform>(nodeID);
			// キーフレーム計算
			Evaluate(*trans, animation, animation.m_channelList[itr->second], time);
		}
	}
	// 子ノード
	for (auto child : goMgr->GetChilds(nodeID))
	{
		calcAnim(goMgr, animation, child, time);
	}
}

// 指定された時刻のアニメーショントラックを評価
void Evaluate(Transform& transform, Animation& anim, Animation::NodeAnimInfo& channel, double time)
{
	// 各アニメーションチャンネルの変換マトリックスを算出

	// ******** 座標 ********
	Vector3 presentPosition(0, 0, 0);
	if (channel.numPosKeys > 0) {
		// 現在のフレーム番号を探す。時間が最後の時間より後の場合は最後の位置から検索し、そうでない場合は最初から検索する。
		// 最初から探す平均的な事例より、ずっと高速に処理する必要がある。
		unsigned int frame = 0;//(time >= mLastTime) ? std::get<0>(mLastPositions[a]) : 0;
		while (frame < channel.numPosKeys - 1) {
			if (time < channel.posKeys[frame + 1].time) {
				break;
			}
			++frame;
		}

		// このフレームの値と次のフレームの値の間を補間
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

	// ******** 回転 ********
	Quaternion presentRotation(0, 0, 0, 1);
	if (channel.numRotKeys > 0) {
		unsigned int frame = 0;//(time >= mLastTime) ? std::get<1>(mLastPositions[a]) : 0;
		while (frame < channel.numRotKeys - 1) {
			if (time < channel.rotKeys[frame + 1].time) {
				break;
			}
			++frame;
		}

		// このフレームの値と次のフレームの値の間を補間
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

	// ******** 拡縮 ********
	Vector3 presentScaling(1, 1, 1);
	if (channel.numScaKeys > 0) {
		unsigned int frame = 0;//(time >= mLastTime) ? std::get<2>(mLastPositions[a]) : 0;
		while (frame < channel.numScaKeys - 1) {
			if (time < channel.scaKeys[frame + 1].time) {
				break;
			}
			++frame;
		}

		// TODO: 補間が必要では? ここでは線形補間でなく対数かも。
		presentScaling = channel.scaKeys[frame].value;
		//std::get<2>(mLastPositions[a]) = frame;
	}

	// 変換マトリックスを構築
	transform.translation = presentPosition;
	transform.rotation = presentRotation;
	transform.scale = presentScaling;

}