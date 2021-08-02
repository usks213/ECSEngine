/*****************************************************************//**
 * \file   Chunk.cpp
 * \brief  チャンク
 * 
 * \author USAMI KOSHI
 * \date   2021/06/27
 *********************************************************************/

#include "Chunk.h"
using namespace ecs;



 /// @brief アーキタイプから生成
 /// @param archetype  アーキタイプ
 /// @param capacity チャンクの容量
 /// @return チャンク
Chunk Chunk::create(const Archetype& archetype, const std::uint32_t capacity)
{
	Chunk result;
	result.m_Archetype = archetype;
	result.m_Capacity = capacity;
	result.m_pBegin = std::make_unique<std::byte[]>(
		result.m_Capacity * result.m_Archetype.getArchetypeMemorySize());
	return result;
}

/// @brief チャンク一致
/// @param other 別のチャンク
/// @return 一致ならTure
bool Chunk::operator==(const Chunk& other) const
{
	return m_Archetype == other.m_Archetype;
}

/// @brief チャンク不一致
/// @param other 別のチャンク
/// @return 不一致ならTure
bool Chunk::operator!=(const Chunk& other) const
{
	return m_Archetype != other.m_Archetype;
}

/// @brief 現在のサイズでCapacityを切り詰め
void Chunk::fit()
{
	resetMemory(m_Size);
}

/// @brief  エンティティを新規作成し、
/// チャンクにコンポーネントデータを追加
/// ※作成したデータは未定義
/// @return 作成したEntity
Entity Chunk::createEntity()
{
	// いっぱいの場合は再確保
	if (m_Capacity == m_Size)
	{
		resetMemory(m_Capacity * 2);
	}

	const auto entity = Entity(m_Size);
	++m_Size;
	return entity;
}

/// @brief Entityを他のチャンクに移動
/// ※Archetypeが違う、相手がいっぱいの場合はオーバーフローするかも
/// @param entity 移動させるEntity
/// @param other 移動先Chunk
void Chunk::moveEntity(Entity& entity, Chunk& other)
{
	const auto newEntity = other.createEntity();

	// ※Archetypeが違う、相手がいっぱいの場合はオーバーフローするかも
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

/// @brief Entityを削除
/// @param entity 削除するEntity
void Chunk::destroyEntity(const Entity& entity)
{
	for (size_t i = 0; i < m_Archetype.getArchetypeSize(); ++i)
	{
		//const auto offset = m_Archetype.getOffsetByIndex(i) * m_Capacity;
		//const auto currentIndex = offset + m_Archetype.getSize(i) * entity.m_index;
		//// 後ろを切り詰める
		//std::memmove(
		//	m_pBegin.get() + currentIndex,
		//	m_pBegin.get() + currentIndex + m_Archetype.getSize(i),
		//	m_Archetype.getSize(i) * (m_Size - entity.m_index - 1));

		const auto offset = m_Archetype.getOffsetByIndex(i) * m_Capacity;
		const auto currentIndex = offset + m_Archetype.getSize(i) * entity.m_index;
		const auto endIndex = offset + m_Archetype.getSize(i) * (m_Size - 1);
		// 最後尾を上書き
		std::memmove(
			m_pBegin.get() + currentIndex,
			m_pBegin.get() + endIndex,
			m_Archetype.getSize(i));
	}
	--m_Size;
}

/// @brief 他のチャンクを自身のチャンクに結合する
/// @param other マージ元
void Chunk::marge(Chunk&& other)
{
	// アーキタイプが同じでなければマージできない
	if (m_Archetype != other.m_Archetype)
		return;

	const auto needSize = m_Size + other.m_Size;
	// メモリが足りない
	if (m_Capacity < needSize)
	{
		resetMemory(needSize);
		m_Capacity = needSize;
	}

	// コピー
	for (size_t i = 0; i < m_Archetype.getArchetypeSize(); ++i)
	{
		const auto archetypeOffset = m_Archetype.getOffsetByIndex(i);
		const auto archetypeSize = m_Archetype.getSize(i);
		const auto offset = archetypeOffset * m_Capacity + archetypeSize * m_Size;
		const auto otherOffset = archetypeOffset * other.m_Capacity;
		// 相手のデータを自身の後ろに結合
		std::memcpy(m_pBegin.get() + offset,
			other.m_pBegin.get() + otherOffset, archetypeSize * other.m_Size);
	}
	// サイズ更新
	m_Size += other.m_Size;

	// 移し終わったチャンクはリセット
	other.m_pBegin.reset();
	other.m_Size = 0;
	other.m_Capacity = 0;
}

/// @brief ChunkのデータをBinaryStreamに出力
/// @param output 出力先
void Chunk::writeBinaryStream(BinaryStream& output)
{
	// チャンクの整理
	fit();

	// アーキタイプのサイズ(数)を書き出し
	const auto archetypeSize = m_Archetype.getArchetypeSize();
	output.write(archetypeSize);

	// 各ハッシュ値とメモリサイズの書き出し
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

	// コンポーネントデータの書き出し
	output.write(m_Size);	// データサイズ
	output.write(m_pBegin.get(), m_Archetype.getArchetypeMemorySize() * m_Size);	// データ
}

/// @brief ChunkのデータをBinaryStreamから構築
/// @param input 入力元
/// @return 読み込んだチャンク
Chunk Chunk::readBinaryStream(BinaryStream& input)
{
	Chunk result;

	// アーキタイプの読み込み
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

	// コンポーネントデータの読み込み
	input.read(&result.m_Size);
	result.m_Capacity = result.m_Size;

	std::size_t memorySize = result.m_Capacity * result.m_Archetype.getArchetypeMemorySize();
	result.m_pBegin = std::make_unique<std::byte[]>(memorySize);
	input.read(result.m_pBegin.get(), memorySize);

	return result;
}

/// @brief メモリを再確保
/// @param capacity 新しいメモリサイズ
void Chunk::resetMemory(std::uint32_t capacity)
{
	// 新しいメモリ
	auto pWork = std::make_unique<std::byte[]>(m_Archetype.getArchetypeMemorySize() * capacity);

	std::size_t offsetBase = 0;

	// コピー
	for (std::size_t i = 0; i < m_Archetype.getArchetypeSize(); ++i)
	{
		const auto offset = offsetBase * m_Capacity;
		const auto newOffset = offsetBase * capacity;
		// 末尾にコピー
		std::memcpy(pWork.get() + newOffset, m_pBegin.get() + offset, m_Archetype.getSize(i) * m_Size);

		offsetBase += m_Archetype.getSize(i);
	}

	m_Capacity = capacity;
	m_pBegin = std::move(pWork);
}