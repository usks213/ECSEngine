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
	if (FAILED(hr)) {														\
		static char szResult[256];                                          \
		_com_error  err(hr);                                                \
		LPCTSTR     errMsg = err.ErrorMessage();							\
        sprintf_s(szResult, sizeof(szResult), "[ERROR] HRESULT: %s",errMsg);\
		MessageBox(nullptr, szResult, "Err", MB_ICONSTOP);					\
		throw std::exception(szResult);     								\
	}

