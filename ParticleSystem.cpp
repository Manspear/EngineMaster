#include "ParticleSystem.h"



ParticleSystem::ParticleSystem(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext)
{
	this->gDeviceContext = gDeviceContext;

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


	party.x = 0.0f;
	party.y = 0.0f;
	party.z = -4.0f;

	party.nx = party.x;
	party.ny = party.y;
	party.nz = party.z;

	party.u = 0.0f;
	party.v = 0.0f;

	//vertexbuffer
	D3D11_BUFFER_DESC bufferDescV;
	memset(&bufferDescV, 0, sizeof(bufferDescV));
	bufferDescV.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescV.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescV.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescV.ByteWidth = sizeof(particle);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &party;

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

ParticleSystem::~ParticleSystem()
{
	gGeometryShaderParticle->Release();
}

void ParticleSystem::renderParticles()
{
	UINT vertSize = sizeof(float) * 8;
	UINT offset = 0;

	gDeviceContext->GSSetShader(gGeometryShaderParticle, nullptr, 0);
	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);

	gDeviceContext->GSSetConstantBuffers(1, 1, &particleConstantBuffer);
	gDeviceContext->IASetVertexBuffers(0, 1, &particleVertexBuffer, &vertSize, &offset);
	gDeviceContext->Draw(vertSize, offset);

}

void ParticleSystem::setShaders(ID3D11VertexShader * gVertexShader)
{
	this->gVertexShader = gVertexShader;
}
