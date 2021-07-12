/*****************************************************************//**
 * \file   Chunk.h
 * \brief  �`�����N
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/
#pragma once

#include "Archetype.h"
#include "BinaryStream.h"
#include "ComponentArray.h"



namespace ecs {

	/// @class Chunk
	/// @brief �`�����N
	class Chunk
	{
	public:

		/// @brief �^���(ICompoenetData)���琶��
		/// @tparam ...Args �^���(IComponentData)
		/// @param capacity �`�����N�̗e��
		/// @return �`�����N
		template<typename ...Args>
		static Chunk create(const std::uint32_t capacity = 1)
		{
			constexpr auto archetype = Archetype::create<Args...>();
			return create(archetype, capacity);
		}

		/// @brief �A�[�L�^�C�v���琶��
		/// @param archetype  �A�[�L�^�C�v
		/// @param capacity �`�����N�̗e��
		/// @return �`�����N
		static Chunk create(const Archetype& archetype, std::uint32_t capacity = 1);

		/// @brief �`�����N��v
		/// @param other �ʂ̃`�����N
		/// @return ��v�Ȃ�Ture
		bool operator==(const Chunk& other) const;

		/// @brief �`�����N�s��v
		/// @param other �ʂ̃`�����N
		/// @return �s��v�Ȃ�Ture
		bool operator!=(const Chunk& other) const;

		/// @brief ���݂̃T�C�Y��Capacity��؂�l��
		void fit();

		/// @brief �G���e�B�e�B��V�K�쐬���A
		/// �`�����N�ɃR���|�[�l���g�f�[�^��ǉ�
		/// ���������ɂ���Ă̓����������炸�댯��
		/// @tparam ...Args �A�[�L�^�C�v�ɂ���^���
		/// @param ...value �A�[�L�^�C�v�ɂ���f�[�^
		/// @return �쐬����Entity
		template<typename ...Args>
		Entity addComponentData(const Args&... value)
		{
			// �����ς��̏ꍇ�͍Ċm��
			if (m_Capacity == m_Size)
			{
				resetMemory(m_Capacity * 2);
			}
			const auto entity = Entity(m_Size);

			addComponentDataImplements(value...);
			++m_Size;
			return entity;
		}

		/// @brief  �G���e�B�e�B��V�K�쐬���A
		/// �`�����N�ɃR���|�[�l���g�f�[�^��ǉ�
		/// ���쐬�����f�[�^�͖���`
		/// @return �쐬����Entity
		Entity createEntity();

		/// @brief Entity�𑼂̃`�����N�Ɉړ�
		/// ��Archetype���Ⴄ�A���肪�����ς��̏ꍇ�̓I�[�o�[�t���[���邩��
		/// @param entity �ړ�������Entity
		/// @param other �ړ���Chunk
		void moveEntity(Entity& entity, Chunk& other);

		/// @brief Entity���폜
		/// @param entity �폜����Entity
		void destroyEntity(const Entity& entity);

		/// @brief ������Entity�Ƀf�[�^�ݒ�
		/// @tparam T �A�[�L�^�C�v�ɂ��邢���ꂩ�̌^���
		/// @param entity �ݒ肷��Entity
		/// @param data �ݒ肷��f�[�^�̒l
		template<typename T>
		void setComponentData(const Entity& entity, const T& data)
		{
			// �ُ�I�� �`�����N�T�C�Y�O �`�����N���Ԉ���Ă邩���H
			if (entity.m_index >= m_Size)
				std::abort();

			using TType = std::remove_const_t<std::remove_reference_t<T>>;
			const auto offset = m_Archetype.getOffset<TType>() * m_Capacity;
			const auto currentIndex = sizeof(TType) * entity.m_index;
			// �f�[�^���R�s�[
			std::memcpy(m_pBegin.get() + offset + currentIndex, &data, sizeof(TType));
		}

		/// @brief �R���|�[�l���g�̔z����擾
		/// @tparam T �A�[�L�^�C�v�ɂ��邢���ꂩ�̌^
		/// @return T�^�̔z��
		template<class T>
		[[nodiscard]] ComponentArray<T> getComponentArray()
		{
			using TType = std::remove_const_t<std::remove_reference_t<T>>;
			auto offset = m_Archetype.getOffset<TType>() * m_Capacity;
			return ComponentArray<T>(reinterpret_cast<TType*>(m_pBegin.get() + offset), m_Size);
		}

		/// @brief �w�肵���^�̃f�[�^���擾
		/// @tparam �R���|�[�l���g�f�[�^�^
		/// @param index �C���f�b�N�X
		/// @return �^�̃|�C���^
		template<class T>
		[[nodiscard]] T* getComponentData(std::uint32_t index)
		{
			using TType = std::remove_const_t<std::remove_reference_t<T>>;
			auto offset = m_Archetype.getOffset<TType>() * m_Capacity;
			return reinterpret_cast<T*>(m_pBegin.get() + offset + m_Archetype.getSize(m_Archetype.getIndex<TType>()) * index);
		}

		/// @brief �w�肵���^���̃f�[�^���擾
		/// @param typeName �^��
		/// @param index �C���f�b�N�X
		/// @return �ėp�|�C���^
		[[nodiscard]] void* getComponentData(std::string_view typeName, std::uint32_t index)
		{
			auto offset = m_Archetype.getOffset(typeName) * m_Capacity;
			return m_pBegin.get() + offset + m_Archetype.getSize(typeName) * index;
		}

		/// @brief �w�肵���n�b�V���l�̃f�[�^���擾
		/// @param typeHash �n�b�V���l
		/// @param index �C���f�b�N�X
		/// @return �ėp�|�C���^
		[[nodiscard]] void* getComponentData(std::size_t typeHash, std::uint32_t index)
		{
			auto offset = m_Archetype.getOffset(typeHash) * m_Capacity;
			return m_pBegin.get() + offset + m_Archetype.getSizeByHash(typeHash) * index;
		}

		/// @brief Archetype���擾
		/// @return archetype
		[[nodiscard]] const Archetype& getArchetype() const
		{
			return m_Archetype;
		}

		/// @brief Chunk�ɓo�^����Ă���f�[�^�̐����擾
		/// @return dataSize
		[[nodiscard]] std::uint32_t getSize() const
		{
			return m_Size;
		}

		/// @brief ���̃`�����N�����g�̃`�����N�Ɍ�������
		/// @param other �}�[�W��
		void marge(Chunk&& other);

		/// @brief Chunk�̃f�[�^��BinaryStream�ɏo��
		/// @param output �o�͐�
		void writeBinaryStream(BinaryStream& output);

		/// @brief Chunk�̃f�[�^��BinaryStream����\�z
		/// @param input ���͌�
		/// @return �ǂݍ��񂾃`�����N
		static Chunk readBinaryStream(BinaryStream& input);

	private:
		/// @brief ���������Ċm��
		/// @param capacity �V�����������T�C�Y
		void resetMemory(std::uint32_t capacity);

		/// @brief �R���|�[�l���g�f�[�^�̒ǉ� 
		/// @tparam Head �擪�^���
		/// @tparam ...Types �^���(��)
		/// @param head �擪�^���
		/// @param ...type �^���(��)
		template<typename Head, typename ...Types>
		void addComponentDataImplements(Head&& head, Types&&... type)
		{
			using HeadType = std::remove_const_t<std::remove_reference_t<Head>>;
			const auto offset = m_Archetype.getOffset<HeadType>() * m_Capacity;
			const auto currentIndex = sizeof(HeadType) * m_Size;
			// �������R�s�[
			std::memcpy(m_pBegin.get() + offset + currentIndex, &head, sizeof(HeadType));
			if constexpr (sizeof...(Types) > 0)
				addComponentDataImplements(type...);
		}

		/// @brief �A�[�L�^�C�v
		Archetype m_Archetype;
		/// @brief �`�����N�̔z��
		std::unique_ptr<std::byte[]> m_pBegin = nullptr;
		/// @brief ���݂̃T�C�Y
		std::uint32_t m_Size = 0;
		/// @brief �ő�T�C�Y
		std::uint32_t m_Capacity = 1;
	};
}