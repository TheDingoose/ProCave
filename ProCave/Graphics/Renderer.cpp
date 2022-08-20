#include "Renderer.h"
#include <d3dcompiler.h>
#include <iostream>
#include <sstream>

#include "spdlog/spdlog.h"

#include "Graphics/RenderHelper.h"

#include "Tools/DebugSettings.h"
#include "Tools/MarchCubeSettings.h"

#include "reactphysics3d/reactphysics3d.h"
using namespace reactphysics3d;

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(layout);

D3D11_INPUT_ELEMENT_DESC Cubelayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT CubenumElements = ARRAYSIZE(Cubelayout);

D3D11_INPUT_ELEMENT_DESC Debuglayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT DebugnumElements = ARRAYSIZE(Debuglayout);

cbPerObject::cbPerObject()
{
	WVP = XMMatrixIdentity();
	PlayerPos = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	SampleMod = MarchCubeSettings::get()->SampleMod;
	SampleOffset = MarchCubeSettings::get()->SampleOffset;

	CubeSize = MarchCubeSettings::get()->CubeSize;
	PlayerLightStrength = MarchCubeSettings::get()->PlayerLightStrength;
	LightStrength = MarchCubeSettings::get()->LightStrength;
	Time = MarchCubeSettings::get()->Time;

	PlayerLightColor = MarchCubeSettings::get()->PlayerLightColor;
	LightColor = MarchCubeSettings::get()->LightColor;
	FogColor = MarchCubeSettings::get()->FogColor;

	FogDistanceNear = MarchCubeSettings::get()->FogDistanceNear;
	FogDistance = MarchCubeSettings::get()->FogDistance;
	DensityLimit = MarchCubeSettings::get()->DensityLimit;
	NormalSampleDistance = MarchCubeSettings::get()->NormalSampleDistance;

	TextureBlendOffset = MarchCubeSettings::get()->TextureBlendOffset;
	TextureBlendExponent = MarchCubeSettings::get()->TextureBlendExponent;
	TextureBlendHeightStrength = MarchCubeSettings::get()->TextureBlendHeightStrength;
	Pad;             //Three More Slots Here!
};

bool Renderer::InitializeDirect3d11App(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT hr;

	//Describe our Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Or, DXGI_MODE_SCALING_STRETCHED


	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	// If the project is in a debug build, enable the debug layer.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//Create our SwapChain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);


	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create our Depthbuffer
	d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);


	//Create our Render Target
	hr = d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();

	//Set our Render Target
	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView); //Third one here is depth buffer, need that later!!

	return true;
}

bool Renderer::InitializeRenderer()
{
	//Compile Shaders from shader file
	ID3DBlob* compilationMsgs = nullptr;
	
	hr = D3DCompileFromFile(L"VertexShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &VS_Buffer, &compilationMsgs);
	CheckError(hr, compilationMsgs);

	hr = D3DCompileFromFile(L"PixelShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &PS_Buffer, &compilationMsgs);
	CheckError(hr, compilationMsgs);

	//Create the Shader Objects
	hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	CheckError(hr, "Error While compiling Vertex Shader!");
	hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	CheckError(hr, "Error While compiling Pixel Shader!");

	//Set Vertex and Pixel Shaders
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

	//Create the Constant Buffer
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	//Create the Input Layout
	hr = d3d11Device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(), &vertLayout);
	CheckError(hr, "Error creating Input Layout!");

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	d3d11DevCon->RSSetViewports(1, &viewport);

	D3D11_RASTERIZER_DESC rastdesc;
	rastdesc.FillMode = D3D11_FILL_SOLID;
	rastdesc.CullMode = D3D11_CULL_NONE;
	rastdesc.FrontCounterClockwise = FALSE;
	rastdesc.DepthBias = 0;
	rastdesc.SlopeScaledDepthBias = 0.0f;
	rastdesc.DepthBiasClamp = 0.0f;
	rastdesc.DepthClipEnable = TRUE;
	rastdesc.ScissorEnable = FALSE;
	rastdesc.MultisampleEnable = FALSE;
	rastdesc.AntialiasedLineEnable = FALSE;
	
	d3d11Device->CreateRasterizerState(&rastdesc, &RasterizerState);
	d3d11DevCon->RSSetState(RasterizerState);

	InitializeCubeRenderer();
	InitializeDebugRenderer();
	return true;
}

bool Renderer::InitializeCubeRenderer()
{
	//Compile Shaders from shader file
	ID3DBlob* compilationMsgs = nullptr;
	
	hr = D3DCompileFromFile(L"CubeVertexShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &CubeVS_Buffer, &compilationMsgs);
	CheckError(hr, compilationMsgs);

	hr = D3DCompileFromFile(L"CubePixelShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &CubePS_Buffer, &compilationMsgs);
	CheckError(hr, compilationMsgs);

	hr = D3DCompileFromFile(L"CubeGeometryShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0", 0, 0, &CubeGS_Buffer, &compilationMsgs);
	CheckError(hr, compilationMsgs);

	//Create the Shader Objects
	hr = d3d11Device->CreateVertexShader(CubeVS_Buffer->GetBufferPointer(), CubeVS_Buffer->GetBufferSize(), NULL, &CubeVS);
	CheckError(hr, "Error While compiling Cube Vertex Shader!");
	hr = d3d11Device->CreatePixelShader(CubePS_Buffer->GetBufferPointer(), CubePS_Buffer->GetBufferSize(), NULL, &CubePS);
	CheckError(hr, "Error While compiling Cube Pixel Shader!");
	hr = d3d11Device->CreateGeometryShader(CubeGS_Buffer->GetBufferPointer(), CubeGS_Buffer->GetBufferSize(), NULL, &CubeGS);
	CheckError(hr, "Error While compiling Cube Geometry Shader!");

	//Create the Input Layout
	hr = d3d11Device->CreateInputLayout(Cubelayout, CubenumElements, CubeVS_Buffer->GetBufferPointer(),
		CubeVS_Buffer->GetBufferSize(), &CubepointLayout);
	CheckError(hr, "Error creating Input Layout!");

	//Setup the batch of position data of where to render the cubes
	CubePositions.resize(MarchCubeSettings::get()->GridSize * MarchCubeSettings::get()->GridSize * MarchCubeSettings::get()->GridSize);

	//Describe the vertex buffer
	D3D11_BUFFER_DESC pointBufferDesc;
	ZeroMemory(&pointBufferDesc, sizeof(pointBufferDesc));

	pointBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pointBufferDesc.ByteWidth = sizeof(XMVECTOR) * MarchCubeSettings::get()->GridSize * MarchCubeSettings::get()->GridSize * MarchCubeSettings::get()->GridSize;
	pointBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	pointBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pointBufferDesc.MiscFlags = 0;

	CubePosBuffer.resize(MarchCubeSettings::get()->GridSize);
	for (int i = 0; i < MarchCubeSettings::get()->GridSize; i++) {
		hr = d3d11Device->CreateBuffer(&pointBufferDesc, 0, &CubePosBuffer[i]);
	}
	hr = d3d11Device->CreateBuffer(&pointBufferDesc, 0, &CubePosiBuffer);

	//Create the Constant Buffer
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	int triTable[256 * 16] =
	{ 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1,
	3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1,
	3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1,
	3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1,
	9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1,
	1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1,
	9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1,
	2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1,
	8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1,
	9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1,
	4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1,
	3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1,
	1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1,
	4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1,
	4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1,
	9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1,
	1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1,
	5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1,
	2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1,
	9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1,
	0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1,
	2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1,
	10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1,
	4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1,
	5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1,
	5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1,
	9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1,
	0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1,
	1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1,
	10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1,
	8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1,
	2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1,
	7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1,
	9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1,
	2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1,
	11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1,
	9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1,
	5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1,
	11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1,
	11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1,
	1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1,
	9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1,
	5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1,
	2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1,
	0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1,
	5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1,
	6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1,
	0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1,
	3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1,
	6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1,
	5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1,
	1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1,
	10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1,
	6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1,
	1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1,
	8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1,
	7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1,
	3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1,
	5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1,
	0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1,
	9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1,
	8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1,
	5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1,
	0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1,
	6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1,
	10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1,
	10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1,
	8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1,
	1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1,
	3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1,
	0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1,
	10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1,
	0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1,
	3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1,
	6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1,
	9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1,
	8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1,
	3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1,
	6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1,
	0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1,
	10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1,
	10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1,
	1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1,
	2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1,
	7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1,
	7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1,
	2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1,
	1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1,
	11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1,
	8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1,
	0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1,
	7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1,
	10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1,
	2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1,
	6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1,
	7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1,
	2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1,
	1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1,
	10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1,
	10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1,
	0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1,
	7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1,
	6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1,
	8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1,
	9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1,
	6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1,
	1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1,
	4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1,
	10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1,
	8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1,
	0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1,
	1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1,
	8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1,
	10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1,
	4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1,
	10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1,
	5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1,
	11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1,
	9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1,
	6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1,
	7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1,
	3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1,
	7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1,
	9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1,
	3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1,
	6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1,
	9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1,
	1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1,
	4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1,
	7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1,
	6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1,
	3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1,
	0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1,
	6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1,
	1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1,
	0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1,
	11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1,
	6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1,
	5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1,
	9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1,
	1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1,
	1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1,
	10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1,
	0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1,
	5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1,
	10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1,
	11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1,
	0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1,
	9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1,
	7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1,
	2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1,
	8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1,
	9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1,
	9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1,
	1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1,
	9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1,
	9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1,
	5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1,
	0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1,
	10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1,
	2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1,
	0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1,
	0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1,
	9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1,
	5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1,
	3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1,
	5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1,
	8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1,
	0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1,
	9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1,
	0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1,
	1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1,
	3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1,
	4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1,
	9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1,
	11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1,
	11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1,
	2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1,
	9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1,
	3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1,
	1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1,
	4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1,
	4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1,
	0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1,
	3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1,
	3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1,
	0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1,
	9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1,
	1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	int PaddedtriTable[256 * 16 * 4];
	//This is stupid and I hate it
	for (int i = 0; i < 256 * 16; i ++) {
		PaddedtriTable[i * 4] = triTable[i];
		PaddedtriTable[i * 4 + 1] = -2;
		PaddedtriTable[i * 4 + 2] = -3;
		PaddedtriTable[i * 4 + 3] = -4;

	}

	D3D11_SUBRESOURCE_DATA Deets;
	Deets.pSysMem = PaddedtriTable;
	Deets.SysMemPitch = 0;

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(int) * 256 * 16 * 4;
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, &Deets, &TriTableBuffer);
	d3d11DevCon->GSSetConstantBuffers(1, 1, &TriTableBuffer);

	Deets.pSysMem = &Lights;
	cbbd.ByteWidth = sizeof(LightingCollection);
	hr = d3d11Device->CreateBuffer(&cbbd, &Deets, &LightBuffer);

	WorldTextures.push_back(new Texture(d3d11Device, "../Assets/Texture/sandstone.jpg"));
	WorldTextures.push_back(new Texture(d3d11Device, "../Assets/Texture/roughstone.jpg"));
	WorldTextures.push_back(new Texture(d3d11Device, "../Assets/Texture/crackedstone.jpg"));
	WorldTextures.push_back(new Texture(d3d11Device, "../Assets/Texture/bigrock/bigrockalb.jpg"));
	WorldTextures.push_back(new Texture(d3d11Device, "../Assets/Texture/bigrock/bigrocknor.jpg"));
	//WorldTextures.push_back(new Texture(d3d11Device, "../Assets/Texture/normaal.jpg"));
	WorldTextures.push_back(new Texture(d3d11Device, "../Assets/Texture/bigrock/bigrockbmp.jpg"));

	return true;
}

bool Renderer::InitializeDebugRenderer()
{
	ID3DBlob* compilationMsgs = nullptr;

	hr = D3DCompileFromFile(L"DebugVertexShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &DebugVS_Buffer, &compilationMsgs);
	CheckError(hr, compilationMsgs);

	hr = D3DCompileFromFile(L"DebugPixelShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &DebugPS_Buffer, &compilationMsgs);
	CheckError(hr, compilationMsgs);

	//Create the Shader Objects
	hr = d3d11Device->CreateVertexShader(DebugVS_Buffer->GetBufferPointer(), DebugVS_Buffer->GetBufferSize(), NULL, &DebugVS);
	CheckError(hr, "Error While compiling Debug Vertex Shader!");
	hr = d3d11Device->CreatePixelShader(DebugPS_Buffer->GetBufferPointer(), DebugPS_Buffer->GetBufferSize(), NULL, &DebugPS);
	CheckError(hr, "Error While compiling Debug Pixel Shader!");

	//Create the Input Layout
	hr = d3d11Device->CreateInputLayout(Debuglayout, DebugnumElements, DebugVS_Buffer->GetBufferPointer(),
		DebugVS_Buffer->GetBufferSize(), &DebugLineLayout);
	CheckError(hr, "Error creating Input Layout!");

	D3D11_RASTERIZER_DESC rastdesc;
	rastdesc.FillMode = D3D11_FILL_WIREFRAME;
	rastdesc.CullMode = D3D11_CULL_NONE;
	rastdesc.FrontCounterClockwise = FALSE;
	rastdesc.DepthBias = 0;
	rastdesc.SlopeScaledDepthBias = 0.0f;
	rastdesc.DepthBiasClamp = 0.0f;
	rastdesc.DepthClipEnable = TRUE;
	rastdesc.ScissorEnable = FALSE;
	rastdesc.MultisampleEnable = FALSE;
	rastdesc.AntialiasedLineEnable = FALSE;

	d3d11Device->CreateRasterizerState(&rastdesc, &DebugRasterizerState);

	return true;
}

bool Renderer::InitializeMesh(Mesh mesh)
{
	ModelBuffers Buffers;

	//Describe the index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * mesh.Indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	//Describe the vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * mesh.Vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &mesh.Indices[0];
	iinitData.SysMemPitch = 0;


	hr = d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &Buffers.IndexBuffer);
	CheckError(hr, "Error creating Index Buffer!");
	Buffers.IndexSize = mesh.Indices.size();

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &mesh.Vertices[0];
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &Buffers.VertexBuffer);
	CheckError(hr, "Error creating Vertex Buffer!");

	Models.push_back(Buffers);
	return true;
}

void Renderer::Resize()
{
	if (Width == 0 || Height == 0) { return; }
	if (SwapChain)
	{
		d3d11DevCon->OMSetRenderTargets(0, 0, 0);

		// Release all outstanding references to the swap chain's buffers.
		renderTargetView->Release();

		HRESULT hr;
		// Preserve the existing buffer count and format.
		// Automatically choose the width and height to match the client rect for HWNDs.
		hr = SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		CheckError(hr, "Error in resizing the buffer");

		// Get buffer and create a render-target-view.
		ID3D11Texture2D* pBuffer;
		hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			(void**)&pBuffer);
		CheckError(hr, "Error in getting the buffer");

		hr = d3d11Device->CreateRenderTargetView(pBuffer, NULL,
			&renderTargetView);
		CheckError(hr, "Error in creating the new render target view");
		pBuffer->Release();

		// Set up the viewport.
		D3D11_VIEWPORT vp;
		vp.Width = Width;
		vp.Height = Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		d3d11DevCon->RSSetViewports(1, &vp);

		depthStencilBuffer->Release();
		depthStencilView->Release();


		//Describe our Depth/Stencil Buffer
		D3D11_TEXTURE2D_DESC depthStencilDesc;

		depthStencilDesc.Width = Width;
		depthStencilDesc.Height = Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//Create our Depthbuffer
		d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
		d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

		d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	}
}

void Renderer::Draw()
{
#if defined(_DEBUG)


#endif
	//Clear the screen before drawing anything new
	float bgColor[4] = { 0.f, 0.f, 0.f, 1.f };
	//float bgColor[4] = { cbPerObj.FogColor.x, cbPerObj.FogColor.y, cbPerObj.FogColor.z, cbPerObj.FogColor.w} ;

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
		d3d11DevCon->IASetIndexBuffer(Mesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

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

	//! NOW MARCHING CUBES :)
	if (MarchCubeSettings::get()->GridSize != 0) {
		//Set Shaders
		d3d11DevCon->VSSetShader(CubeVS, 0, 0);
		d3d11DevCon->PSSetShader(CubePS, 0, 0);
		d3d11DevCon->GSSetShader(CubeGS, 0, 0);

		//Set Primitive Topology
		d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//Set the Input Layouts
		d3d11DevCon->IASetInputLayout(CubepointLayout);

		cbPerObj.WVP = XMMatrixTranspose(*VP);
		cbPerObj.PlayerPos = *PlayerPos;
		cbPerObj.Time = MarchCubeSettings::get()->Time;

		d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		d3d11DevCon->GSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		d3d11DevCon->PSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		d3d11DevCon->UpdateSubresource(LightBuffer, 0, NULL, &Lights, 0, 0);
		d3d11DevCon->PSSetConstantBuffers(1, 1, &LightBuffer);

		d3d11DevCon->PSSetShaderResources(0, 1, &WorldTextures[3]->m_TextureView);
		d3d11DevCon->PSSetSamplers(0, 1, &WorldTextures[3]->m_SamplerState);

		d3d11DevCon->PSSetShaderResources(1, 1, &WorldTextures[4]->m_TextureView);
		d3d11DevCon->PSSetSamplers(1, 1, &WorldTextures[4]->m_SamplerState);

		d3d11DevCon->GSSetShaderResources(2, 1, &WorldTextures[5]->m_TextureView);
		d3d11DevCon->GSSetSamplers(2, 1, &WorldTextures[5]->m_SamplerState);


		XMVECTOR Player = XMVectorRound(XMLoadFloat4(PlayerPos));
		//We have to round the player position to a multiple of the cubesize
		Player = GridRound(Player);


		XMVECTOR Worker = XMVectorZero();
		unsigned int Numb = 0;
		unsigned int BuffNumb = 0;
		//int GridSize = MarchCubeSettings::get()->GridSize;

		//XMVECTOR DepthPos = XMVectorZero();
		XMVECTOR PlayerXAngle = (XMVector4Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), *PlayerTransform));
		XMVECTOR PlayerYAngle = (XMVector4Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), *PlayerTransform));
		XMVECTOR PlayerZAngle = (XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), *PlayerTransform));

		XMVECTOR BehindCam = XMLoadFloat4(&cbPerObj.PlayerPos) - PlayerZAngle;

		float CHalfHeight = /* 2.f * */ tan((0.4f * 3.14f) / 2.f) * (((float)MarchCubeSettings::get()->GridSize + 1.f) * cbPerObj.CubeSize); //Is this height or width?
		float CHalfWidth = CHalfHeight * ((float)Width / (float)Height);

		XMVECTOR FrontPoint = Player + (float)MarchCubeSettings::get()->GridSize * PlayerZAngle;

		XMVECTOR RightUpPoint = FrontPoint + PlayerXAngle * CHalfWidth + PlayerYAngle * CHalfHeight;
		XMVECTOR RightDownPoint = FrontPoint + PlayerXAngle * CHalfWidth + PlayerYAngle * -CHalfHeight;
		XMVECTOR LeftUpPoint = FrontPoint + -PlayerXAngle * CHalfWidth + PlayerYAngle * CHalfHeight;
		XMVECTOR LeftDownPoint = FrontPoint + -PlayerXAngle * CHalfWidth + PlayerYAngle * -CHalfHeight;

		RightUpPoint = GridRound(RightUpPoint);
		RightDownPoint = GridRound(RightDownPoint);
		LeftUpPoint = GridRound(LeftUpPoint);
		LeftDownPoint = GridRound(LeftDownPoint);




		XMVECTOR MinCube = XMVectorMin(BehindCam, XMVectorMin(RightUpPoint, XMVectorMin(RightDownPoint, XMVectorMin(LeftUpPoint, XMVectorMin(LeftDownPoint, XMVectorSplatInfinity())))));
		XMVECTOR MaxCube = XMVectorMax(BehindCam, XMVectorMax(RightUpPoint, XMVectorMax(RightDownPoint, XMVectorMax(LeftUpPoint, XMVectorMax(LeftDownPoint, -XMVectorSplatInfinity())))));
		MinCube = GridFloor(MinCube);
		MaxCube = GridCeil(MaxCube);
		//PlayerYAngle = PlayerYAngle * (cbPerObj.CubeSize / fabs(PlayerYAngle.m128_f32[1]));
		//float SizeX = PlayerXAngle.m128_f32[0] * cbPerObj.CubeSize;
		//float SizeY = PlayerXAngle.m128_f32[1] * cbPerObj.CubeSize;
		//float SizeZ = PlayerXAngle.m128_f32[2] * cbPerObj.CubeSize;

		std::vector<XMVECTOR> Points;
		Points.resize(((int)fabs(MinCube.m128_f32[0] - MaxCube.m128_f32[0])) * ((int)fabs(MinCube.m128_f32[1] - MaxCube.m128_f32[1])));
		for (float z = MinCube.m128_f32[2]; z < MaxCube.m128_f32[2]; z += cbPerObj.CubeSize) {
			unsigned int NumPoints = 0;
			for (float y = MinCube.m128_f32[1]; y < MaxCube.m128_f32[1]; y += cbPerObj.CubeSize) {
				for (float x = MinCube.m128_f32[0]; x < MaxCube.m128_f32[0]; x += cbPerObj.CubeSize) {

					Points[NumPoints] = XMVectorSet(x, y, z, 0);
					NumPoints++;
				}
			}

			//Send them!
			D3D11_MAPPED_SUBRESOURCE BufferData;
			ZeroMemory(&BufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			HRESULT hr = d3d11DevCon->Map(CubePosiBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &BufferData);
			//CheckError(hr, "Swap texture on noise regenerate");

			memcpy(BufferData.pData, &Points[0], sizeof(XMVECTOR) * NumPoints);

			Renderer::get()->d3d11DevCon->Unmap(CubePosiBuffer, 0);

			d3d11DevCon->IASetVertexBuffers(0, 1, &CubePosiBuffer, &CubeStride, &CubeOffset);

			d3d11DevCon->Draw(NumPoints, 0);
		}
	}

	//! Collision Debug Draw!	
	
	if (DebugSettings::get()->DrawPhysicsDebug && debugRenderer->getNbTriangles() > 0) {
		
		d3d11DevCon->VSSetShader(DebugVS, 0, 0);
		d3d11DevCon->PSSetShader(DebugPS, 0, 0);
		d3d11DevCon->GSSetShader(0, 0, 0);

		d3d11DevCon->RSSetState(DebugRasterizerState);

		//Set Primitive Topology
		d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(DebugLineLayout);

		//Set the vertex buffer
		UINT stride = sizeof(XMVECTOR);
		UINT offset = 0;

		ID3D11Buffer* Buffer;

		//Describe the vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(XMVECTOR) * debugRenderer->getNbTriangles() * 3;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = debugRenderer->getTrianglesArray();
		hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &Buffer);
		CheckError(hr, "Error creating Vertex Buffer!");

		d3d11DevCon->IASetVertexBuffers(0, 1, &Buffer, &stride, &offset);

		//Mesh.Transform.UpdateMatrix();
		cbPerObj.WVP = XMMatrixTranspose(*VP);
		d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

		//Draw!
		d3d11DevCon->Draw(debugRenderer->getNbTriangles() * 3, 0);

		Buffer->Release();
		d3d11DevCon->RSSetState(RasterizerState);

	}
	
	DevUIDriver::get()->Draw();

	SwapChain->Present(0, 0);
}