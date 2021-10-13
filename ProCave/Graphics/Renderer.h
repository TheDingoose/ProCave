#pragma once
#include <SH.h>

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <vector>
//#include <d3dx11.h>
//#include <d3dx10.h>
//#include <xnamath.h>

#include "General/Transform.h"
#include "Rendering/Vertex.h"
#include "Rendering/Model.h"

#include "Rendering/DevUIDriver.h"
#include "Tools/MarchCubeSettings.h"

using namespace DirectX;

//constexpr float MCSize = 1.f;
//constexpr unsigned int MCFieldSize = 76;

//float4x4 WVP;
//float4 PlayerPos;
//float CubeSize;
//float LightStrength;
//float4 SampleMod;
//float4 SampleOffset;
//float Time;
namespace reactphysics3d {
class DebugRenderer;
}

struct cbPerObject
{
	XMMATRIX  WVP;
	XMFLOAT4 PlayerPos = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4 SampleMod = MarchCubeSettings::get()->SampleMod;
	XMFLOAT4 SampleOffset = MarchCubeSettings::get()->SampleOffset;
	float CubeSize = MarchCubeSettings::get()->CubeSize;
	float LightStrength = MarchCubeSettings::get()->LightStrength;
	float Time = MarchCubeSettings::get()->Time;
	float DensityLimit = MarchCubeSettings::get()->DensityLimit;
};

struct CubeConstBuff {
	int triTable[256 * 16];
};

struct ModelBuffers {
	ID3D11Buffer* IndexBuffer;
	UINT IndexSize;
	ID3D11Buffer* VertexBuffer;
	cTransform Transform;
};


class Renderer
{
public:
	static Renderer* get() {
		static Renderer Instance;
		
		return &Instance;
	}

	unsigned int Width = 800;
	unsigned int Height = 800;

	void SetVP(XMMATRIX* aVP, XMFLOAT4* aPlayerPos) {
		VP = aVP;
		PlayerPos = aPlayerPos;
	}

	std::vector<Model> ModelData;
	std::vector<ModelBuffers> Models;

	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;

	cbPerObject cbPerObj;

	vector<ID3D11Buffer*> CubePosBuffer;
	std::vector<XMVECTOR> CubePositions;

	reactphysics3d::DebugRenderer* debugRenderer;

private:
	Renderer() {}

	IDXGISwapChain* SwapChain;
	
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* RasterizerState;

	ID3D11Texture2D* depthStencilBuffer;
	HRESULT hr;
	
	ID3D11Buffer* cbPerObjectBuffer;
	

	XMMATRIX* VP = new XMMATRIX(XMMatrixIdentity());

	//Normal Device
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3DBlob* VS_Buffer;
	ID3DBlob* PS_Buffer;
	ID3D11InputLayout* vertLayout;

	//Marching Cube Device
	ID3D11VertexShader* CubeVS;
	ID3D11GeometryShader* CubeGS;
	ID3D11PixelShader* CubePS;
	ID3DBlob* CubeVS_Buffer;
	ID3DBlob* CubeGS_Buffer;
	ID3DBlob* CubePS_Buffer;
	ID3D11InputLayout* CubepointLayout;

	
	
	const UINT CubeStride = sizeof(XMVECTOR);
	const UINT CubeOffset = 0;
	ID3D11Buffer* TriTableBuffer;

	ID3D11VertexShader* DebugVS;
	ID3D11PixelShader* DebugPS;
	ID3DBlob* DebugVS_Buffer;
	ID3DBlob* DebugPS_Buffer;
	ID3D11InputLayout* DebugLineLayout;
	ID3D11RasterizerState* DebugRasterizerState;

	XMFLOAT4* PlayerPos;

public:
	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;

	bool InitializeDirect3d11App(HINSTANCE hInstance, HWND HandleWindow);
	bool InitializeRenderer();
	bool InitializeCubeRenderer();
	bool InitializeDebugRenderer();
	bool InitializeModel(Model aModel);
	void Resize();

	void ReleaseObjects() {
			//Release the COM Objects we created
			SwapChain->Release();
			d3d11Device->Release();
			d3d11DevCon->Release();
			renderTargetView->Release();
			depthStencilView->Release();
			depthStencilBuffer->Release();

			cbPerObjectBuffer->Release();

			VS->Release();
			PS->Release();
			VS_Buffer->Release();
			PS_Buffer->Release();
			vertLayout->Release();

			ModelData.clear();

			for (auto& i : Models) {
				i.IndexBuffer->Release();
				i.VertexBuffer->Release();
			}

			CubeVS->Release();
			CubeGS->Release();
			CubePS->Release();
			CubeVS_Buffer->Release();
			CubeGS_Buffer->Release();
			CubePS_Buffer->Release();
			CubepointLayout->Release();
			for (auto& i : CubePosBuffer) {
				i->Release();
			}
			TriTableBuffer->Release();

			DebugVS->Release();
			DebugPS->Release();
			DebugVS_Buffer->Release();
			DebugPS_Buffer->Release();
			DebugLineLayout->Release();

	}

	void Draw();
};
