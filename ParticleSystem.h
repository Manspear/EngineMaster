#pragma once
#ifndef PARTICLE_SYSTEMS_H
#define PARTICLE_SYSTEMS_H

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class ParticleSystem
{
private:
	struct modelWorldStruct {
		XMMATRIX worldMatrix;
	};
	struct particle
	{
		float x, y, z, 
			  nx, ny, nz, 
			  u, v;
	};

	ID3D11Buffer* particleConstantBuffer = nullptr;
	ID3D11Buffer* particleVertexBuffer = nullptr;
	XMMATRIX particleWorldMatrix;
	int maxParticles;
	float gTime;
	float timeStep;
	float age;

public:

	XMFLOAT4X4 eyePosW;
	XMFLOAT4X4 emitPosW;
	XMFLOAT4X4 emitDirW;

	particle party;

	ParticleSystem(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext);
	~ParticleSystem();

};





#endif