#include "ShadowMap.h"

void ShadowMap::createDepthBuffer(ID3D11DeviceContext* deviceContext, ID3D11Device* device)
{
	/*Create the shadow-buffer's texture*/
	HRESULT hr;
	pDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = (float)640;
	descDepth.Height = (float)480;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D16_UNORM;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
}

ShadowMap::ShadowMap()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleStateWrap = 0;

	//Initialize the new sampler state to null in the class constructor.

	m_sampleStateClamp = 0;
	m_matrixBuffer = 0;
	m_lightBuffer = 0;
	m_lightBuffer2 = 0;
}

bool ShadowMap::initialize(ID3D11Device * device, HWND handle)
{
	// Initialize the vertex and pixel shaders.


	return true;
}

void ShadowMap::uninitialize()
{
}

bool ShadowMap::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * depthMapTexture, XMMATRIX lightPosition, XMMATRIX ambientColor, XMMATRIX diffuseColor)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, texture,
		depthMapTexture, lightPosition, ambientColor, diffuseColor);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

ID3D11Texture2D * ShadowMap::getDepthTexture()
{
	return pDepthStencil;
}

bool ShadowMap::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

}


bool ShadowMap::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * depthMapTexture, XMMATRIX lightPosition, XMMATRIX ambientColor, XMMATRIX diffuseColor)
{
	return false;
}

void ShadowMap::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{

}
