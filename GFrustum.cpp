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

float retLength(XMFLOAT3 input) {
	return sqrt((input.x * input.x) + (input.y * input.y) + (input.z * input.z));
}

void GFrustum::updateFrustumPos(const DirectX::XMMATRIX & cameraProjection, const DirectX::XMMATRIX &cameraView)
{
	frustum.CreateFromMatrix(frustum, cameraProjection);
	frustum.Transform(frustum, XMMatrixInverse(nullptr, cameraView));

	//Manual frustum inc. 
	XMMATRIX fMatrix = cameraProjection * cameraView;
	XMFLOAT4X4 TFM;
	XMStoreFloat4x4(&TFM, fMatrix);
	//get the planes
	
	//left plane
	fPlanes[0].normal.x = -(TFM._14 + TFM._11);
	fPlanes[0].normal.y = -(TFM._24 + TFM._21);
	fPlanes[0].normal.z = -(TFM._34 + TFM._31);
	fPlanes[0].distance = -(TFM._44 + TFM._41);
	//right plane
	fPlanes[1].normal.x = -(TFM._14 - TFM._11);
	fPlanes[1].normal.y = -(TFM._24 - TFM._21);
	fPlanes[1].normal.z = -(TFM._34 - TFM._31);
	fPlanes[1].distance = -(TFM._44 - TFM._41);
	//top plane
	fPlanes[2].normal.x = -(TFM._14 - TFM._12);
	fPlanes[2].normal.y = -(TFM._24 - TFM._22);
	fPlanes[2].normal.z = -(TFM._34 - TFM._32);
	fPlanes[2].distance = -(TFM._44 - TFM._42);
	//bottom plane
	fPlanes[2].normal.x = -(TFM._14 + TFM._12);
	fPlanes[2].normal.y = -(TFM._24 + TFM._22);
	fPlanes[2].normal.z = -(TFM._34 + TFM._32);
	fPlanes[2].distance = -(TFM._44 + TFM._42);
	//near plane
	fPlanes[3].normal.x = -(TFM._14 + TFM._13);
	fPlanes[3].normal.y = -(TFM._24 + TFM._23);
	fPlanes[3].normal.z = -(TFM._34 + TFM._33);
	fPlanes[3].distance = -(TFM._44 + TFM._43);
	//far plane
	fPlanes[3].normal.x = -(TFM._14 - TFM._13);
	fPlanes[3].normal.y = -(TFM._24 - TFM._23);
	fPlanes[3].normal.z = -(TFM._34 - TFM._33);
	fPlanes[3].distance = -(TFM._44 - TFM._43);

	fPlanes[0].normal;
	//normalize the planes

	for (int i = 0; i < 6; i++)
	{
		float denom = 1.0f / retLength(fPlanes[i].normal);
		fPlanes[i].normal.x = fPlanes[i].normal.x * denom;
		fPlanes[i].normal.y = fPlanes[i].normal.y * denom;
		fPlanes[i].normal.z = fPlanes[i].normal.z * denom;
	}

	//Question: If I've made a frustum, can I change it's matrix and plane positions
	//by multiplying them with just the new, updated view-frustum? Or do I have to  
	//remake my frustum entirely every frame?
	//Answer: You have to remake this every frame. 
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
		printf("collision 0\n");
		
	}
	if (frustum.Contains(modelBBox) == 1) {
		//the values returned by the Contains()-function mean: 0 = not intersection 1 = intersects 2 = contains
		//read for return value meaning: https://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.directxcollision.containmenttype(v=vs.85).aspx
		collision = true;
		printf("collision 1\n");

	}
	if (frustum.Contains(modelBBox) == 2) {
		//the values returned by the Contains()-function mean: 0 = not intersection 1 = intersects 2 = contains
		//read for return value meaning: https://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.directxcollision.containmenttype(v=vs.85).aspx
		collision = true;
		printf("collision 2\n");

	}
	bool hasCollided = false;
	


	return collision;
}

