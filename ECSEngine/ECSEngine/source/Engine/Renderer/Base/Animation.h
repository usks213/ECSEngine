/*****************************************************************//**
 * \file   Animation.h
 * \brief  �A�j���[�V����
 * 
 * \author USAMI KOSHI
 * \date   2021/09/30
 *********************************************************************/
#pragma once

#include <string>
#include <Engine/Utility/Mathf.h>

/// @brief �A�j���[�V����ID
using AnimationID = std::uint32_t;
/// @brief ���݂��Ȃ��A�j���[�V����ID
constexpr AnimationID NONE_ANIMATION_ID = std::numeric_limits<AnimationID>::max();


/// @class Animation
/// @brief �A�j���[�V����
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
	/// @brief �R���X�g���N�^
	/// @param id �A�j���[�V����ID
	/// @param name ���O
	Animation(const AnimationID& id, std::string_view name) :
		m_id(id), m_name(name),
		m_duration(0), m_ticksPerSecond(0),
		m_numChannels(0)
	{
	}

	/// @brief �f�X�g���N�^
	~Animation() = default;

public:
	AnimationID					m_id;
	std::string					m_name;

	double						m_duration;		// �A�j���[�V�����̒�����(�~���b�H)
	double						m_ticksPerSecond;	// �P�ʃt���[��
	std::uint32_t				m_numChannels;	// �`�����l����
	std::vector<NodeAnimInfo>	m_channelList;	// �`�����l�����X�g
	std::unordered_map<std::string, std::uint32_t>	m_channelTable;	// ���O����
};

