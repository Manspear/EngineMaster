#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext)
{
	party.x = 1.0f;
	party.y = 1.0f;
	party.z = 1.0f;

	party.nx = party.x;
	party.ny = party.y;
	party.nz = party.z;

	party.u = 0.0f;
	party.v = 0.0f;

	//vertexbuffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(particle);//fbxobj->modelVertexList.size()*sizeof(MyVertexStruct);//250 000 verticer * byte-storleken på en vertex för att få den totala byten
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &party;

	gDevice->CreateBuffer(&bufferDesc, &data, &particleVertexBuffer);


	//constantbuffer
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


}