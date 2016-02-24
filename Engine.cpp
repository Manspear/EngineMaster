#include "Engine.h"
#include "bth_image.h" //This header wouldn't work in Engine.h VS complained
#include "FbxDawg.h"					   //of one or more multiply defined symbols found



#pragma region Texture includes
#include "WICTextureLoader.h"
#include <wincodec.h>

#pragma endregion

Engine::Engine()
{
	//EMPTY
}


Engine::~Engine()
{


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
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
	int shit = fbxobj->modelVertexList.size();
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = fbxobj->modelVertexList.size()*sizeof(MyVertexStruct);//fbxobj->modelVertexList.size()*sizeof(MyVertexStruct);//250 000 verticer * byte-storleken p� en vertex f�r att f� den totala byten

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = fbxobj->modelVertexList.data();
	gDevice->CreateBuffer(&bufferDesc, &data, &gVertexBuffer);
}

void Engine::CreateTexture() {
	#pragma region // Import texture from memory
	HRESULT hr;
	
	#pragma endregion

	#pragma region //Import from File
	ID3D11ShaderResourceView * Texture;
	CoInitialize(NULL);

	hr = CreateWICTextureFromFile(gDevice, fbxobj->textureFilepath.c_str(), NULL, &gTextureView[0]);//wstring �ger functionen c_str som �r en getFucntion till wchar_t* som finns redan
	hr = CreateWICTextureFromFile(gDevice, L"./Images/normal_map.jpg", NULL, &gTextureView[1]);
	//(d3d11DeviceInterface, d3d11DeviceContextInterface, L"test.bmp", 0, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0, 0, &pTex2D, NULL);
	#pragma endregion 
	

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
	//vertex shaders, 1 f�r animation, 1 f�r ej animation, 1 f�r specialeffekter
	//Specialeffekter: 1 egen vertex shader, 1 egen geometry-shader, 1 egen pixel shader (om annan ljuss�ttning kr�vs)
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
	gDeviceContext->PSSetShaderResources(0, 2, gTextureView);
	UINT32 vertexSize = sizeof(float) * 8;
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->Draw(fbxobj->modelVertexList.size(), 0);
}



void Engine::Update() {

	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	//world matrix
	static float radianRotation = 0.00;
	radianRotation += 0.0002;
	XMMATRIX worMat = XMMatrixRotationY(radianRotation);

	//Transpose the matrices. This is a must for DirectX 11
	worMat = XMMatrixTranspose(worMat);

	input->getKeyboardState();
	input->GetMouseLoc();

	if (input->keyState[DIK_LALT])
	{
		camera->rotate(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), input->mouseY);
		camera->rotate(XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f), -input->mouseX);
	}

	// kryssprodukten mellan upp vektor och riktings vektorn ger sidleds vektorn.
	if (input->keyState[DIK_S])
		camera->moveForward(-0.001);
	if (input->keyState[DIK_A])
		camera->moveStrafe(0.001);
	if (input->keyState[DIK_D])
		camera->moveStrafe(-0.001);
	if (input->keyState[DIK_W])
		camera->moveForward(0.001);
	if (input->keyState[DIK_R])
		camera->reset();


	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	matrixBuffer* dataPtr;

	gDeviceContext->Map(gConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);
	dataPtr = (matrixBuffer*)gMappedResource.pData;

	dataPtr->worldMatrix = worMat;
	dataPtr->viewMatrix = XMMatrixTranspose(camera->getViewMatrix());
	dataPtr->projectionMatrix = XMMatrixTranspose(camera->getProjMatrix());

	gDeviceContext->Unmap(gConstantBuffer, NULL);
}

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

	if (fbxobj != nullptr)
	{
		delete fbxobj;
		//for every new, have a delete.
		//the destructor of FbxDawg is called before this destructor is called.
		//Destructors are called every time a variable runs out of scope.
		//Delete removes the memory allocated.
	}
	delete camera;
	delete input;
}
void Engine::InitializeCamera()
{
	camera = new GCamera;//										vv far plane	dessa �r 400 g�nger ifr�n varandra. Det �r okej att ha runt 10 000 - 20 000
	camera->InitProjMatrix(XM_PI * 0.45, wHEIGHT, wWIDTH, 0.05, 20);
	//													  ^^ near plane  f�rut var det 0.5 h�r
}

void Engine::Initialize(HWND wndHandle, HINSTANCE hinstance) {
	input = new GInput;

	const char* filePath = ".\\box3.fbx";
	fbxobj = new FbxDawg();
	fbxobj->loadModels(filePath);

	CreateDirect3DContext(wndHandle);

	SetViewport();

	input->initialize(hinstance, wndHandle, wWIDTH, wHEIGHT);

	CreateShaders();

	CreateTriangleData();

	CreateTexture();

	CreateConstantBuffer();

	InitializeCamera();

}