#include "GFrustum.h"
//BoundingFrustum functions
//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.directxmath.boundingfrustum(v=vs.85).aspx

GFrustum::GFrustum()
{

}

GFrustum::~GFrustum()
{

}

void GFrustum::updateFrustum(const DirectX::XMMATRIX &cameraProjection)
{
	//Now make the Frustum follow the camera. How? By creating it with the camera's Frustum!
	frustum.CreateFromMatrix(frustum, cameraProjection);
	
	//collision-detection. Loop through bounding-boxes, doing the frustum.contains(boundingbox)
	//To create a bounding-box you need: a list of vertices that the bounding-box need to include. 
	//The list is of type XMFLOAT3
}

