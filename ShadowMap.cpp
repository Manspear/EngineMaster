#include "ShadowMap.h"

void ShadowMap::initializeShadowMap(ID3D11DeviceContext* deviceContext, ID3D11Device* device)
{
	/*Create the shadow-buffer's texture*/
	HRESULT hr;
	pShadowMap = NULL;
	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = (float)640;
	depthTexDesc.Height = (float)480;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	//Create the depth stencil view desc
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc;
	//DSVdesc.Format = depthTexDesc.Format; //<-- tutorial says this

	InitializeShader(device);
	DSVdesc.Format = DXGI_FORMAT_D32_FLOAT; //<-- comment says this
	DSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVdesc.Texture2D.MipSlice = 0;

	//Create the shader resource view desc
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc;
	//SRVdesc.Format = DXGI_FORMAT_R32_FLOAT; //<-- tutorial says this
	SRVdesc.Format = DXGI_FORMAT_R32_FLOAT; //<-- comment says this
	SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVdesc.Texture2D.MipLevels = depthTexDesc.MipLevels;
	SRVdesc.Texture2D.MostDetailedMip = 0;
	
	hr = device->CreateTexture2D(&depthTexDesc, NULL, &pShadowMap);
	hr = device->CreateDepthStencilView(pShadowMap, &DSVdesc, &pDepthStencilView);
	hr = device->CreateShaderResourceView(pShadowMap, &SRVdesc, &pShadowResource);
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
	return pShadowMap;
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
