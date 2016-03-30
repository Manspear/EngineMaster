#include "DeferredBuffer.h"

DeferredBuffer::DeferredBuffer()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		renderTargerTextureArray[i] = 0;
		renderTargetVíewArray[i] = 0;
		shaderResourceViewArray[i] = 0;
	}

	depthStencilBuffer = 0;
	depthStencilView = 0;
}

DeferredBuffer::DeferredBuffer(const DeferredBuffer &)
{
}

DeferredBuffer::~DeferredBuffer()
{
}

void DeferredBuffer::initialize(ID3D11Device * gDevice, int textureWidth, int textureHeight, float screenDepth, float sceenNear)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT rs;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetVewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilVeiwDesc;
	
	this->textureHeight = textureHeight;
	this->textureWidth = textureWidth;

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	//initialize
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		rs = gDevice->CreateTexture2D(&textureDesc)

	}

}

void DeferredBuffer::Shutdown()
{
}

void DeferredBuffer::setRenderTarget(ID3D11DeviceContext *)
{
}

void DeferredBuffer::clearRenderTargets(ID3D11DeviceContext *, float, float, float, float)
{
}

ID3D11ShaderResourceView DeferredBuffer::getShaderResource(int)
{
	return ID3D11ShaderResourceView();
}
