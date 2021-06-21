/*****************************************************************//**
 * \file   D3D11Shader.h
 * \brief  Directx11シェーダ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/19
 *********************************************************************/
#pragma once

#include <Engine/Renderer/Base/Shader.h>
#include "D3D11Utility.h"
#include <d3d11shader.h>


/// @class D3D11Shader
/// @brief DirectX11シェーダ
class D3D11Shader final : public Shader
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

private:
	/// @brief シェーダのスマートポインタ(解放用)
	std::vector<ComPtr<ID3D11DeviceChild>>	m_comShaders;

private:
	/// @brief シェーダオブジェクト生成
	/// @param device デバイス
	/// @param stage シェーダステージ
	/// @param blob コンパイルデータ
	void createShaderObjct(ID3D11Device1* device, const EShaderStage& stage, ComPtr<ID3DBlob>& blob);

};

