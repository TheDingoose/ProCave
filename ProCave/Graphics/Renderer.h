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

using namespace DirectX;

constexpr float MCSize = 1.f;
constexpr unsigned int MCFieldSize = 76;



struct cbPerObject
{
	XMMATRIX  WVP;
	XMFLOAT4 PlayerPos;
	float MarchCubeSize = MCSize;
	float Time;
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
private:
	Renderer() {}

	cbPerObject cbPerObj;

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

	std::vector<XMVECTOR> CubePositions;
	vector<ID3D11Buffer*> CubePosBuffer;
	const UINT CubeStride = sizeof(XMVECTOR);
	const UINT CubeOffset = 0;
	ID3D11Buffer* TriTableBuffer;

	XMFLOAT4* PlayerPos;

public:
	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;

	bool InitializeDirect3d11App(HINSTANCE hInstance, HWND HandleWindow);
	bool InitializeRenderer();
	bool InitializeCubeRenderer();
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

	}

	//Move this!
	void Draw() {
#if defined(_DEBUG)
		
		
#endif
		//Clear the screen before drawing anything new
		float bgColor[4] = { 0.37f, 0.18f, 0.56f, 1.f };

		d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);
		d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//Set Shaders
		d3d11DevCon->VSSetShader(VS, 0, 0);
		d3d11DevCon->PSSetShader(PS, 0, 0);
		d3d11DevCon->GSSetShader(0, 0, 0);

		//Set Primitive Topology
		d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(vertLayout);

		for (auto& Mesh : Models) {
			//Set the index buffer
			d3d11DevCon->IASetIndexBuffer(Mesh.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			
			//Set the vertex buffer
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			d3d11DevCon->IASetVertexBuffers(0, 1, &Mesh.VertexBuffer, &stride, &offset);



			Mesh.Transform.UpdateMatrix();
			cbPerObj.WVP = XMMatrixTranspose(Mesh.Transform.Transform * *VP);
			d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
			d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

			//Draw!
			d3d11DevCon->DrawIndexed(Mesh.IndexSize, 0, 0);
		}


		//NOW MARCHING CUBES :)

		//Set Shaders
		d3d11DevCon->VSSetShader(CubeVS, 0, 0);
		d3d11DevCon->PSSetShader(CubePS, 0, 0);
		d3d11DevCon->GSSetShader(CubeGS, 0, 0);

		//Set Primitive Topology
		d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//Set the Input Layouts
		d3d11DevCon->IASetInputLayout(CubepointLayout);
		
		cbPerObj.Time += 0.016f;
		cbPerObj.PlayerPos = *PlayerPos;
		cbPerObj.WVP = XMMatrixTranspose(*VP);
		d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		d3d11DevCon->GSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

		XMVECTOR Player = XMLoadFloat4(PlayerPos);
		XMVECTOR Worker = XMVectorZero();
		unsigned int Numb = 0;
		unsigned int BuffNumb = 0;
		for (float x = (-(MCSize * (MCFieldSize / 2))); x < (MCSize * (MCFieldSize / 2)); x += MCSize) {
			Worker.m128_f32[0] = x;
			for (float y = (-(MCSize * (MCFieldSize / 2))); y < (MCSize * (MCFieldSize / 2)); y += MCSize) {
				Worker.m128_f32[1] = y;
				for (float z = (-(MCSize * (MCFieldSize / 2))); z < (MCSize * (MCFieldSize / 2)); z += MCSize) {
					Worker.m128_f32[2] = z;
					CubePositions[Numb] = XMVectorAdd(Worker, Player);
					Numb++;
				}
			}

			D3D11_MAPPED_SUBRESOURCE BufferData;
			ZeroMemory(&BufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			//D3D11CalcSubresource(0, 0, 1);
			HRESULT hr = d3d11DevCon->Map(CubePosBuffer[BuffNumb], 0, D3D11_MAP_WRITE_DISCARD, 0, &BufferData);
			//CheckError(hr, "Swap texture on noise regenerate");

			memcpy(BufferData.pData, &CubePositions[Numb - MCFieldSize * MCFieldSize], sizeof(XMVECTOR) * MCFieldSize * MCFieldSize);

			Renderer::get()->d3d11DevCon->Unmap(CubePosBuffer[BuffNumb], 0);

			d3d11DevCon->IASetVertexBuffers(0, 1, &CubePosBuffer[BuffNumb], &CubeStride, &CubeOffset);

			d3d11DevCon->Draw(MCFieldSize * MCFieldSize, 0);
			BuffNumb++;
		}

		
		DevUIDriver::get()->Draw();

		SwapChain->Present(0, 0);
	}




};
