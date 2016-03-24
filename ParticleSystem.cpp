#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, ID3D11Buffer* vp)
{

	particleCount = 0;
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
	this->gConstantBuffer = vp;

	//TESTPARTICLE
	//will become the emitter position. to do this replace with translation multiplied in to world matrix.
	party.x = 0.0f;
	party.y = 1.0f;
	party.z = 0.0f;

	party.r = 1.0f;
	party.g = 0.0f;
	party.b = 0.0f;
	//END TESTPARTICLE

	particleWorldMatrix = XMMatrixTranspose(XMMatrixTranslation(party.x, party.y, party.z));
	

	this->initializeParticles();
	this->initializeBuffers();

}

ParticleSystem::~ParticleSystem()
{
	gGeometryShaderParticle->Release();
}


void ParticleSystem::doShit(float dTime)
{
	this->emitParticles(dTime);
	this->updateParticles(dTime);
	this->updateBuffers();
}

void ParticleSystem::updateBuffers()
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	vertexData * dataPtr;

	hr = gDeviceContext->Map(particleVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gMappedResource);
	dataPtr = (vertexData*)gMappedResource.pData;
	memcpy(dataPtr, &particleList->data, (sizeof(vertexData)*particleCount));
	gDeviceContext->Unmap(particleVertexBuffer, 0);

	return;
}

void ParticleSystem::updateParticles(float dTime)
{
	for (int i = 0; i < particleCount; i++)
	{
		particleList[i].y = (particleList[i].y - particleList[i].velocity * dTime * 0.001);
		particleList[i].data.x = particleList[i].x;
		particleList[i].data.y = particleList[i].y;
		particleList[i].data.z = particleList[i].z;

		particleList[i].data.r = particleList[i].r;
		particleList[i].data.g = particleList[i].g;
		particleList[i].data.b = particleList[i].b;
	}
	return;
}

void ParticleSystem::emitParticles(float dTime)
{
	float posX, posY, posZ;
	float red, green, blue;
	float velocity;
	bool emit,found;
	int index, i, j;

	accumulatedTime += dTime;

	emit = false;

	if (accumulatedTime > (1000.0f / particlesPerSecond))
	{
		accumulatedTime = 0.0f;
		emit = true;
	}

	if ((emit == true) && (particleCount < (maxParticles - 1)))
	{
		particleCount++;

		//randomize a particle
		posX = (((float)rand() - (float)rand()) / RAND_MAX) * deviationX;
		posY = (((float)rand() - (float)rand()) / RAND_MAX) * deviationY;
		posZ = (((float)rand() - (float)rand()) / RAND_MAX) * deviationZ;

		velocity = this->velocity + (((float)rand() - (float)rand()) / RAND_MAX) * velocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) +0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) +0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) +0.5f;

		//rendering from back to front seems like a good idea so let's sort shit!
		found = false;
		index = 0;

		while (!found)
		{
			if ((particleList[index].active == false) || (particleList[index].z < posZ))
				found = true;
			else
				index++;
		}

		i = particleCount;
		j = i - 1;

		while (i != index)
		{
			particleList[i].x = particleList[j].x;
			particleList[i].y = particleList[j].y;
			particleList[i].z = particleList[j].z;
			particleList[i].r = particleList[j].r;
			particleList[i].g = particleList[j].g;
			particleList[i].b = particleList[j].b;
			particleList[i].velocity = particleList[j].velocity;
			particleList[i].active = particleList[j].active;
			i--;
			j--;
		}
		particleList[index].x = posX;
		particleList[index].y = posY;
		particleList[index].z = posZ;
		particleList[index].r = red;
		particleList[index].g = green;
		particleList[index].b = blue;
		particleList[index].velocity = velocity;
		particleList[index].active = true;
	}
	return;
}



void ParticleSystem::initializeBuffers()
{
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

	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
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
	gDevice->CreateGeometryShader(pGSps->GetBufferPointer(), pGSps->GetBufferSize(), nullptr, &gGeometryShaderParticle);
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
	HRESULT hr = gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayoutParticle);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	//vertexbuffer
	D3D11_BUFFER_DESC bufferDescV;
	memset(&bufferDescV, 0, sizeof(bufferDescV));
	bufferDescV.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescV.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescV.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescV.ByteWidth = sizeof(vertexData)*maxParticles;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &particleList->data;

	gDevice->CreateBuffer(&bufferDescV, &data, &particleVertexBuffer);

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

	gDeviceContext->Map(particleConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);
	dataPtr = (modelWorldStruct*)gMappedResource.pData;

	dataPtr->worldMatrix = particleWorldMatrix;

	gDeviceContext->Unmap(particleConstantBuffer, NULL);
}

void ParticleSystem::initializeParticles()
{
	deviationX = 0.5f;
	deviationY = 0.1f;
	deviationZ = 2.0f;

	velocity = 1.0f;
	velocityVariation = 0.2f;

	size = 0.2f;

	particlesPerSecond = 250.0f;

	maxParticles = 5000;

	this->particleList = new particle[this->maxParticles];

	for (int i = 0; i < maxParticles; i++)
	{
		particleList[i].active = false;
	}

	particleCount = 0;
	accumulatedTime = 0.0f;
}

void ParticleSystem::renderParticles()
{
	UINT vertSize = sizeof(float) * 6;
	UINT offset = 0;

	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShaderParticle, nullptr, 0);
	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->IASetInputLayout(gVertexLayoutParticle);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->GSSetConstantBuffers(1, 1, &particleConstantBuffer);
	gDeviceContext->IASetVertexBuffers(0, 1, &particleVertexBuffer, &vertSize, &offset);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//gDeviceContext->Draw(1, 0);
	gDeviceContext->Draw(particleCount, 0);

}

void ParticleSystem::setShaders(ID3D11VertexShader * gVertexShader)
{
	this->gVertexShader = gVertexShader;
}


