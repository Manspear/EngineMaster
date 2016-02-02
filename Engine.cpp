#include "Engine.h"
#include "bth_image.h" //This header wouldn't work in Engine.h VS complained
					   //of one or more multiply defined symbols found
#include <fbxsdk.h>

Engine::Engine(){
	//EMPTY
}


void Engine::CreateShaders()
{



	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);


	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"PixelShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();

	//Creating Geometry Shader...
	ID3DBlob* pGS = nullptr; //This may be used for error handling!
	D3DCompileFromFile(
		L"GeometryShader.hlsl", //The L here specifies unicode vs. ansii... Dunno exactly.
		nullptr,
		nullptr,
		"GS_main",
		"gs_4_0",
		0,
		0,
		&pGS,
		nullptr
		);
	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);
	pGS->Release();
}

void Engine::CreateTriangleData()
{
	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	};

	TriangleVertex triangleVertices[6] =
	{
		-0.5f, 0.5f, 0.0f,	//v0 pos
		0.0f, 0.0f,	//v0 uv

		0.5f, -0.5f, 0.0f,	//v1
		1.0f, 1.0f, 	//v1 uv

		-0.5f, -0.5f, 0.0f, //v2
		0.0f, 1.0f, 	//v2 uv

		-0.5f, 0.5f, 0.0f,	//v3 pos
		0.0f, 0.0f,	//v3 uv

		0.5f, 0.5f, 0.0f, //v4
		1.0f, 0.0f,	//v4 uv

		0.5f, -0.5f, 0.0f,	//v5
		1.0f, 1.0f	//v5 uv	

	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	gDevice->CreateBuffer(&bufferDesc, &data, &gVertexBuffer);
}

void Engine::CreateTexture() {
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = BTH_IMAGE_WIDTH;
	textureDesc.Height = BTH_IMAGE_HEIGHT;
	textureDesc.MipLevels = textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MiscFlags = 0;
	textureDesc.CPUAccessFlags = 0;

	ID3D11Texture2D *pTexture = NULL;
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = (void*)BTH_IMAGE_DATA;
	data.SysMemPitch = BTH_IMAGE_WIDTH * 4 * sizeof(char);
	hr = gDevice->CreateTexture2D(&textureDesc, &data, &pTexture);
	//resource view description
	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
	resViewDesc.Format = textureDesc.Format;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	resViewDesc.Texture2D.MostDetailedMip = 0;
	gDevice->CreateShaderResourceView(pTexture, &resViewDesc, &gTextureView);
	pTexture->Release();
}

void Engine::CreateConstantBuffer() {

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(matrixBuffer);
	gDevice->CreateBuffer(&bufferDesc, NULL, &gConstantBuffer);
}

void Engine::CreateDepthStencilBuffer() {

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = (float)640;
	depthStencilDesc.Height = (float)480;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	depthStencilDesc.SampleDesc.Count = 4;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	gDevice->CreateTexture2D(&depthStencilDesc, NULL, &gDepthStencilBuffer);
	gDevice->CreateDepthStencilView(gDepthStencilBuffer, NULL, &depthStencilView);
}

void Engine::SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)640;
	vp.Height = (float)480;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}
// SWAG
void Engine::Render()
{
	float clearColor[] = { 0, 0, 0, 1 };
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//>>>EXPLANATION OF DEPTH-BUFFER AND IT'S RELATIONSHIP WITH VIEW-FRUSTUM<<<
	//The depth buffer clears itself with a value between 0 and 1. If it clears to 1
	//it has a depth-value corresponding to the Far Plane of the view-frustum. 
	//If it clears to 0, it has a depth-value corresponding to the Near-Plane  
	//of the view-frustum.
	//You want to clear the depth-buffer to 1, because you use the depth value 
	//that you've cleared to as comparison to the depth-value of objects. 
	//The check goes like this:
	//if(storedDepth > objectDepth){ storedDepth = objectDepth } (it compares objects in the order they are processed)
	//So the cleared-to-value is used as a basis. Usually getting replaced pretty quickly
	//with the object-depth.
	//
	//So now to the not-so-intuitive part:
	//You should think that when the depth-value is 0.5 that it's situated halfway 
	//between the view-frustum's nearplane and farplane, but it's not. The reasoning
	//here is that we are more concerned about object-flickering that occurs close
	//to the camera, since it's more noticeable to the viewer.
	//So the depth-values are therefore distributed more generously near the Near-Plane, than the far-plane.
	//The exact distribution I am unaware of though.

	//>>>SMALL EXPLANATIN OF STENCIL BUFFER INC<<<
	//Stencil buffer is used when doing shadow volumes. It stores values used to determine is something is inside of a shadow volume.
	//The values it holds are both positive and negative, where negative values represent 
	//the shadow volume that is behind the last object that it hits, and positive values 
	//represent the shadow-volume before it hits it's "object-that-will-get-shadow-on-it".

	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
	gDeviceContext->PSSetShaderResources(0, 1, &gTextureView);
	UINT32 vertexSize = sizeof(float) * 5;
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);



	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->Draw(12, 0);
}



void Engine::Update() {

	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;


	//world matrix
	static float radianRotation = 0.00;
	radianRotation += 0.0002;
	XMMATRIX worMat = XMMatrixRotationY(radianRotation);


	//view matrix
	XMVECTOR camPos = XMVectorSet(0, 0, -2, 1); //Explanation of w: http://gamedev.stackexchange.com/questions/17987/what-does-the-graphics-card-do-with-the-fourth-element-of-a-vector-as-the-final
	XMVECTOR lookAtVec = XMVectorSet(0, 0, 0, 1);
	XMVECTOR upVec = XMVectorSet(0, 1, 0, 1);
	XMMATRIX viewMat = XMMatrixLookAtLH(camPos, lookAtVec, upVec); //Makes a view-matrix <---- tHIS DOESN'T WORK FOR SOME REASON BECAUSE IM FUCKING RETARDED STUPID NIGGA SHIT FAGGS


																   //projection matrix
	float FOV = XM_PI * 0.45;
	float aspectRatio = 640.f / 480.f; //If you divide int by int, u get an int. Remember floats.
	float nearPlane = 0.5;
	float farPlane = 20;
	XMMATRIX projMat = XMMatrixPerspectiveFovLH(FOV, aspectRatio, nearPlane, farPlane);


	//Transpose the matrices. This is a must for DirectX 11
	worMat = XMMatrixTranspose(worMat);
	viewMat = XMMatrixTranspose(viewMat); //Transposes it, it's a requirement for DirectX11. You do this b4 sending it into the GPU (the shader)
	projMat = XMMatrixTranspose(projMat); //Transposes the matrix


	XMStoreFloat4x4(&worldMatrix, worMat);
	XMStoreFloat4x4(&viewMatrix, viewMat); //Stores the view matrix in a Float4x4-type variable. I wonder why.
	XMStoreFloat4x4(&projectionMatrix, projMat);

	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	matrixBuffer* dataPtr;

	gDeviceContext->Map(gConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);
	dataPtr = (matrixBuffer*)gMappedResource.pData;

	dataPtr->worldMatrix = worldMatrix;
	dataPtr->viewMatrix = viewMatrix;
	dataPtr->projectionMatrix = projectionMatrix;

	gDeviceContext->Unmap(gConstantBuffer, NULL);
}

//LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch (message)
//	{
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//	}
//
//	return DefWindowProc(hWnd, message, wParam, lParam);
//}
//
//HWND InitWindow(HINSTANCE hInstance)
//{
//	WNDCLASSEX wcex = { 0 };
//	wcex.cbSize = sizeof(WNDCLASSEX);
//	wcex.style = CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc = WndProc;
//	wcex.hInstance = hInstance;
//	wcex.lpszClassName = L"BTH_D3D_DEMO";
//	if (!RegisterClassEx(&wcex))
//		return false;
//
//	RECT rc = { 0, 0, 640, 480 };
//	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
//
//	HWND handle = CreateWindow(
//		L"BTH_D3D_DEMO",
//		L"BTH Direct3D Demo",
//		WS_OVERLAPPEDWINDOW,
//		CW_USEDEFAULT,
//		CW_USEDEFAULT,
//		rc.right - rc.left,
//		rc.bottom - rc.top,
//		nullptr,
//		nullptr,
//		hInstance,
//		nullptr);
//
//	return handle;
//}


HRESULT Engine::CreateDirect3DContext(HWND wndHandle)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
															// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{

		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();
		//gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, NULL);

		CreateDepthStencilBuffer();
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, depthStencilView);

	}
	return hr;
}

void Engine::Clean() {
	gVertexBuffer->Release();

	gVertexLayout->Release();
	gVertexShader->Release();
	gPixelShader->Release();
	gGeometryShader->Release();

	gBackbufferRTV->Release();
	gSwapChain->Release();
	gDevice->Release();
	gDeviceContext->Release();


	depthStencilView->Release();
	gDepthStencilBuffer->Release();
}

void Engine::Initialize(HWND wndHandle) {
	CreateDirect3DContext(wndHandle);

	SetViewport();

	CreateShaders();

	CreateTriangleData();

	CreateTexture();

	CreateConstantBuffer();
}