#pragma once
#include <d3d11.h>

#include "spdlog/spdlog.h"

void CheckError(HRESULT Res, const char* msg);
void CheckError(HRESULT Res, ID3DBlob* msg);