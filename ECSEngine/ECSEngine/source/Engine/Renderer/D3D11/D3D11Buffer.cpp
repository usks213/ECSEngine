/*****************************************************************//**
 * \file   D3D11Buffer.h
 * \brief  DirectX11�o�b�t�@
 *
 * \author USAMI KOSHI
 * \date   2021/08/24
 *********************************************************************/

#include "D3D11Buffer.h"


 /// @brief �R���X�g���N�^
 /// @param device �f�o�C�X
 /// @param id �o�b�t�@ID
 /// @param desc �o�b�t�@Desc
 /// @param data �������f�[�^
D3D11Buffer::D3D11Buffer(ID3D11Device1* device, const BufferID& id, const BufferDesc& desc, const BufferData* data) :
	Buffer(id, desc)
{
	
}
