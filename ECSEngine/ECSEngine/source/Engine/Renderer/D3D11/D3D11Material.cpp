/*****************************************************************//**
 * \file   D3D11Material.h
 * \brief  DirectX11マテリアル
 *
 * \author USAMI KOSHI
 * \date   2021/06/21
 *********************************************************************/

#include "D3D11Material.h"


/// @brief コンストラクタ
/// @param device デバイス
/// @param id マテリアルID
/// @param name マテリアル名
/// @param shader シェーダ
D3D11Material::D3D11Material(ID3D11Device1* device, const MaterialID& id,
	const std::string& name, const Shader& shader) :
	Material(id,name,shader)
{
	// コンスタントバッファの初期化
	D3D11_BUFFER_DESC d3dDesc = {};
	d3dDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	d3dDesc.Usage			= D3D11_USAGE_DYNAMIC;
	d3dDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	d3dDesc.MiscFlags		= 0; // STRUCTURED_BUFFERならD3D11_RESOURCE_MISC_BUFFER_STRUCTURED
	//d3dDesc.StructureByteStride = 0;

	// コンスタントバッファの確保(GPU)
	for (EShaderStage stage = EShaderStage::VS; stage < EShaderStage::MAX; ++stage)
	{
		//--- CBuffer確保
		auto stageIndex = static_cast<size_t>(stage);
		for (auto& cbData : m_cbufferData[stageIndex])
		{
			// CBufferサイズ
			d3dDesc.ByteWidth	= cbData.second.size;
			// 生成
			CHECK_FAILED(device->CreateBuffer(&d3dDesc, nullptr, 
				m_d3dCbuffer[stageIndex][cbData.first].GetAddressOf()));
		}
	}
}
