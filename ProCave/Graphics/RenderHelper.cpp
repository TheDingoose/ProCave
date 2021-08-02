#include "RenderHelper.h"

void CheckError(HRESULT Res, const char* msg) {
#ifdef _DEBUG 
	if (Res != S_OK) {
		spdlog::error(msg);
		OutputDebugStringA(msg);
		throw(1);
	}
#endif
	return;
}

void CheckError(HRESULT Res, ID3DBlob* msg) {
#ifdef _DEBUG 
	if (Res != S_OK) {
		spdlog::error((const char*)msg->GetBufferPointer());
		OutputDebugStringA((const char*)msg->GetBufferPointer());
		throw(1);
	}
#endif
	return;
}