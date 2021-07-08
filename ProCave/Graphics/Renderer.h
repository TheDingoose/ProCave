#pragma once
#include <SH.h>

#include <d3d11.h>
#include <dxgi.h>
#include "Rendering/Vertex.h"
#include <DirectXMath.h>
//#include <d3dx11.h>
//#include <d3dx10.h>
//#include <xnamath.h>

using namespace DirectX;

struct cbPerObject
{
	XMMATRIX  WVP;
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

	void SetVP(XMMATRIX* aVP) {
		VP = aVP;
	}

private:
	Renderer() {}

	cbPerObject cbPerObj;

	IDXGISwapChain* SwapChain;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	HRESULT hr;

	ID3D11Buffer* cbPerObjectBuffer;

	XMMATRIX* VP = new XMMATRIX(XMMatrixIdentity());

	ID3D11Buffer* IndexBuffer;
	ID3D11Buffer* VertBuffer;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3DBlob* VS_Buffer;
	ID3DBlob* PS_Buffer;
	ID3D11InputLayout* vertLayout;

public:
	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;


	bool InitializeDirect3d11App(HINSTANCE hInstance, HWND HandleWindow);
	bool InitializeRenderer();

	void ReleaseObjects() {
			//Release the COM Objects we created
			SwapChain->Release();
			d3d11Device->Release();
			d3d11DevCon->Release();
			renderTargetView->Release();
			depthStencilView->Release();
			depthStencilBuffer->Release();

			cbPerObjectBuffer->Release();

			VertBuffer->Release();
			IndexBuffer->Release();
			VS->Release();
			PS->Release();
			VS_Buffer->Release();
			PS_Buffer->Release();
			vertLayout->Release();
	}

	//Move this!
	void Draw() {
		//Clear the screen before drawing anything new
		float bgColor[4] = { (0.5f, 0.0f, 0.0f, 0.0f) };
		d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);
		d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



		cbPerObj.WVP = XMMatrixTranspose(   *VP);
		d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);


		//Draw!
		d3d11DevCon->DrawIndexed(6, 0, 0);

		SwapChain->Present(0, 0);
	}




};
