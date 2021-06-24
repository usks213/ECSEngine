/*****************************************************************//**
 * \file   Shader.h
 * \brief  シェーダー
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

/// @brief シェーダーID (格納先ハッシュ値)
using ShaderID = std::uint32_t;


/// @enum EShaderStage
/// @brief シェーダーステージ
enum class EShaderStage : std::uint32_t
{
	VS,		// 頂点シェーダー
	GS,		// ジオメトリシェーダー
	DS,		// ドメインシェーダー
	HS,		// ハルシェーダー
	PS,		// ピクセルシェーダー
	CS,		// コンピュートシェーダー
	MAX,	// シェーダーステージ数
};

/// @enum EShaderStageFlags
/// @brief シェーダーステージフラグ
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

/// @brief ループ用インクリメント
EShaderStage operator++(EShaderStage& value); 
/// @brief ループ用インクリメント
EShaderStage operator++(EShaderStage& value, int); 
/// @brief シェーダーステージフラグ用論理和
std::uint32_t operator|(const EShaderStageFlags& l, const EShaderStageFlags& r);
/// @brief シェーダーステージフラグ用論理和
std::uint32_t operator|(const std::uint32_t& l, const EShaderStageFlags& r);

/// @brief 
/// @param shaderStage 
/// @return 
constexpr EShaderStageFlags ConvertShaderStage2Flags(const EShaderStage& shaderStage); 
/// @brief シェーダーステージがフラグに含まれているか
/// @details FlagsとANDを取る
/// @param shaderStageFlags シェーダーステージフラグ
/// @param shaderStage シェーダーステージ
/// @return 含まれているならTRUE
bool hasStaderStage(const std::uint32_t& shaderStageFlags, const EShaderStage& shaderStage);


/// @brief シェーダー生成用構造体
struct ShaderDesc
{
	std::uint32_t	m_stages;	// シェーダーステージフラグ
	std::string		m_name;		// シェーダー名
	// シェーダマクロ
};

/// @class Shader
/// @brief シェーダ
class Shader
{
public:
	/// @brief コンストラクタ
	Shader(ShaderDesc shaderDesc) :
		m_id(std::numeric_limits<ShaderID>::max()),
		m_desc(shaderDesc)
	{
	}

	/// @brief デストラクタ
	virtual ~Shader() = default;

public:
	/// @brief 入力レイアウト情報構造体
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
		std::string semanticName;		// セマンティック名		例:TEXCOOD
		std::uint32_t semanticIndex;	// セマンティック番号	例:TEXCOOD[0]←
		std::size_t offset;				// 変数オフセット
		FormatSize formatSize;			// フォーマットサイズ	例:R32=1,R32B32=2, MAX=5
		FormatType formatType;			// フォーマットタイプ	例:uint32=1,sint32=2, MAX=5
		InputLayoutVariable() :
			semanticName(),
			semanticIndex(0),
			offset(0),
			formatSize(FormatSize::UNKNOWN),
			formatType(FormatType::UNKNOWN) {}
	};

	/// @brief 入力レイアウト情報リスト
	std::vector<InputLayoutVariable> m_inputLayoutVariableList;

	/// @brief CBufferの変数情報
	struct CBufferVariable
	{
		std::string name;	// 変数名
		std::size_t size;	// 型サイズ
		std::size_t offset;	// 変数オフセット
	};
	/// @brief CBufferの構造体レイアウト情報
	struct CBufferLayout
	{
		std::string		name;	// cbuffer宣言名
		std::uint32_t	slot;	// レジスタスロット
		std::size_t		size;	// cbufferサイズ
		std::vector<CBufferVariable> variables;	// 変数データ
		CBufferLayout() :name(), slot(0), size(0), variables() {}
		CBufferLayout(const std::uint32_t& slot, const std::string& name, const std::size_t& size) :
			slot(slot), name(name), size(size), variables() {}
	};
	/// @brief 全ステージのCBufferレイアウト
	std::array<std::unordered_map<std::uint32_t, CBufferLayout>,
		static_cast<size_t>(EShaderStage::MAX)> m_cbufferLayouts;
	/// @brief CBuffer変数のデフォルト値(初期化子付き値)
	std::unordered_map<std::string, std::unique_ptr<std::byte[]>> m_cbufferDefaults;

	/// @brief テクスチャリソースバインド情報
	struct TextureBindData
	{
		std::string name;
		EShaderStage stage;
		std::uint32_t slot;
	};
	/// @brief サンプラリソースバインド情報
	struct SamplerBindData
	{
		std::string name;
		EShaderStage stage;
		std::uint32_t slot;
	};

	/// @brief 全ステージのテクスチャリソース情報
	std::array<std::unordered_map<std::uint32_t, TextureBindData>,
		static_cast<size_t>(EShaderStage::MAX)> m_textureBindDatas;
	/// @brief 全ステージのサンプラリソース情報
	std::array<std::unordered_map<std::uint32_t, SamplerBindData>,
		static_cast<size_t>(EShaderStage::MAX)> m_samplerBindDatas;

public:
	/// @brief シェーダID
	ShaderID	m_id;
	/// @brief シェーダー生成情報
	ShaderDesc  m_desc;
	/// @brief マクロのハッシュ値
	/// 
};
