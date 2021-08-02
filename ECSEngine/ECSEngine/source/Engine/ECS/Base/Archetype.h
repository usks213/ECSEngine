/*****************************************************************//**
 * \file   Archetype.h
 * \brief  �A�[�L�^�C�v
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "IComponentData.h"


namespace ecs {

	/// @struct Archetype
	/// @brief Component�̑g�ݍ��킹(�f�[�^�\����)��\��
	struct Archetype
	{
		/// @brief �A�[�L�^�C�v�̈�v
		/// @param other �ʂ̃A�[�L�^�C�v
		/// @return ��v�Ȃ�True
		constexpr bool operator==(const Archetype& other) const
		{
			if (m_ArchetypeSize != other.m_ArchetypeSize)
				return false;

			for (size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i] != other.m_TypeDataList[i])
					return false;
			}
			return true;
		}

		/// @brief �A�[�L�^�C�v�̕s��v
		/// @param other �ʂ̃A�[�L�^�C�v
		/// @return �s��v�Ȃ�True
		constexpr bool operator!=(const Archetype& other) const
		{
			return !(*this == other);
		}

		/// @brief ���g�������Ă���^���𑊎肪�S�Ď����Ă��邩����
		/// @param other ����̃A�[�L�^�C�v
		/// @return �����Ă���ꍇ��True
		[[nodiscard]] constexpr bool isIn(const Archetype& other) const
		{
			for (size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				auto isIn = false;
				for (size_t j = 0; j < other.m_ArchetypeSize; j++)
				{
					if (m_TypeDataList[i] == other.m_TypeDataList[j])
					{
						isIn = true;
					}
				}
				if (!isIn) return false;
			}
			return true;
		}

		/// @brief template��ComponentData����Archetype�𐶐�
		/// @tparam ...Args �ψ��� ComponentData...
		/// @return ���������A�[�L�^�C�v
		template<typename ...Args>
		static constexpr Archetype create()
		{
			Archetype result;
			result.createImplements<Args...>();

			// �o�u���\�[�g
			for (auto i = 0; i < result.m_ArchetypeSize - 1; ++i)
			{
				for (auto j = i + 1; j < result.m_ArchetypeSize; ++j)
				{
					if (result.m_TypeDataList[i].getHash() > result.m_TypeDataList[j].getHash())
					{
						const auto work = result.m_TypeDataList[i];
						result.m_TypeDataList[i] = result.m_TypeDataList[j];
						result.m_TypeDataList[j] = work;
					}
				}
			}
			// �������T�C�Y�v�Z
			for (auto i = 0; i < result.m_ArchetypeSize; ++i)
			{
				result.m_ArchetypeMemorySize += result.m_TypeDataList[i].getSize();
			}

			return result;
		}

		/// @brief �A�[�L�^�C�v�̒ǉ�
		/// @tparam T �ǉ�����^
		/// @return ���g�̎Q��
		template<typename T>
		constexpr Archetype& addType()
		{
			constexpr auto newType = TypeInfo::create<T>();
			m_ArchetypeMemorySize += sizeof(T);

			// �\�[�g�}��
			for (std::size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i].getHash() > newType.getHash())
				{
					for (auto j = m_ArchetypeSize; j > i; --j)
					{
						m_TypeDataList[j] = m_TypeDataList[j - 1];
					}
					m_TypeDataList[i] = newType;
					++m_ArchetypeSize;
					return *this;
				}
			}

			m_TypeDataList[m_ArchetypeSize] = newType;
			m_ArchetypeSize++;

			return *this;
		}

		/// @brief �����̃n�b�V���l�ƃ������T�C�Y�ŃA�[�L�^�C�v�̒ǉ�
		/// @param typeHash �^�̃n�b�V���l
		/// @param typeSize �^�̃������T�C�Y
		/// @return ���g�̎Q��
		constexpr Archetype& addType(const std::size_t typeHash, const std::size_t typeSize)//, std::string_view typeName)
		{
			const auto newType = TypeInfo::create(typeHash, typeSize);//, typeName);
			m_ArchetypeMemorySize += typeSize;

			// �\�[�g�}��
			for (std::size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i].getHash() > newType.getHash())
				{
					for (auto j = m_ArchetypeSize; j > i; --j)
					{
						m_TypeDataList[j] = m_TypeDataList[j - 1];
					}
					m_TypeDataList[i] = newType;
					++m_ArchetypeSize;
					return *this;
				}
			}

			m_TypeDataList[m_ArchetypeSize] = newType;
			m_ArchetypeSize++;

			return *this;
		}

		/// @brief ���O����^�O��ǉ�
		/// @detail size=0
		/// @param typeName �^�O��
		/// @return ���g�̎Q��
		constexpr Archetype& addTag(std::string_view typeName)
		{
			return addType(TypeNameToTypeHash(typeName), 0);//, typeName);
		}

		/// @brief �n�b�V���l�Ń^�O��ǉ�
		/// @detail size=0, name=typeHash
		/// @param typeHash �^�̃n�b�V���l
		/// @return ���g�̎Q��
		constexpr Archetype& addTag(const std::size_t typeHash)
		{
			return addType(typeHash, 0);//, std::to_string(typeHash));
		}

		/// @brief �w�肵���^�����Ԗڂɂ��邩
		/// @tparam Type IComponentData�^
		/// @return ��v�����ԍ�
		template<typename Type, typename = std::enable_if_t<is_component_data<Type>>>
		[[nodiscard]] constexpr std::size_t getIndex() const
		{
			for (auto i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i] == TypeInfo::create<Type>())
					return i;
			}
			return m_ArchetypeSize;
		}

		/// @brief �w�肵���^�܂ł̃������T�C�Y���擾
		/// @tparam Type IComponentData�^
		/// @return Index�܂ł̃o�C�g��
		template<typename Type, typename = std::enable_if_t<is_component_data<Type>>>
		[[nodiscard]] constexpr std::size_t getOffset() const
		{
			std::size_t result = 0;
			for (auto i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i] == TypeInfo::create<Type>())
					return result;
				result += m_TypeDataList[i].getSize();
			}
			return result;
		}

		///// @brief �w�肵���^���܂ł̃������T�C�Y���擾
		///// @param typeName IComponentData�^�̖��O
		///// @return Index�܂ł̃o�C�g��
		//[[nodiscard]] constexpr std::size_t getOffset(std::string_view typeName) const
		//{
		//	std::size_t result = 0;
		//	for (auto i = 0; i < m_ArchetypeSize; ++i)
		//	{
		//		if (m_TypeDataList[i].getName() == typeName)
		//			return result;
		//		result += m_TypeDataList[i].getSize();
		//	}
		//	return result;
		//}

		/// @brief �w�肵���n�b�V���l�܂ł̃������T�C�Y���擾
		/// @param typeHash �^�C�v�n�b�V��
		/// @return Index�܂ł̃o�C�g��
		[[nodiscard]] constexpr std::size_t getOffset(std::size_t typeHash) const
		{
			std::size_t result = 0;
			for (auto i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i].getHash() == typeHash)
					return result;
				result += m_TypeDataList[i].getSize();
			}
			return result;
		}

		/// @brief �w�肵���C���f�b�N�X�܂ł̃������T�C�Y���擾
		/// @param index N�ԍ�
		/// @return �C���f�b�N�X�܂ł̃o�C�g��
		[[nodiscard]] constexpr std::size_t getOffsetByIndex(const std::size_t index) const
		{
			std::size_t result = 0;
			for (size_t i = 0; i < index; ++i)
			{
				result += m_TypeDataList[i].getSize();
			}
			return result;
		}

		/// @brief �^��񂩂�C���f�b�N�X���擾
		/// @param info �^���
		/// @return ��v�����C���f�b�N�X
		[[nodiscard]] constexpr std::size_t getIndexByTypeInfo(const TypeInfo info) const
		{
			for (size_t i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i] == info)
					return i;
			}
			return m_ArchetypeSize;
		}

		/// @brief �w�肵���^���̃������T�C�Y���擾
		/// @param index �w��l
		/// @return �^�̃T�C�Y
		[[nodiscard]] constexpr std::size_t getSize(const std::size_t index) const
		{
			return m_TypeDataList[index].getSize();
		}

		///// @brief �w�肵���^���̃������T�C�Y���擾
		///// @param typeName �^��
		///// @return �^�̃T�C�Y
		//[[nodiscard]] constexpr std::size_t getSize(std::string_view typeName) const
		//{
		//	std::size_t result = 0;
		//	for (auto i = 0; i < m_ArchetypeSize; ++i)
		//	{
		//		if (m_TypeDataList[i].getName() == typeName)
		//			return m_TypeDataList[i].getSize();
		//	}
		//	return result;
		//}

		/// @brief �w�肵���n�b�V���l�̃������T�C�Y���擾
		/// @param typeHash �n�b�V���l
		/// @return �^�̃T�C�Y
		[[nodiscard]] constexpr std::size_t getSizeByHash(std::size_t typeHash) const
		{
			std::size_t result = 0;
			for (auto i = 0; i < m_ArchetypeSize; ++i)
			{
				if (m_TypeDataList[i].getHash() == typeHash)
					return m_TypeDataList[i].getSize();
			}
			return result;
		}

		/// @brief �w��l��TypeInfo���擾����
		/// @param index �w��l
		/// @return �^���
		[[nodiscard]] constexpr TypeInfo getTypeInfo(const std::size_t index) const
		{
			return m_TypeDataList[index];
		}

		/// @brief �i�[����Ă���Archetype�̐����擾
		/// @return Archetype�̐�
		[[nodiscard]] constexpr std::size_t getArchetypeSize() const
		{
			return m_ArchetypeSize;
		}

		/// @brief �i�[����Ă���Archetype��Byte�����擾
		/// @return Archetype�̐�
		[[nodiscard]] constexpr std::size_t getArchetypeMemorySize() const
		{
			return m_ArchetypeMemorySize;
		}

	private:
		/// @brief �ψ�������^���𐶐�
		/// @tparam Head �擪�^���
		/// @tparam ...Tails �ό^���
		/// @tparam  �^��񂪃R���|�[�l���g�f�[�^��
		template<typename Head, typename ...Tails, typename = std::enable_if_t<is_component_data<Head>>>
		constexpr void createImplements()
		{
			// �^���𐶐�
			m_TypeDataList[m_ArchetypeSize] = TypeInfo::create<Head>();
			m_ArchetypeSize++;

			// �Ō�܂ōċA�ŌJ��Ԃ�
			if constexpr (sizeof...(Tails) != 0)
			{
				createImplements<Tails...>();
			}
		}

		/// @brief �A�[�L�^�C�v�̃������T�C�Y
		std::size_t m_ArchetypeMemorySize = 0;
		/// @brief �i�[����Ă��鐔
		std::size_t m_ArchetypeSize = 0;
		/// @brief �^���̔z��
		TypeInfo m_TypeDataList[c_maxComponentSize];
	};
}