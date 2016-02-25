#pragma once
#include <directXMath.h>
#include <DirectXCollision.h>
//BoundingFrustum functions
//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.directxmath.boundingfrustum(v=vs.85).aspx

class GFrustum
{
public:
	GFrustum();
	~GFrustum();

	DirectX::BoundingFrustum frustum;
	DirectX::XMFLOAT3* cornerList;
	//static DirectX::XMVECTOR HomogenousPoints[6];
	bool GFrustum::isCollision(const DirectX::BoundingBox& modelBBox);//<- updates frustum position, and checks for intersection
	void updateFrustumPos(const DirectX::XMMATRIX &cameraProjection);//will add a "boundingbox-list" as paremeter to updateFrustum. Or make a collision-function that updateFrustum calls.
private:

};

