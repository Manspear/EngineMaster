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
	//frustum.CreateFromMatrix(frustum, cameraProjection);
	//frustum.Transform(frustum, cameraView); 

	//Manual frustum inc. 
	//these are transposed, since they're both left-handed
	//And they're in the right order (supposedly...)
	XMMATRIX fMatrix =  cameraView * cameraProjection;
	
	/*XMMATRIX projTest = cameraProjection;
	XMFLOAT4X4 projectionDerp;
	XMStoreFloat4x4(&projectionDerp, projTest);
	float zMin, r;
	zMin = -projectionDerp._43 / projectionDerp._33;
	r = 20 / (20 - zMin);
	projectionDerp._33 = r;
	projectionDerp._43 = -r * zMin;
	projTest = XMLoadFloat4x4(&projectionDerp);
	XMMATRIX TFMat;
	TFMat = cameraView * projTest;*/
	XMFLOAT4X4 TFM;
	XMStoreFloat4x4(&TFM, fMatrix);
	//get the planes
	//I get nearly no d-variable. I only get d for the near and far planes
	//when the matrices are non-transposed I get larger d-values.
	//All of the planes need D-values, or else the planes lie in the origin
	//and nothing can fit inside the origin... Duh.
	
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
	fPlanes[2].normal.x = -(TFM._14 + TFM._12);
	fPlanes[2].normal.y = -(TFM._24 + TFM._22);
	fPlanes[2].normal.z = -(TFM._34 + TFM._32);
	fPlanes[2].distance = -(TFM._44 + TFM._42);
	//bottom plane
	fPlanes[3].normal.x = -(TFM._14 - TFM._12);
	fPlanes[3].normal.y = -(TFM._24 - TFM._22);
	fPlanes[3].normal.z = -(TFM._34 - TFM._32);
	fPlanes[3].distance = -(TFM._44 - TFM._42);
	
	//DirectX doesn't have a whole unit-cube in clip-space.
	//So it didn't need the multiplication with near and far plane.
	//Should've just looked at some matrix math from the start.

	//near plane
	fPlanes[4].normal.x = -(TFM._13);
	fPlanes[4].normal.y = -(TFM._23);
	fPlanes[4].normal.z = -(TFM._33);
	fPlanes[4].distance = -(TFM._43); 
	//far plane
	fPlanes[5].normal.x = -(TFM._14 - TFM._13);
	fPlanes[5].normal.y = -(TFM._24 - TFM._23);
	fPlanes[5].normal.z = -(TFM._34 - TFM._33);
	fPlanes[5].distance = -(TFM._44 - TFM._43);

	//normalize the planes

	for (int i = 0; i < 6; i++)
	{
		float denom = 1.0f / retLength(fPlanes[i].normal);
		fPlanes[i].normal.x = fPlanes[i].normal.x * denom;
		fPlanes[i].normal.y = fPlanes[i].normal.y * denom;
		fPlanes[i].normal.z = fPlanes[i].normal.z * denom;
		//You also need to normalize the distance-variable... For DirectX!
		fPlanes[i].distance = fPlanes[i].distance * denom;
	}

	//Question: If I've made a frustum, can I change it's matrix and plane positions
	//by multiplying them with just the new, updated view-frustum? Or do I have to  
	//remake my frustum entirely every frame?
	//Answer: You have to remake this every frame. 
}

/*bool GFrustum::isCollision(const DirectX::BoundingBox& modelBBox)
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
}*/

bool GFrustum::hasCollided(GBoundingBox& modelBox)
{
	for (int planeC = 0; planeC < 6; planeC++) {
		//Check this every frustum-plane against all of the points of the cube. If any point is inside the current plane --> intersection for this plane is true.
		//Then go through the rest of the planes. As long as there are boxpoint(s) inside all planes, we have an intersection!
		bool intersection = false;
		for (int i = 0; i < 8; i++) {
			if (((fPlanes[planeC].normal.x) * (modelBox.vertices.BBoxPoint[i].x) + (fPlanes[planeC].normal.y) * (modelBox.vertices.BBoxPoint[i].y) + (fPlanes[planeC].normal.z) * (modelBox.vertices.BBoxPoint[i].z) + fPlanes[planeC].distance) <= 0) {
				intersection = true;
				break; //break finds the closest loop.
			}
		}
		if (intersection == false) {
			return false;
		}
	}
	return true; //If we haven't already returned false, we have an intersection!
	//if nothing intersected, no collision was made.
	printf("Non-Collision!\n");
	return false;
}

