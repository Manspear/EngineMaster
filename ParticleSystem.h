#pragma once
#ifndef PARTICLE_SYSTEMS_H
#define PARTICLE_SYSTEMS_H

#include <d3d11.h>
#include <d3dcompiler.h> 
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
		float x, y, z;
	};
	ID3D11InputLayout* gVertexLayout = nullptr;
	ID3D11PixelShader* gPixelShader = nullptr;
	ID3D11VertexShader* gVertexShader = nullptr;
	ID3D11GeometryShader* gGeometryShaderParticle = nullptr;
	ID3D11DeviceContext* gDeviceContext;
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

	void renderParticles();
	void setShaders(ID3D11VertexShader* gVertexShader);
};





#endif