#include "ShadowMap.h"

void ShadowMap::initializeShadowMap(ID3D11DeviceContext* deviceContext, ID3D11Device* device)
{
	/*Create the shadow-buffer's texture*/
	HRESULT hr;
	pDepthStencilBuffer = NULL;
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
	hr = device->CreateTexture2D(&descDepth, NULL, &pDepthStencilBuffer);
	hr = device->CreateDepthStencilView(pDepthStencilBuffer, NULL, &pDepthStencilView);
}

ShadowMap::ShadowMap()
{
	/*lightViewport.Width = (float)640;
	lightViewport.Height = (float)480;
	lightViewport.MinDepth = 0.0f;
	lightViewport.MaxDepth = 1.0f;
	lightViewport.TopLeftX = 0;
	lightViewport.TopLeftY = 0;*/
}

bool ShadowMap::initialize(ID3D11Device * device, HWND handle)
{
	// Initialize the vertex and pixel shaders.


	return true;
}

void ShadowMap::uninitialize()
{
}

bool ShadowMap::RenderShadowed(ID3D11DeviceContext* deviceContext, ID3D11Buffer* vertexBuffer, ShaderType shadTp, UINT32 vertexSize)
{
	if (shadTp == ShaderType::vanilla)
	{
		//Set vertexSize somewhere!
		UINT32 vertexSize = 0;
		int offset = 0;
		//The firstPassShader obviously needs a HLSL file tied to it.
		deviceContext->VSSetShader(firstPassShader, nullptr, 0);
		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, 0);
	}
	

	return false;
}



ID3D11Texture2D * ShadowMap::getDepthTexture()
{
	return pDepthStencilBuffer;
}

bool ShadowMap::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{

	//HRESULT result;
	//ID3D10Blob* errorMessage;
	//ID3D10Blob* vertexShaderBuffer;
	//ID3D10Blob* pixelShaderBuffer;
	//D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	//unsigned int numElements;
	//D3D11_SAMPLER_DESC samplerDesc;
	//D3D11_BUFFER_DESC matrixBufferDesc;
	//D3D11_BUFFER_DESC lightBufferDesc;

	return true;
}


//bool ShadowMap::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * depthMapTexture, XMMATRIX lightPosition, XMMATRIX ambientColor, XMMATRIX diffuseColor)
//{
//	return false;
//}

void ShadowMap::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{

}
