#include "Engine.h"
//#include "bth_image.h" //This header wouldn't work in Engine.h VS complained
//of one or more multiply defined symbols found

#pragma region Texture includes
#include "WICTextureLoader.h"
#include <wincodec.h>

#pragma endregion

Engine::Engine()
{
	countsPerSecond = 0;
	counterStart = 0;
	frameCount = 0;
	fps = 0;
	frameTimeOld = 0;
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
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	ID3DBlob* pSVS = nullptr;
	D3DCompileFromFile(
		L"SkeletalVertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VSSkeletal_main",		// entry point
		"vs_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pSVS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);


	gDevice->CreateVertexShader(pSVS->GetBufferPointer(), pSVS->GetBufferSize(), nullptr, &gVertexShaderSkeletal);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDescSkeletal[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	gDevice->CreateInputLayout(inputDescSkeletal, ARRAYSIZE(inputDescSkeletal), pSVS->GetBufferPointer(), pSVS->GetBufferSize(), &gVertexLayoutSkeletal);
	// we do not need anymore this COM object, so we release it.
	pSVS->Release();

	//create vertex shader
	ID3DBlob* pVSbs = nullptr;
	D3DCompileFromFile(
		L"VertexShaderBS.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pVSbs,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);


	gDevice->CreateVertexShader(pVSbs->GetBufferPointer(), pVSbs->GetBufferSize(), nullptr, &gVertexShaderBS);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDescBS[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HRESULT hr = gDevice->CreateInputLayout(inputDescBS, ARRAYSIZE(inputDescBS), pVSbs->GetBufferPointer(), pVSbs->GetBufferSize(), &gVertexLayoutBS);
	// we do not need anymore this COM object, so we release it.

	pVSbs->Release();

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

	//Creating a Sampler for the Pixel Shader
	D3D11_SAMPLER_DESC sampDesc;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.MipLODBias = 0;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; //Maybe this does something bad? Hmm... Keep an eye out.
	sampDesc.BorderColor[0] = 1.f;
	sampDesc.BorderColor[1] = 1.f;
	sampDesc.BorderColor[2] = 0.f;
	sampDesc.BorderColor[3] = 1.f; //Not sure why this RGB value's w value must be 1... Alpha maybe?
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 12;

	hr = gDevice->CreateSamplerState(&sampDesc, &gPSTextureSampler);

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

void Engine::Render()
{
#pragma region //EXPLANATION OF DEPTH-BUFFER AND IT'S RELATIONSHIP WITH VIEW-FRUSTUM
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
#pragma endregion
	//vertex shaders, 1 f�r animation, 1 f�r ej animation, 1 f�r specialeffekter
	//Specialeffekter: 1 egen vertex shader, 1 egen geometry-shader, 1 egen pixel shader (om annan ljuss�ttning kr�vs)

	float clearColor[] = { 1, 0, 0.5, 1 };
	float clearColor2[] = { 1,0.5,0,1 };

	if (this->mousePickEffectOnClearColor == true)
		gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);

	else
		gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor2);

	gDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);

	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
	gDeviceContext->PSSetSamplers(0, 1, &gPSTextureSampler);
	
	UINT32 vertexSize;
	UINT32 offset = 0; 

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

																					   
	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);					   
																					   
	listOfModels = modelListObject.getModelList();									   
																					   
	listOfModels[0].animationTime;
	//bool isRoot = true;
	//cullingFrustum->updateFrustumPos(camera->getProjMatrix(), camera->getViewMatrix());
	//cullingFrustum->QuadTreeCollision(&quadTreeRoot->rootBox, isRoot);
	////rootBox has 2 modelChildren, but the bbox-children of rootBox have no modelchildren. Odd that.
	//		gDeviceContext->VSSetConstantBuffers(0, 1, &listOfModels[bufferCounter].bsWBuffer);
	//		gDeviceContext->VSSetShader(gVertexShaderBS, nullptr, 0);
	//		gDeviceContext->IASetInputLayout(gVertexLayoutBS);
	//		vertexSize = sizeof(float) * 16;
	//	}else{
	//		gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	//		gDeviceContext->IASetInputLayout(gVertexLayout);
	//		vertexSize = sizeof(float) * 8;
	//	}														  	 
																  	 
																  	 
	//struct frustumVert {										  	 
	//		float x, y, z, xn, yn, zn, u, v;					  	 
	//	};														  	 
	//	frustumVert frustumVertices[6]; //...					  	 
																  	 
																	 

	for (int bufferCounter = 0; bufferCounter < cullingFrustum->seenObjects.size(); bufferCounter++)
	{

		//printf("%d", cullingFrustum->seenObjects.size());
		if (cullingFrustum->seenObjects[bufferCounter]->hasBlendShape())
		{
			
			gDeviceContext->VSSetShader(gVertexShaderBS, nullptr, 0);
			gDeviceContext->IASetInputLayout(gVertexLayoutBS);
			gDeviceContext->VSSetConstantBuffers(0, 1, &cullingFrustum->seenObjects[bufferCounter]->bsWBuffer);
			vertexSize = sizeof(float) * 16;
		}
		else if (cullingFrustum->seenObjects[bufferCounter]->isAnimated())
		{
			//the frustum never registers collision with an animated mesh
			int poop = 5;
			cullingFrustum->seenObjects[bufferCounter]->updateAnimation(gDeviceContext);
		}
		else
		{
			gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
			gDeviceContext->IASetInputLayout(gVertexLayout);
			vertexSize = sizeof(float) * 8 + sizeof(int);
		}


			
		gDeviceContext->GSSetConstantBuffers(1, 1, &cullingFrustum->seenObjects[bufferCounter]->modelConstantBuffer); //each model only one vertex buffer. Exceptions: Objects with separate parts, think stone golem with floating head, need one vertex buffer per separate geometry.

		gDeviceContext->PSSetShaderResources(0, 2, cullingFrustum->seenObjects[bufferCounter]->modelTextureView);

		gDeviceContext->IASetVertexBuffers(0, 1, &cullingFrustum->seenObjects[bufferCounter]->modelVertexBuffer, &vertexSize, &offset);
		gDeviceContext->IASetIndexBuffer(cullingFrustum->seenObjects[bufferCounter]->modelIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		gDeviceContext->DrawIndexed(cullingFrustum->seenObjects[bufferCounter]->sizeOfIndexArray, 0, 0);
	}

	particleSys->renderParticles();

	//{
	//	//if (!cullingFrustum->isCollision(listOfModels[bufferCounter].modelBBox))
	//	//	continue; //skips one loop iteration, not sending vertexbuffers to the shader. (if the frustum doesn't contain the mesh)
	//	if (!cullingFrustum->hasCollided(listOfModels[bufferCounter].bBox))
	//		continue;
	//	gDeviceContext->GSSetConstantBuffers(1, 1, &listOfModels[bufferCounter].modelConstantBuffer);

	//	//each model only one vertex buffer. Exceptions: Objects with separate parts, think stone golem with floating head, need one vertex buffer per separate geometry.
	//	gDeviceContext->PSSetShaderResources(0, 2, listOfModels[bufferCounter].modelTextureView);

	//	gDeviceContext->IASetVertexBuffers(0, 1, &listOfModels[bufferCounter].modelVertexBuffer, &vertexSize, &offset);

	//	gDeviceContext->Draw(listOfModels[bufferCounter].modelVertices.size(), 0);
	//}
}



void Engine::Update()
{


	frameCount++;
	if (getTime() > 1.0f)
	{
		fps = frameCount;
		frameCount = 0;
		startTimer();
	}
	dt = getFrameTime();

	particleSys->doShit(dt);
	//XMFLOAT4X4 viewMatrix;
	//XMFLOAT4X4 projectionMatrix;

	//world matrix
	//static float radianRotation = 0.00;
	//radianRotation += 4500*dt;
	//XMMATRIX worMat = DirectX::XMMatrixRotationY(radianRotation);

	//Transpose the matrices. This is a must for DirectX 11
	//worMat = DirectX::XMMatrixTranspose(worMat);
	//dataPtr->worldMatrix = worMat;
#pragma region Camera
	input->getKeyboardState();
	input->GetMouseLoc();

	if (input->keyState[DIK_LCONTROL])
	{
		camera->rotate(XAXIS, input->mouseX*MOUSE_SENSITIVITY*dt);
		camera->rotate(YAXIS, -input->mouseY*MOUSE_SENSITIVITY*dt);
	}
	static float weight;
	if (input->keyState[DIK_UPARROW])
	{
		weight += 0.2*dt;
		if (weight > 1.0)
			weight = 1.0;
		listOfModels[2].setBlendWeight(weight, gDeviceContext);
	}
	if (input->keyState[DIK_DOWNARROW])
	{
		weight -= 0.2*dt;
		if (weight < 0.0)
			weight = 0.0;
		listOfModels[2].setBlendWeight(weight, gDeviceContext);
	}

	// kryssprodukten mellan upp vektor och riktings vektorn ger sidleds vektorn.
	if (input->keyState[DIK_S])
		camera->moveForward(-MOVESPEED*dt);
	if (input->keyState[DIK_A])
		camera->moveStrafe(MOVESPEED*dt);
	if (input->keyState[DIK_D])
		camera->moveStrafe(-MOVESPEED*dt);
	if (input->keyState[DIK_W])
		camera->moveForward(MOVESPEED*dt);
	if (input->keyState[DIK_R])
		camera->reset();


	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	matrixBuffer* dataPtr;

	gDeviceContext->Map(gConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);
	dataPtr = (matrixBuffer*)gMappedResource.pData;


	dataPtr->viewMatrix = DirectX::XMMatrixTranspose(camera->getViewMatrix());
	dataPtr->projectionMatrix = DirectX::XMMatrixTranspose(camera->getProjMatrix());
	dataPtr->camPos = camera->getPosition();
	dataPtr->camDir = camera->getCameraDirection();

	gDeviceContext->Unmap(gConstantBuffer, NULL);
#pragma endregion Camera

#pragma region FrustumCulling
	bool startCollision = true;

	cullingFrustum->updateFrustumPos(camera->getProjMatrix(), camera->getViewMatrix());
	cullingFrustum->QuadTreeCollision(&quadTreeRoot->rootBox, startCollision);
#pragma endregion FrustumCulling

	MousePickingObject->updateClass(cullingFrustum->seenObjects, this->mousePickEffectOnClearColor);


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
		D3D11_CREATE_DEVICE_DEBUG,
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

	gVertexLayout->Release();
	gVertexShader->Release();
	gPixelShader->Release();
	gGeometryShader->Release();
	delete particleSys;

	gBackbufferRTV->Release();
	gSwapChain->Release();
	gDevice->Release();
	gDeviceContext->Release();

	gPSTextureSampler->Release();

	depthStencilView->Release();
	gDepthStencilBuffer->Release();

	delete camera;
	delete input;
	delete cullingFrustum;
	delete quadTreeRoot;
	delete MousePickingObject;
	//delete[] loops through the new-objects in the array, and deletes them!
}
void Engine::InitializeCamera()
{
	camera = new GCamera;//										vv far plane	dessa �r 400 g�nger ifr�n varandra. Det �r okej att ha runt 10 000 - 20 000
	camera->InitProjMatrix(XM_PI * 0.5, wHEIGHT, wWIDTH, 0.1, 20);
	//													  ^^ near plane  f�rut var det 0.5 h�r
	//pi * 0.45, wHEIGHT, wWiDTH, 0.05, 20
}

void Engine::initializeMousePicking(HWND wndHandle)
{
	GModel* listOfModels = modelListObject.getModelList();
	MousePickingObject = new MousePicking(wndHandle, this->camera, this->wHEIGHT, this->wWIDTH);
}



void Engine::InitializeModels() {
	//Here create the dynamic GModel-Array:
	modelListObject.initializeModels(gDevice, gDeviceContext);
}

void Engine::InitializeQuadTree()
{
	quadTreeRoot = new GQuadTree;
	quadTreeRoot->makeTree(5, modelListObject);
	//modelList[3].setPosition(XMFLOAT4(-14, 0, 1, 1), gDeviceContext);
	//modelList[4].setPosition(XMFLOAT4(-7, 0, 1, 1), gDeviceContext);
}

void Engine::initializeParticles()
{
	particleSys = new ParticleSystem(gDevice, gDeviceContext, gConstantBuffer);
	//particleSys->setShaders(gVertexShader);
}

void Engine::Initialize(HWND wndHandle, HINSTANCE hinstance) {
	input = new GInput;

	CreateDirect3DContext(wndHandle);

	InitializeModels();

	SetViewport();

	input->initialize(hinstance, wndHandle, wWIDTH, wHEIGHT);

	CreateShaders();

	CreateConstantBuffer();

	InitializeCamera();

	initializeMousePicking(wndHandle);
	initializeParticles();

	InitializeQuadTree();

	InitializeFrustum();
}

void Engine::InitializeFrustum()
{
	cullingFrustum = new(GFrustum);
}

void Engine::renderText(std::wstring text)
{

}

void Engine::startTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	counterStart = frequencyCount.QuadPart;
}

double Engine::getTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - counterStart) / countsPerSecond;
}
double Engine::getFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount) / countsPerSecond;
}