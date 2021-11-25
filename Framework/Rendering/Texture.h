#pragma once

#include "stb/stb_image.h"
#include <d3d11.h>
//#include <d3dx11tex.h>
#include <string>
#include "spdlog/spdlog.h"

static int counter = 0;

class Texture
{
public:
	void LoadTexture(ID3D11Device* Device, std::string FileName) {

		int width, height, channels;
		unsigned char* img = stbi_load(FileName.c_str(), &width, &height, &channels, 4);
		if (img == NULL) {
			spdlog::warn("Error in loading the image: " + FileName);
			spdlog::warn(stbi_failure_reason());
			return;
		}
		
		//GetPixelFormat();

		D3D11_SUBRESOURCE_DATA Data;
		Data.pSysMem = img;
		Data.SysMemPitch = width * 4;

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = desc.ArraySize = 1;
		//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		Device->CreateTexture2D(&desc, &Data, &m_Texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;

		Device->CreateShaderResourceView(m_Texture, &srvDesc, &m_TextureView);

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		Device->CreateSamplerState(&sampDesc, &m_SamplerState);
	};

	//Texture() { m_Texture = 0; };
	Texture(ID3D11Device* Device, std::string FileName) {
		LoadTexture(Device, FileName);
	};
	~Texture() {
		if (m_Texture != nullptr) {
			m_Texture->Release();
		}
	};

	int count = counter++;

	ID3D11Texture2D* m_Texture;
	ID3D11SamplerState* m_SamplerState;
	ID3D11ShaderResourceView* m_TextureView;
};

