#include "ShadowMap.h"

void ShadowMap::clearDSV(ID3D11DeviceContext* deviceContext)
{
	deviceContext->ClearDepthStencilView(pShadowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void ShadowMap::initializeShadowMap(ID3D11DeviceContext* deviceContext, ID3D11Device* device, GCamera * camera)
{
	InitializeShader(device);
	createCbuffers(device);
	this->camera = camera;
	initializeMatrix(device, deviceContext);
	
	HRESULT hr;

	D3D11_TEXTURE2D_DESC textDesc;
	textDesc.Width = 640;
	textDesc.Height = 640;
	textDesc.MipLevels = 1;
	textDesc.ArraySize = 1;
	textDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textDesc.SampleDesc.Count = 1;
	textDesc.SampleDesc.Quality = 0;
	textDesc.Usage = D3D11_USAGE_DEFAULT;
	textDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textDesc.CPUAccessFlags = 0;
	textDesc.MiscFlags = 0;
	
	hr = device->CreateTexture2D(&textDesc, NULL, &pShadowMap);
	
	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Flags = 0;
	depthDesc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(pShadowMap, &depthDesc, &pShadowDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC shadDesc;
	ZeroMemory(&shadDesc, sizeof(shadDesc));
	shadDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shadDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	shadDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(pShadowMap, &shadDesc, &pShadowSRV);
}

ShadowMap::ShadowMap()
{

}

bool ShadowMap::initialize(ID3D11Device * device, HWND handle)
{
	// Initialize the vertex and pixel shaders.


	return true;
}

void ShadowMap::uninitialize()
{
	pShadowMap->Release();
	pShadowDSV->Release();
	vertexShaderShadow->Release();

	VertexlayoutShadow->Release();
	pShadowSRV->Release();

	lightViewProjMatrixBuffer->Release();
	lightViewMatrixBuffer->Release();
}

//Get an array containing all models in the scene 
//(the ones that are static & basic if possible), 
//loop through them, and calculate the depth for
//the 1st pass
ID3D11ShaderResourceView* ShadowMap::RenderFirstPassShadowed(ID3D11DeviceContext* deviceContext, GModelList &modelList,
	ID3D11RenderTargetView* RTV, ID3D11DepthStencilView* DSV,
	D3D11_VIEWPORT& cameraViewport)
{
	D3D11_VIEWPORT lightViewport;
	lightViewport.Width = (float)640;
	lightViewport.Height = (float)640;
	lightViewport.MinDepth = 0.0f;
	lightViewport.MaxDepth = 1.0f;
	lightViewport.TopLeftX = 0;
	lightViewport.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &lightViewport);

	deviceContext->OMSetRenderTargets(0, NULL, pShadowDSV);
	deviceContext->ClearDepthStencilView(pShadowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->VSSetShader(vertexShaderShadow, nullptr, 0);
	deviceContext->HSSetShader(nullptr, nullptr, 0);
	deviceContext->DSSetShader(nullptr, nullptr, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);

	UINT32 testVertexSize = 8 * sizeof(float) + sizeof(int);
	UINT32 offset = 0;
	GModel* model = modelList.getModelList();

	//Set render targets for the first pass. This sets up our DSV to fill up our resource for later use.
	//deviceContext->OMSetRenderTargets(1, &RTV, pShadowDSV);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < modelList.numberOfModels; i++)
	{
		if (!model[i].isAnimated() && !model[i].hasBlendShape())
		{
			deviceContext->IASetVertexBuffers(0, 1, &model[i].modelVertexBuffer, &testVertexSize, &offset);

			deviceContext->IASetInputLayout(VertexlayoutShadow);

			deviceContext->VSSetConstantBuffers(0, 1, &lightViewMatrixBuffer);
			deviceContext->VSSetConstantBuffers(1, 1, &model[i].modelConstantBuffer);

			deviceContext->IASetIndexBuffer(model[i].modelIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			deviceContext->DrawIndexed(model[i].sizeOfIndexArray, 0, 0);
		}
	}
	//Sets the render target and depth buffer back to it's initial state
	//deviceContext->OMSetRenderTargets(1, &RTV, DSV);

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

	return true;
}


//bool ShadowMap::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * depthMapTexture, XMMATRIX lightPosition, XMMATRIX ambientColor, XMMATRIX diffuseColor)
//{
//	return false;
//}

void ShadowMap::createCbuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC matrixDesc;
	D3D11_BUFFER_DESC lvmMatrixDesc;


	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.ByteWidth = sizeof(matrixCbuff);
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	device->CreateBuffer(&matrixDesc, NULL, &lightViewMatrixBuffer);

	lvmMatrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	lvmMatrixDesc.ByteWidth = sizeof(lvMatrixCbuff);
	lvmMatrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lvmMatrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lvmMatrixDesc.MiscFlags = 0;
	lvmMatrixDesc.StructureByteStride = 0;

	device->CreateBuffer(&lvmMatrixDesc, NULL, &lightViewProjMatrixBuffer);
}

void ShadowMap::initializeMatrix(ID3D11Device* device, ID3D11DeviceContext * deviceContext)
{
	XMVECTOR lPosition = XMVectorSet(4.0f, 3.0f, -3.0f, 1.0f);
	//XMVECTOR lDirection = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR lUp = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR lTarget = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	XMMATRIX ProjectionMat = XMMatrixPerspectiveFovLH(XM_PI * 0.5, 640/480, 0.1, 20.0f);
	XMMATRIX orthoProjectionMat = XMMatrixOrthographicLH(640, 480, 0.1, 1000);

	XMMATRIX viewMat = (XMMatrixLookAtLH(lPosition, lTarget, lUp));

	XMMATRIX viewProj = XMMatrixTranspose(XMMatrixMultiply(viewMat, ProjectionMat));

	matrix_cbuffer.lightViewProjection = viewProj;

	D3D11_MAPPED_SUBRESOURCE mapThing;
	matrixCbuff* dataPtr;
	deviceContext->Map(lightViewMatrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapThing);

	dataPtr = (matrixCbuff*)mapThing.pData;
	dataPtr->lightViewProjection = viewProj;
	
	deviceContext->Unmap(lightViewMatrixBuffer, NULL);

	
	D3D11_MAPPED_SUBRESOURCE mapThing2;
	lvMatrixCbuff* dataPtr2;
	deviceContext->Map(lightViewProjMatrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapThing2);

	dataPtr2 = (lvMatrixCbuff*)mapThing2.pData;
	dataPtr2->lightView = XMMatrixTranspose(viewMat);
	dataPtr2->lightProj = XMMatrixTranspose(ProjectionMat);
	deviceContext->Unmap(lightViewProjMatrixBuffer, NULL);
}

ID3D11Buffer * ShadowMap::getLightCbuffer()
{
	return lightViewMatrixBuffer;
}

void ShadowMap::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{

}
