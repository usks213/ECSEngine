/*****************************************************************//**
 * \file   D3D11Utility.h
 * \brief  DirectX11Utility
 * 
 * \author USAMI KOSHI
 * \date   2021/06/21
 *********************************************************************/
#pragma once

#include <comdef.h>
#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#include <d3d11_1.h>

#include <Engine/Renderer/Base/CommonState.h>
#include <Engine/Renderer/Base/LightData.h>
#include <Engine/Utility/Mathf.h>


#define CHECK_FAILED(hr)													\
    if (FAILED(hr)) {                                                                                                                 \
        static char szResult[256];                                                                                                    \
        _com_error  err(hr);                                                                                                          \
        LPCTSTR     errMsg = err.ErrorMessage();                                                                                      \
        sprintf_s(szResult, sizeof(szResult), "[ERROR] HRESULT: %08X %s#L%d\n%s", static_cast<UINT>(hr), __FILE__, __LINE__, errMsg); \
        MessageBox(nullptr, szResult, "ERROR", MB_OK | MB_ICONERROR);                                                                 \
        throw std::exception(szResult);                                                                                               \
    }

namespace D3D11Util
{
    D3D11_USAGE getD3D11Usage(Usage usage);
    UINT32 getD3D11BindFlags(BindFlags flags);
    UINT32 getD3D11CPUAccessFlags(CPUAccessFlags flags);
    UINT32 getD3D11MiscFlags(MiscFlags flags);
    D3D11_PRIMITIVE_TOPOLOGY getD3D11PrimitiveTopology(PrimitiveTopology topology);
    DXGI_FORMAT getDXGIFormat(TextureFormat format);

}
