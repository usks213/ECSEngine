/*****************************************************************//**
 * \file   Animation.h
 * \brief  アニメーション
 * 
 * \author USAMI KOSHI
 * \date   2021/09/30
 *********************************************************************/
#pragma once

#include <string>
#include <Engine/Utility/Mathf.h>

/// @brief アニメーションID
using AnimationID = std::uint32_t;
/// @brief 存在しないアニメーションID
constexpr AnimationID NONE_ANIMATION_ID = std::numeric_limits<AnimationID>::max();


/// @class Animation
/// @brief アニメーション
class Animation
{
public:
	struct NodeAnimInfo
	{
		struct Vector3Key
		{
			double time;
			Vector3 value;
		};
		struct QuatKey
		{
			double time;
			Quaternion value;
		};

		std::string boneName;
		std::uint32_t numPosKeys;
		std::uint32_t numRotKeys;
		std::uint32_t numScaKeys;
		std::vector<Vector3Key>	posKeys;
		std::vector<QuatKey>	rotKeys;
		std::vector<Vector3Key>	scaKeys;
	};

public:
	/// @brief コンストラクタ
	/// @param id アニメーションID
	/// @param name 名前
	Animation(const AnimationID& id, std::string_view name) :
		m_id(id), m_name(name),
		m_duration(0), m_ticksPerSecond(0),
		m_numChannels(0)
	{
	}

	/// @brief デストラクタ
	~Animation() = default;

public:
	AnimationID					m_id;
	std::string					m_name;

	double						m_duration;		// アニメーションの長押し(ミリ秒？)
	double						m_ticksPerSecond;	// 単位フレーム
	std::uint32_t				m_numChannels;	// チャンネル数
	std::vector<NodeAnimInfo>	m_channelList;	// チャンネルリスト
	std::unordered_map<std::string, std::uint32_t>	m_channelTable;	// 名前検索
};

