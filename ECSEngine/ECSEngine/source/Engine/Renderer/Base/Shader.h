/*****************************************************************//**
 * \file   Shader.h
 * \brief  �V�F�[�_�[
 * 
 * \author USAMI KOSHI
 * \date   2021/06/18
 *********************************************************************/
#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <array>

#undef max

/// @brief �V�F�[�_�[ID (�i�[��n�b�V���l)
using ShaderID = std::uint32_t;


/// @enum EShaderStage
/// @brief �V�F�[�_�[�X�e�[�W
enum class EShaderStage : std::uint32_t
{
	VS,		// ���_�V�F�[�_�[
	GS,		// �W�I���g���V�F�[�_�[
	DS,		// �h���C���V�F�[�_�[
	HS,		// �n���V�F�[�_�[
	PS,		// �s�N�Z���V�F�[�_�[
	CS,		// �R���s���[�g�V�F�[�_�[
	MAX,	// �V�F�[�_�[�X�e�[�W��
};

/// @enum EShaderStageFlags
/// @brief �V�F�[�_�[�X�e�[�W�t���O
enum class EShaderStageFlags : std::uint32_t
{
	NONE = 0,
	VS = 1 << 1,
	GS = 1 << 2,
	DS = 1 << 3,
	HS = 1 << 4,
	PS = 1 << 5,
	CS = 1 << 6,
	MAX = std::numeric_limits<std::uint32_t>::max(),
	ALL = MAX,
};

/// @brief ���[�v�p�C���N�������g
EShaderStage operator++(EShaderStage& value); 
/// @brief ���[�v�p�C���N�������g
EShaderStage operator++(EShaderStage& value, int); 
/// @brief �V�F�[�_�[�X�e�[�W�t���O�p�_���a
std::uint32_t operator|(const EShaderStageFlags& l, const EShaderStageFlags& r);
/// @brief �V�F�[�_�[�X�e�[�W�t���O�p�_���a
std::uint32_t operator|(const std::uint32_t& l, const EShaderStageFlags& r);

/// @brief 
/// @param shaderStage 
/// @return 
constexpr EShaderStageFlags ConvertShaderStage2Flags(const EShaderStage& shaderStage); 
/// @brief �V�F�[�_�[�X�e�[�W���t���O�Ɋ܂܂�Ă��邩
/// @details Flags��AND�����
/// @param shaderStageFlags �V�F�[�_�[�X�e�[�W�t���O
/// @param shaderStage �V�F�[�_�[�X�e�[�W
/// @return �܂܂�Ă���Ȃ�TRUE
bool hasStaderStage(const std::uint32_t& shaderStageFlags, const EShaderStage& shaderStage);


/// @brief �V�F�[�_�[�����p�\����
struct ShaderDesc
{
	std::uint32_t	m_stages;	// �V�F�[�_�[�X�e�[�W�t���O
	std::string		m_name;		// �V�F�[�_�[��
	// �V�F�[�_�}�N��
};

/// @class Shader
/// @brief �V�F�[�_
class Shader
{
public:
	/// @brief �R���X�g���N�^
	Shader(ShaderDesc shaderDesc) :
		m_id(std::numeric_limits<ShaderID>::max()),
		m_desc(shaderDesc)
	{
	}

	/// @brief �f�X�g���N�^
	virtual ~Shader() = default;

public:
	/// @brief ���̓��C�A�E�g���\����
	struct InputLayoutVariable
	{
		enum class FormatSize {
			UNKNOWN,
			R32,
			R32G32,
			R32G32B32,
			R32G32B32A32,
			MAX,
		};
		enum class FormatType {
			UNKNOWN,
			UINT32,
			SINT32,
			FLOAT32,
			MAX,
		};
		enum class Format;
		std::string semanticName;		// �Z�}���e�B�b�N��		��:TEXCOOD
		std::uint32_t semanticIndex;	// �Z�}���e�B�b�N�ԍ�	��:TEXCOOD[0]��
		std::size_t offset;				// �ϐ��I�t�Z�b�g
		FormatSize formatSize;			// �t�H�[�}�b�g�T�C�Y	��:R32=1,R32B32=2, MAX=5
		FormatType formatType;			// �t�H�[�}�b�g�^�C�v	��:uint32=1,sint32=2, MAX=5
		InputLayoutVariable() :
			semanticName(),
			semanticIndex(0),
			offset(0),
			formatSize(FormatSize::UNKNOWN),
			formatType(FormatType::UNKNOWN) {}
	};

	/// @brief ���̓��C�A�E�g��񃊃X�g
	std::vector<InputLayoutVariable> m_inputLayoutVariableList;

	/// @brief CBuffer�̕ϐ����
	struct CBufferVariable
	{
		std::string name;	// �ϐ���
		std::size_t size;	// �^�T�C�Y
		std::size_t offset;	// �ϐ��I�t�Z�b�g
	};
	/// @brief CBuffer�̍\���̃��C�A�E�g���
	struct CBufferLayout
	{
		std::string		name;	// cbuffer�錾��
		std::uint32_t	slot;	// ���W�X�^�X���b�g
		std::size_t		size;	// cbuffer�T�C�Y
		std::vector<CBufferVariable> variables;	// �ϐ��f�[�^
		CBufferLayout() :name(), slot(0), size(0), variables() {}
		CBufferLayout(const std::uint32_t& slot, const std::string& name, const std::size_t& size) :
			slot(slot), name(name), size(size), variables() {}
	};
	/// @brief �S�X�e�[�W��CBuffer���C�A�E�g
	std::array<std::unordered_map<std::uint32_t, CBufferLayout>,
		static_cast<size_t>(EShaderStage::MAX)> m_cbufferLayouts;
	/// @brief CBuffer�ϐ��̃f�t�H���g�l(�������q�t���l)
	std::unordered_map<std::string, std::unique_ptr<std::byte[]>> m_cbufferDefaults;

	/// @brief �e�N�X�`�����\�[�X�o�C���h���
	struct TextureBindData
	{
		std::string name;
		EShaderStage stage;
		std::uint32_t slot;
	};
	/// @brief �T���v�����\�[�X�o�C���h���
	struct SamplerBindData
	{
		std::string name;
		EShaderStage stage;
		std::uint32_t slot;
	};

	/// @brief �S�X�e�[�W�̃e�N�X�`�����\�[�X���
	std::array<std::unordered_map<std::uint32_t, TextureBindData>,
		static_cast<size_t>(EShaderStage::MAX)> m_textureBindDatas;
	/// @brief �S�X�e�[�W�̃T���v�����\�[�X���
	std::array<std::unordered_map<std::uint32_t, SamplerBindData>,
		static_cast<size_t>(EShaderStage::MAX)> m_samplerBindDatas;

public:
	/// @brief �V�F�[�_ID
	ShaderID	m_id;
	/// @brief �V�F�[�_�[�������
	ShaderDesc  m_desc;
	/// @brief �}�N���̃n�b�V���l
	/// 
};
