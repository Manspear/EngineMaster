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

	InitializeShader(device);
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
		deviceContext->VSSetShader(vertexShaderShadow, nullptr, 0);
		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, 0);
	}
	

	return false;
}



ID3D11Texture2D * ShadowMap::getDepthTexture()
{
	return pDepthStencilBuffer;
}

bool ShadowMap::InitializeShader(ID3D11Device* device)
{
	//create vertex shader
	ID3DBlob* pVSS = nullptr;
	D3DCompileFromFile(
		L"ShadowVertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pVSS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);


	device->CreateVertexShader(pVSS->GetBufferPointer(), pVSS->GetBufferSize(), nullptr, &vertexShaderShadow);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDescSM[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	device->CreateInputLayout(inputDescSM, ARRAYSIZE(inputDescSM), pVSS->GetBufferPointer(), pVSS->GetBufferSize(), &VertexlayoutShadow);
	// we do not need anymore this COM object, so we release it.
	pVSS->Release();




	//create pixel shader
	ID3DBlob* pPSS = nullptr;
	D3DCompileFromFile(
		L"ShadowPixel.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPSS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);

	device->CreatePixelShader(pPSS->GetBufferPointer(), pPSS->GetBufferSize(), nullptr, &pixelShaderShadow);
	// we do not need anymore this COM object, so we release it.
	pPSS->Release();

	return true;
}


//bool ShadowMap::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * depthMapTexture, XMMATRIX lightPosition, XMMATRIX ambientColor, XMMATRIX diffuseColor)
//{
//	return false;
//}

void ShadowMap::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{

}
