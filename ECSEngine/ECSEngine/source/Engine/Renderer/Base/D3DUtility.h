/*****************************************************************//**
 * \file   D3DUtility.h
 * \brief  DirectX11”Ä—p
 * 
 * \author USAMI KOSHI
 * \date   2021/06/17
 *********************************************************************/
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <memory>

#include <comdef.h>
#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include "CommonState.h"


#define CHECK_FAILED(hr)													\
    if (FAILED(hr)) {                                                                                                                 \
        static char szResult[256];                                                                                                    \
        _com_error  err(hr);                                                                                                          \
        LPCTSTR     errMsg = err.ErrorMessage();                                                                                      \
        sprintf_s(szResult, sizeof(szResult), "[ERROR] HRESULT: %08X %s#L%d\n%s", static_cast<UINT>(hr), __FILE__, __LINE__, errMsg); \
        MessageBox(nullptr, szResult, "ERROR", MB_OK | MB_ICONERROR);                                                                 \
        throw std::exception(szResult);                                                                                               \
    }

