#include "GFrustum.h"
#include <iostream>
//BoundingFrustum functions
//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.directxmath.boundingfrustum(v=vs.85).aspx
using namespace DirectX;
GFrustum::GFrustum()
{

}

GFrustum::~GFrustum()
{

}

void GFrustum::updateFrustumPos(const DirectX::XMMATRIX & cameraProjection, const DirectX::XMMATRIX & cameraView)
{
	//the camera-projection-frustum seem to need the camera's projection-matrix too. 
	//since the projection matrix doesn't move by itself. 
	frustum.CreateFromMatrix(frustum, cameraProjection); 

	frustum.Transform(frustum, XMMatrixInverse(nullptr, cameraView));//The view-matrix transforms between world to view-space
	
	//Sebastian said: matrices aren't ever IN a space, they transform between spaces.
	//nullptr is a stand-in for the determinant variable
	//the determinant is related to volume. The determinant is 1 for orthogonal unit vectors.
	//You get the volume of the 3D paraboloid. 
	//the parallelogram in 2D you get a from cross-product.

	collisionFrustum = cameraProjection * cameraView;
	//left clipping plane
	XMFLOAT4X4 CF;
	XMStoreFloat4x4(&CF, collisionFrustum);
	planes[0].normal.x = -(CF._14 + CF._11);
	planes[1].normal.y = -(CF._24 + CF._21);
	//follow the powerpoint!
}

bool GFrustum::isCollision(const DirectX::BoundingBox& modelBBox)
{
	//Now make the Frustum follow the camera. How? By creating it with the camera's Frustum!	
	//Uhmm...What to do when a collision has actually happened? I cannot do it here, can I? No. I gotta send back a bool to main, and there handle the "model-ignoring"	
	//will do it by simply not sending the vertex-buffer of the object into the vertex shader. Smart. Easy. Good thinking. Proud of you.
	bool collision = false;
	if (frustum.Contains(modelBBox) == 0) { 
		//the values returned by the Contains()-function mean: 0 = not intersection 1 = intersects 2 = contains
		//read for return value meaning: https://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.directxcollision.containmenttype(v=vs.85).aspx
		collision = false;
		printf("in 0\n");
	}
	else if (frustum.Contains(modelBBox) == 1) {
		collision = true; printf("in 1\n");
	}
	else {
		collision = true; printf("in 2\n");
	}
	return collision;
}

