/*****************************************************************//**
 * \file   D3D11Material.h
 * \brief  DirectX11マテリアル
 *
 * \author USAMI KOSHI
 * \date   2021/06/21
 *********************************************************************/

#include "D3D11Material.h"
#include "D3D11Utility.h"


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
	d3dDesc.Usage			= D3D11_USAGE_DEFAULT;
	d3dDesc.CPUAccessFlags	= 0;
	d3dDesc.MiscFlags		= 0; // STRUCTURED_BUFFERならD3D11_RESOURCE_MISC_BUFFER_STRUCTURED
	//d3dDesc.StructureByteStride = 0;
	// Map()/Unmap()でリソースを更新する場合は↓を用いる
	//d3dDesc.Usage = D3D11_USAGE_DYNAMIC;
	//d3dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 初期化データ
	D3D11_SUBRESOURCE_DATA initData = {};

	// コンスタントバッファの確保(GPU)
	for (ShaderStage stage = ShaderStage::VS; stage < ShaderStage::MAX; ++stage)
	{
		//--- CBuffer確保
		auto stageIndex = static_cast<size_t>(stage);
		for (auto& cbData : m_cbufferData[stageIndex])
		{
			// CBufferサイズ
			d3dDesc.ByteWidth	= cbData.second.size;
			// 初期化データ
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = cbData.second.data.get();
			// 生成
			CHECK_FAILED(device->CreateBuffer(&d3dDesc, &initData,
				m_d3dCbuffer[stageIndex][cbData.first].GetAddressOf()));
		}
	}
}
