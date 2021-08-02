/*****************************************************************//**
 * \file   Chunk.cpp
 * \brief  �`�����N
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "Chunk.h"
using namespace ecs;



 /// @brief �A�[�L�^�C�v���琶��
 /// @param archetype  �A�[�L�^�C�v
 /// @param capacity �`�����N�̗e��
 /// @return �`�����N
Chunk Chunk::create(const Archetype& archetype, const std::uint32_t capacity)
{
	Chunk result;
	result.m_Archetype = archetype;
	result.m_Capacity = capacity;
	result.m_pBegin = std::make_unique<std::byte[]>(
		result.m_Capacity * result.m_Archetype.getArchetypeMemorySize());
	return result;
}

/// @brief �`�����N��v
/// @param other �ʂ̃`�����N
/// @return ��v�Ȃ�Ture
bool Chunk::operator==(const Chunk& other) const
{
	return m_Archetype == other.m_Archetype;
}

/// @brief �`�����N�s��v
/// @param other �ʂ̃`�����N
/// @return �s��v�Ȃ�Ture
bool Chunk::operator!=(const Chunk& other) const
{
	return m_Archetype != other.m_Archetype;
}

/// @brief ���݂̃T�C�Y��Capacity��؂�l��
void Chunk::fit()
{
	resetMemory(m_Size);
}

/// @brief  �G���e�B�e�B��V�K�쐬���A
/// �`�����N�ɃR���|�[�l���g�f�[�^��ǉ�
/// ���쐬�����f�[�^�͖���`
/// @return �쐬����Entity
Entity Chunk::createEntity()
{
	// �����ς��̏ꍇ�͍Ċm��
	if (m_Capacity == m_Size)
	{
		resetMemory(m_Capacity * 2);
	}

	const auto entity = Entity(m_Size);
	++m_Size;
	return entity;
}

/// @brief Entity�𑼂̃`�����N�Ɉړ�
/// ��Archetype���Ⴄ�A���肪�����ς��̏ꍇ�̓I�[�o�[�t���[���邩��
/// @param entity �ړ�������Entity
/// @param other �ړ���Chunk
void Chunk::moveEntity(Entity& entity, Chunk& other)
{
	const auto newEntity = other.createEntity();

	// ��Archetype���Ⴄ�A���肪�����ς��̏ꍇ�̓I�[�o�[�t���[���邩��
	for (size_t i = 0; i < m_Archetype.getArchetypeSize(); ++i)
	{
		const auto currentOffset = m_Archetype.getOffsetByIndex(i) * m_Capacity;
		const auto currentIndex = currentOffset + m_Archetype.getSize(i) * entity.m_index;

		const auto otherI = other.m_Archetype.getIndexByTypeInfo(m_Archetype.getTypeInfo(i));
		const auto otherOffset = other.m_Archetype.getOffsetByIndex(otherI) * other.m_Capacity;
		const auto otherIndex = otherOffset + other.m_Archetype.getSize(otherI) * newEntity.m_index;

		std::memcpy(other.m_pBegin.get() + otherIndex, m_pBegin.get() + currentIndex, m_Archetype.getSize(i));
	}

	destroyEntity(entity);
	entity = newEntity;
}

/// @brief Entity���폜
/// @param entity �폜����Entity
void Chunk::destroyEntity(const Entity& entity)
{
	for (size_t i = 0; i < m_Archetype.getArchetypeSize(); ++i)
	{
		//const auto offset = m_Archetype.getOffsetByIndex(i) * m_Capacity;
		//const auto currentIndex = offset + m_Archetype.getSize(i) * entity.m_index;
		//// ����؂�l�߂�
		//std::memmove(
		//	m_pBegin.get() + currentIndex,
		//	m_pBegin.get() + currentIndex + m_Archetype.getSize(i),
		//	m_Archetype.getSize(i) * (m_Size - entity.m_index - 1));

		const auto offset = m_Archetype.getOffsetByIndex(i) * m_Capacity;
		const auto currentIndex = offset + m_Archetype.getSize(i) * entity.m_index;
		const auto endIndex = offset + m_Archetype.getSize(i) * (m_Size - 1);
		// �Ō�����㏑��
		std::memmove(
			m_pBegin.get() + currentIndex,
			m_pBegin.get() + endIndex,
			m_Archetype.getSize(i));
	}
	--m_Size;
}

/// @brief ���̃`�����N�����g�̃`�����N�Ɍ�������
/// @param other �}�[�W��
void Chunk::marge(Chunk&& other)
{
	// �A�[�L�^�C�v�������łȂ���΃}�[�W�ł��Ȃ�
	if (m_Archetype != other.m_Archetype)
		return;

	const auto needSize = m_Size + other.m_Size;
	// ������������Ȃ�
	if (m_Capacity < needSize)
	{
		resetMemory(needSize);
		m_Capacity = needSize;
	}

	// �R�s�[
	for (size_t i = 0; i < m_Archetype.getArchetypeSize(); ++i)
	{
		const auto archetypeOffset = m_Archetype.getOffsetByIndex(i);
		const auto archetypeSize = m_Archetype.getSize(i);
		const auto offset = archetypeOffset * m_Capacity + archetypeSize * m_Size;
		const auto otherOffset = archetypeOffset * other.m_Capacity;
		// ����̃f�[�^�����g�̌��Ɍ���
		std::memcpy(m_pBegin.get() + offset,
			other.m_pBegin.get() + otherOffset, archetypeSize * other.m_Size);
	}
	// �T�C�Y�X�V
	m_Size += other.m_Size;

	// �ڂ��I������`�����N�̓��Z�b�g
	other.m_pBegin.reset();
	other.m_Size = 0;
	other.m_Capacity = 0;
}

/// @brief Chunk�̃f�[�^��BinaryStream�ɏo��
/// @param output �o�͐�
void Chunk::writeBinaryStream(BinaryStream& output)
{
	// �`�����N�̐���
	fit();

	// �A�[�L�^�C�v�̃T�C�Y(��)�������o��
	const auto archetypeSize = m_Archetype.getArchetypeSize();
	output.write(archetypeSize);

	// �e�n�b�V���l�ƃ������T�C�Y�̏����o��
	for (size_t i = 0; i < archetypeSize; ++i)
	{
		const auto typeHash = m_Archetype.getTypeInfo(i).getHash();
		const auto typeSize = m_Archetype.getTypeInfo(i).getSize();
		const auto typeName = m_Archetype.getTypeInfo(i).getName();
		const auto nameSize = typeName.size();
		output.write(typeHash);
		output.write(typeSize);
		output.write(nameSize);
		output.write(typeName.data(), nameSize);
	}

	// �R���|�[�l���g�f�[�^�̏����o��
	output.write(m_Size);	// �f�[�^�T�C�Y
	output.write(m_pBegin.get(), m_Archetype.getArchetypeMemorySize() * m_Size);	// �f�[�^
}

/// @brief Chunk�̃f�[�^��BinaryStream����\�z
/// @param input ���͌�
/// @return �ǂݍ��񂾃`�����N
Chunk Chunk::readBinaryStream(BinaryStream& input)
{
	Chunk result;

	// �A�[�L�^�C�v�̓ǂݍ���
	std::size_t archetypeSize;
	input.read(&archetypeSize);
	for (size_t i = 0; i < archetypeSize; ++i)
	{
		std::size_t typeHash, typeSize, nameSize;
		input.read(&typeHash);
		input.read(&typeSize);
		input.read(&nameSize);
		char* name = new char[nameSize];
		input.read(name, nameSize);
		result.m_Archetype.addType(typeHash, typeSize, name);
		delete[] name;
	}

	// �R���|�[�l���g�f�[�^�̓ǂݍ���
	input.read(&result.m_Size);
	result.m_Capacity = result.m_Size;

	std::size_t memorySize = result.m_Capacity * result.m_Archetype.getArchetypeMemorySize();
	result.m_pBegin = std::make_unique<std::byte[]>(memorySize);
	input.read(result.m_pBegin.get(), memorySize);

	return result;
}

/// @brief ���������Ċm��
/// @param capacity �V�����������T�C�Y
void Chunk::resetMemory(std::uint32_t capacity)
{
	// �V����������
	auto pWork = std::make_unique<std::byte[]>(m_Archetype.getArchetypeMemorySize() * capacity);

	std::size_t offsetBase = 0;

	// �R�s�[
	for (std::size_t i = 0; i < m_Archetype.getArchetypeSize(); ++i)
	{
		const auto offset = offsetBase * m_Capacity;
		const auto newOffset = offsetBase * capacity;
		// �����ɃR�s�[
		std::memcpy(pWork.get() + newOffset, m_pBegin.get() + offset, m_Archetype.getSize(i) * m_Size);

		offsetBase += m_Archetype.getSize(i);
	}

	m_Capacity = capacity;
	m_pBegin = std::move(pWork);
}