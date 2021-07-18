/*****************************************************************//**
 * \file   QuadTreeSystem.h
 * \brief  ��ԕ����V�X�e��(�l����)
 * 
 * \author USAMI KOSHI
 * \date   2021/07/18
 *********************************************************************/
#pragma once

#include <Engine/ECS/Base/SystemBase.h>
#include <array>


namespace ecs {

	class QuadTreeSystem final : public SystemBase
	{
		ECS_DECLARE_SYSTEM(QuadTreeSystem);
	public:
		/// @brief �R���X�g���N�^
		/// @param pWorld ���[���h
		explicit QuadTreeSystem(World* pWorld);

		/// �f�X�g���N�^
		~QuadTreeSystem() = default;

		/// @brief ������
		void onCreate() override;
		/// @brief �폜��
		void onDestroy() override;
		/// @brief �X�V
		void onUpdate() override;

		/// @brief ��ԕ������̎擾
		std::uint32_t getCellNum() { return m_uiMaxCell; }

	private:

		// �r�b�g�����֐�
		std::uint32_t BitSeparate32(std::uint32_t n)
		{
			n = (n | (n << 8)) & 0x00ff00ff;
			n = (n | (n << 4)) & 0x0f0f0f0f;
			n = (n | (n << 2)) & 0x33333333;
			return (n | (n << 1)) & 0x55555555;
		}

		// 2D���[�g����Ԕԍ��Z�o�֐�
		std::uint16_t Get2DMortonNumber(std::uint16_t x, std::uint16_t y)
		{
			return (std::uint16_t)(BitSeparate32(x) | (BitSeparate32(y) << 1));
		}

		// ���W�����`4���ؗv�f�ԍ��ϊ��֐�
		std::uint32_t GetPointElem(float pos_x, float pos_y)
		{
			// �K�w�I�t�Z�b�g
			pos_x += m_fWidth / 2;
			pos_y += m_fHeight / 2;

			// �̈�O�Ȃ�e��
			if (pos_x - m_fLeft < 0 || pos_y - m_fTop < 0)
				return 0xffffffff;

			// �{���̓t�B�[���h�̑傫���Ƃ�
			return Get2DMortonNumber((std::uint16_t)((pos_x - m_fLeft) / m_fUnit_W),
				(std::uint16_t)((pos_y - m_fTop) / m_fUnit_H));
		}

		void SetLevel(std::uint32_t level)
		{
			if (level >= MAX_LEVEL) return;

			m_uiLevel = level;
			m_uiMaxCell = (m_iPow[level + 1] - 1) / 3;
		}

		void SetQuadSize(float width, float height)
		{
			m_fWidth = width;
			m_fHeight = height;
			// �ŏ����x����ԃT�C�Y�X�V
			m_fUnit_W = width / (float)(1 << (m_uiLevel - 1));
			m_fUnit_H = height / (float)(1 << (m_uiLevel - 1));
		}

		void SetLeftTop(float left, float height)
		{
			m_fLeft = left;
			m_fTop = height;
		}

	private:
		float m_fUnit_W;				// �ŏ����x����Ԃ̕��P��
		float m_fUnit_H;				// �ŏ����x����Ԃ̍��P��
		float m_fLeft;					// �̈�̍����iX���ŏ��l�j
		float m_fTop;					// �̈�̏㑤�iZ���ŏ��l�j
		float m_fWidth;					// �̈�̕�
		float m_fHeight;				// �̈�̍���
		std::uint32_t m_uiLevel;		// �ŉ��ʃ��x��
		std::uint32_t m_uiMaxCell;		// ��ԕ�����

		static constexpr std::uint32_t MAX_LEVEL = 9;
		std::array<std::uint32_t, MAX_LEVEL + 1> m_iPow;

	public:
		std::vector<std::vector<Entity>> m_dynamicMainList;
		std::vector<std::vector<Entity>> m_dynamicSubList;
		std::vector<std::vector<Entity>> m_staticMainList;
		std::vector<std::vector<Entity>> m_staticSubList;
	};
}