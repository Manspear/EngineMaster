#include "ShadowMap.h"

void ShadowMap::initializeShadowMap(ID3D11DeviceContext* deviceContext, ID3D11Device* device)
{
	InitializeShader(device);
	createCbuffers(device);
	initializeMatrix(device, deviceContext);
	




	/*Create the shadow-buffer's texture*/
	HRESULT hr;
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
	//Borked!
	hr = device->CreateDepthStencilView(pShadowMap, &DSVdesc, &pShadowDSV);

	hr = device->CreateShaderResourceView(pShadowMap, &SRVdesc, &pShadowSRV);
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

//Get an array containing all models in the scene 
//(the ones that are static & basic if possible), 
//loop through them, and calculate the depth for
//the 1st pass
ID3D11ShaderResourceView* ShadowMap::RenderFirstPassShadowed(ID3D11DeviceContext* deviceContext, GModelList modelList,
	ID3D11RenderTargetView* RTV, ID3D11DepthStencilView* DSV)
{
	//Set render targets for the first pass. This sets up our DSV to fill up our resource for later use.
	deviceContext->OMSetRenderTargets(0, 0, pShadowDSV);
	//Clear the depth stencil view since last "run". Since we render our scene "one model at a time"
	//we should maybe move this to another function. We will see.
	deviceContext->ClearDepthStencilView(pShadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	UINT32 testVertexSize = 8 * sizeof(float) + sizeof(int);
	int offset = 0;
	GModel* model = modelList.getModelList();

	deviceContext->VSSetShader(vertexShaderShadow, nullptr, 0);
	for (int i = 0; i < modelList.numberOfModels; i++)
	{
		if (!model[i].isAnimated() && !model[i].hasBlendShape())
		{
			deviceContext->IASetVertexBuffers(0, 1, &model[i].modelVertexBuffer, &testVertexSize, 0);
			deviceContext->IASetInputLayout(VertexlayoutShadow);

			deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
			deviceContext->VSSetConstantBuffers(1, 1, &model[i].modelConstantBuffer);

			deviceContext->IASetIndexBuffer(model[i].modelIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			deviceContext->DrawIndexed(model[i].sizeOfIndexArray, 0, 0);
		}
	}
	//Sets the render target and depth buffer back to it's initial state
	deviceContext->OMSetRenderTargets(1, &RTV, DSV);

	

	return pShadowSRV;
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


	//create samplers
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampleStateWrap);


	// create the clamp texture
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	device->CreateSamplerState(&samplerDesc, &sampleStateClamp);

	return true;
}


//bool ShadowMap::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * depthMapTexture, XMMATRIX lightPosition, XMMATRIX ambientColor, XMMATRIX diffuseColor)
//{
//	return false;
//}

void ShadowMap::createCbuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC matrixDesc;
	D3D11_BUFFER_DESC worldDesc;
	D3D11_BUFFER_DESC lightDesc;

	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.ByteWidth = sizeof(matrixCbuff);
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	device->CreateBuffer(&matrixDesc, NULL, &matrixBuffer);

	worldDesc.Usage = D3D11_USAGE_DYNAMIC;
	worldDesc.ByteWidth = sizeof(matrixCbuff);
	worldDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	worldDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	worldDesc.MiscFlags = 0;
	worldDesc.StructureByteStride = 0;

	device->CreateBuffer(&matrixDesc, NULL, &matrixBuffer);
}

void ShadowMap::initializeMatrix(ID3D11Device* device, ID3D11DeviceContext * deviceContext)
{
	XMVECTOR lPosition = XMVectorSet(4.0f, 3.0f, -3.0f, 1.0f);
	XMVECTOR lDirection = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR lUp = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR lTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX orthoProjectionMat = XMMatrixOrthographicLH(640, 480, 0.1, 1000);


	matrix_cbuffer.lightViewMatrix = XMMatrixLookAtLH(lPosition, lTarget, lUp);

	//XMStoreFloat4x4(&matrix_cbuffer.lightViewMatrix, XMMatrixLookToLH(lPosition, lDirection, lUp));
	matrix_cbuffer.lightProjectionMatrix = orthoProjectionMat;

	//If error, try transposing the matrices.

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(matrixCbuff);
	device->CreateBuffer(&bufferDesc, NULL, &matrixBuffer);

	D3D11_MAPPED_SUBRESOURCE mapThing;
	matrixCbuff* dataPtr;
	deviceContext->Map(matrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapThing);

	//deviceContext->Map(gConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);

	dataPtr = (matrixCbuff*)mapThing.pData;
	dataPtr = &matrix_cbuffer;
	deviceContext->Unmap(matrixBuffer, NULL);

}

void ShadowMap::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{

}
