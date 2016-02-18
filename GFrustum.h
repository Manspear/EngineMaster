#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
class GFrustum
{
public:
	GFrustum();
	~GFrustum();
	void ConstructFrustum(float nearPlane, float farplane, float topDownFOV, float aspectRatio);

private:

};

GFrustum::GFrustum()
{

}

GFrustum::~GFrustum()
{

}