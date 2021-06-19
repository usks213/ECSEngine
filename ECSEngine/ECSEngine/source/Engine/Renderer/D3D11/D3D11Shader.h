/*****************************************************************//**
 * \file   D3D11Shader.h
 * \brief  Directx11シェーダ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/19
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/ShaderBase.h>
#include <Engine/Renderer/Base/D3DUtility.h>
#include <d3d11_1.h>
#include <d3d11shader.h>


/// @class D3D11Shader
/// @brief DirectX11シェーダ
class D3D11Shader : public ShaderBase
{
public:
	/// @brief  コンストラクタ
	/// @param device デバイス
	/// @param desc シェーダ情報
	D3D11Shader(ID3D11Device1* device, ShaderDesc desc);
	/// @brief デストラクタ
	~D3D11Shader() = default;

public:
	/// @brief シェーダデータ
	union {
		struct {
			ID3D11VertexShader*		vs;
			ID3D11GeometryShader*	gs;
			ID3D11DomainShader*		ds;
			ID3D11HullShader*		hs;
			ID3D11PixelShader*		ps;
			ID3D11ComputeShader*	cs;
		};
		ID3D11DeviceChild* shaders[static_cast<size_t>(EShaderStage::MAX)];
	};
	/// @brief 入力レイアウト
	ComPtr<ID3D11InputLayout>				m_inputLayout;

	/// @brief 入力レイアウト情報構造体
	struct InputLayoutVariable
	{
		enum class FormatType {
			R32,
			R32G32,
			R32G32B32,
			R32G32B32A32,
			MAX,
		};
		std::string semanticName;		// セマンティック名		例:TEXCOOD
		std::uint32_t semanticIndex;	// セマンティック番号	例:TEXCOOD[0]←
		FormatType formatType;			// フォーマットタイプ	例:R32=0,R32B32=1, MAX=4
		DXGI_FORMAT	format;				// フォーマット
		InputLayoutVariable():
			semanticName(),
			semanticIndex(0),
			formatType(FormatType::MAX),
			format(DXGI_FORMAT_UNKNOWN){}
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
		CBufferLayout(const std::uint32_t& slot, const std::string& name, const std::size_t& size) :
			slot(slot), name(name), size(size){}
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

private:
	/// @brief シェーダのスマートポインタ(解放用)
	std::vector<ComPtr<ID3D11DeviceChild>>	m_comShaders;

private:
	/// @brief シェーダオブジェクト生成
	/// @param device デバイス
	/// @param stage シェーダステージ
	/// @param blob コンパイルデータ
	void createShaderObjct(ID3D11Device1* device, const EShaderStage& stage, ComPtr<ID3DBlob>& blob);

private:

};

