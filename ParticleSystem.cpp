#include "ParticleSystem.h"
#include <time.h>

ParticleSystem::ParticleSystem(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, ID3D11Buffer* vp)
{

	particleCount = 0;
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
	this->gConstantBuffer = vp;


	particleWorldMatrix = XMMatrixTranspose(XMMatrixTranslation(4.0f, 1.0f, 0.0f));


	this->initializeParticles();
	this->initializeBuffers();

}

ParticleSystem::~ParticleSystem()
{
	gGeometryShaderParticle->Release();
	gPixelShaderParticle->Release();
	gVertexLayoutParticle->Release();
	gVertexShaderParticle->Release();

	delete[] particleList;
	delete[] partyDataList;
}


void ParticleSystem::doShit(float dTime)
{
	this->killParticles();
	this->emitParticles(dTime);
	this->updateParticles(dTime);
	this->updateBuffers();
}

void ParticleSystem::killALL()
{
	for (int i = 0; i < (maxParticles); i++)
	{
		if (particleList[i].active == true)
		{
			particleList[i].active = false;
			particleCount--;
		}
	}
}

void ParticleSystem::killParticles()
{

	for (int i = 0; i < (maxParticles); i++)
	{
		//if ((particleList[i].data.pos.y < -3.0f) && particleList[i].active == true)
		if ((particleList[i].y < -3.0f) && particleList[i].active == true)
		{
			particleList[i].active = false;
			particleCount--;
		}
	}
}

void ParticleSystem::updateBuffers()
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	vertexData * dataPtr;



	hr = gDeviceContext->Map(particleVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gMappedResource);
	dataPtr = (vertexData*)gMappedResource.pData;
	memcpy(dataPtr, (void*)partyDataList, (sizeof(vertexData)*particleCount));
	gDeviceContext->Unmap(particleVertexBuffer, 0);


	return;
}

void ParticleSystem::updateParticles(float dTime)
{
	for (int i = 0; i < maxParticles; i++)
	{
		if (particleList[i].active == true)
		{
			particleList[i].y = particleList[i].y - particleList[i].velocity * dTime * 0.01;
			//particleList[i].data.pos.y = particleList[i].data.pos.y - particleList[i].velocity * dTime * 0.01;

			partyDataList[i].pos = XMFLOAT3(particleList[i].x, particleList[i].y, particleList[i].z);
			partyDataList[i].color = XMFLOAT4(particleList[i].r, particleList[i].g, particleList[i].b, particleList[i].a);

		}
	}



	return;
}

void ParticleSystem::initializeParticles()
{
	deviationX = 2.0f;
	deviationY = 0.5f;
	deviationZ = 2.0f;

	velocity = 50.0f;
	velocityVariation = 20.0f;

	particlesPerSecond = 5000.0f;

	size = 0.05;
	sizeDeviation = 0.1;
	maxParticles = 50000;


	this->particleList = new particle[this->maxParticles];

	for (int i = 0; i < maxParticles; i++)
	{
		particleList[i].active = false;
	}

	this->partyDataList = new vertexData[this->maxParticles];

	particleCount = 0;
	accumulatedTime = 0.0f;
}

void ParticleSystem::emitParticles(float dTime)
{
	float posX, posY, posZ;
	float red, green, blue;
	float vel;
	float siz;
	bool emit, found;
	int index;

	accumulatedTime += dTime;

	emit = false;

	if (accumulatedTime > (1.0f / particlesPerSecond))
	{
		accumulatedTime = 0.0f;
		emit = true;
	}

	//randomize a particle

	if ((emit == true) && (particleCount < (maxParticles)))
	{
		particleCount++;



		//randomize a particle
		posX = (((float)rand() - (float)rand()) / RAND_MAX) * deviationX;
		posY = (((float)rand() - (float)rand()) / RAND_MAX) * deviationY;
		posZ = (((float)rand() - (float)rand()) / RAND_MAX) * deviationZ;

	

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX)+ 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX)+ 0.5f;


		siz = size + (((float)rand() - (float)rand()) / RAND_MAX) * sizeDeviation;
		vel = this->velocity + (((float)rand() - (float)rand()) / RAND_MAX) * velocityVariation;

		found = false;
		index = 0;

		while (!found)
		{
			if ((particleList[index].active == false))
				found = true;
			else
				index++;
		}
		particleList[index].x = posX;
		particleList[index].y = posY;
		particleList[index].z = posZ;
		particleList[index].r = red;
		particleList[index].g = green;
		particleList[index].b = blue;
		particleList[index].a = siz;

		particleList[index].velocity = vel;
		particleList[index].active = true;

	}

	return;
}



void ParticleSystem::initializeBuffers()
{
	HRESULT hr;

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"PixelShaderParticle.hlsl", // filename
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

	hr = gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShaderParticle);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();

	ID3DBlob* pGSps = nullptr;
	D3DCompileFromFile(
		L"GeometryShaderParticle.hlsl",
		nullptr,
		nullptr,
		"main",
		"gs_4_0",
		0,
		0,
		&pGSps,
		nullptr
		);
	hr = gDevice->CreateGeometryShader(pGSps->GetBufferPointer(), pGSps->GetBufferSize(), nullptr, &gGeometryShaderParticle);
	pGSps->Release();

	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"VertexShaderParticle.hlsl", // filename
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
	hr = gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShaderParticle);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayoutParticle);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	//memset(&partyDataList, 0, sizeof(vertexData)*maxParticles);

	//vertexbuffer
	D3D11_BUFFER_DESC bufferDescV;
	memset(&bufferDescV, 0, sizeof(bufferDescV));
	bufferDescV.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescV.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescV.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescV.ByteWidth = sizeof(vertexData)*maxParticles;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = partyDataList;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&bufferDescV, &data, &particleVertexBuffer);

	//constantbuffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(modelWorldStruct);
	gDevice->CreateBuffer(&bufferDesc, NULL, &particleConstantBuffer);

	//Giving the constant buffer data

	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	modelWorldStruct* dataPtr;

	hr = gDeviceContext->Map(particleConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);
	dataPtr = (modelWorldStruct*)gMappedResource.pData;

	dataPtr->worldMatrix = particleWorldMatrix;

	gDeviceContext->Unmap(particleConstantBuffer, NULL);
}



void ParticleSystem::renderParticles()
{

	UINT vertSize = sizeof(vertexData);
	UINT offset = 0;

	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShaderParticle, nullptr, 0);
	gDeviceContext->VSSetShader(gVertexShaderParticle, nullptr, 0);
	gDeviceContext->IASetInputLayout(gVertexLayoutParticle);
	gDeviceContext->PSSetShader(gPixelShaderParticle, nullptr, 0);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->GSSetConstantBuffers(1, 1, &particleConstantBuffer);
	gDeviceContext->IASetVertexBuffers(0, 1, &particleVertexBuffer, &vertSize, &offset);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//gDeviceContext->Draw(1, 0);
	gDeviceContext->Draw(particleCount, 0);

}




