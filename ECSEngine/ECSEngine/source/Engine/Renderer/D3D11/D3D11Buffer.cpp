/*****************************************************************//**
 * \file   D3D11Buffer.h
 * \brief  DirectX11バッファ
 *
 * \author USAMI KOSHI
 * \date   2021/08/24
 *********************************************************************/

#include "D3D11Buffer.h"


 /// @brief コンストラクタ
 /// @param device デバイス
 /// @param id バッファID
 /// @param desc バッファDesc
 /// @param data 初期化データ
D3D11Buffer::D3D11Buffer(ID3D11Device1* device, const BufferID& id, const BufferDesc& desc, const BufferData* data) :
	Buffer(id, desc)
{
	
}
