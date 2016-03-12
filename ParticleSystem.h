#pragma once
#ifndef PARTICLE_SYSTEMS_H
#define PARTICLE_SYSTEMS_H

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class ParticleSystem
{
public:
	XMVECTOR eyePosW;
	XMVECTOR emitPosW;
	XMVECTOR emitDirW;

	XMFLOAT4X4 eyePosW;
	XMFLOAT4X4 emitPosW;
	XMFLOAT4X4 emitDirW;

private:
	int maxParticles;
	float gTime;
	float timeStep;
	float age;



};





#endif