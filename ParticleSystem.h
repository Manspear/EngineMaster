#pragma once
#ifndef PARTICLE_SYSTEMS_H
#define PARTICLE_SYSTEMS_H

#include <d3d11.h>
#include <d3dcompiler.h> 
#include <DirectXMath.h>

using namespace DirectX;

class ParticleSystem
{
public:
	struct vertexData
	{
		XMFLOAT3 pos;
		XMFLOAT4 color;
		//float x, y, z, r, g, b;
	};

	struct particle
	{
		float x, y, z, r, g, b, a;
		float velocity;
		bool active;
		//vertexData data;
	};

private:
	struct modelWorldStruct {
		XMMATRIX worldMatrix;
	};


	ID3D11InputLayout* gVertexLayoutParticle = nullptr;
	ID3D11PixelShader* gPixelShaderParticle = nullptr;
	ID3D11VertexShader* gVertexShaderParticle = nullptr;
	ID3D11GeometryShader* gGeometryShaderParticle = nullptr;
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext;
	ID3D11Buffer* particleConstantBuffer = nullptr;
	ID3D11Buffer* particleVertexBuffer = nullptr;
	ID3D11Buffer* gConstantBuffer = nullptr;
	XMMATRIX particleWorldMatrix;

	particle * particleList;
	vertexData * partyDataList;

	float deviationX, deviationY, deviationZ;
	float velocity, velocityVariation;
	float size, sizeDeviation;;
	float particlesPerSecond;

	int maxParticles;
	int particleCount;

	float gTime;
	float timeStep;
	float accumulatedTime;

public:
	ParticleSystem(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, ID3D11Buffer* vp);
	~ParticleSystem();

	void killALL();
	void killParticles();
	void doShit(float dTime);
	void updateBuffers();
	void updateParticles(float dTime);
	void emitParticles(float dTime);
	void initializeBuffers();
	void initializeParticles();
	void renderParticles();
};





#endif