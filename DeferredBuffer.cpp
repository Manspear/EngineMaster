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

void DeferredBuffer::initialize(ID3D11Device * gDevice, ID3D11DeviceContext * gDeviceContext, int textureWidth, int textureHeight, float screenDepth, float sceenNear)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT rs;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetVewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilVeiwDesc;
	
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
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
		rs = gDevice->CreateTexture2D(&textureDesc, NULL, &renderTargerTextureArray[i]);
	}

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 1; i < BUFFER_COUNT; i++)
	{
		rs = gDevice->CreateShaderResourceView(renderTargerTextureArray[i], &shaderResourceViewDesc, &shaderResourceViewArray[i]);
	}

	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	rs = gDevice->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);

	ZeroMemory(&depthStencilVeiwDesc, sizeof(depthStencilVeiwDesc));

	depthStencilVeiwDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilVeiwDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilVeiwDesc.Texture2D.MipSlice = 0;

	rs = gDevice->CreateDepthStencilView(depthStencilBuffer, &depthStencilVeiwDesc, &depthStencilView);

	viewport.Width = (float)textureWidth;
	viewport.Height = (float)textureHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	return;
}

void DeferredBuffer::Shutdown()
{
	if (depthStencilView)
		depthStencilView->Release();
	if (depthStencilBuffer)
		depthStencilBuffer->Release();

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		if (shaderResourceViewArray[i])
			shaderResourceViewArray[i]->Release();
		if (renderTargerTextureArray[i])
			renderTargerTextureArray[i]->Release();
		if (renderTargetVíewArray[i])
			renderTargetVíewArray[i]->Release();
	}
	return;
}

void DeferredBuffer::setRenderTarget()
{
	gDeviceContext->OMSetRenderTargets(BUFFER_COUNT, renderTargetVíewArray, depthStencilView);
	gDeviceContext->RSSetViewports(1, &viewport);
}

void DeferredBuffer::clearRenderTargets(float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		gDeviceContext->ClearRenderTargetView(renderTargetVíewArray[i], color);
	}

	gDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

ID3D11ShaderResourceView * DeferredBuffer::getShaderResource(int view)
{
	return shaderResourceViewArray[view];
}
