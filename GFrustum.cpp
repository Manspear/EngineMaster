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
	//Manual frustum inc. 
	//these are transposed, since they're both left-handed
	//And they're in the right order (supposedly...)
	XMMATRIX fMatrix =  cameraView * cameraProjection;
	
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
}

bool GFrustum:: quadTreeCollided(GQuadTreeBoundingBox* modelBox)
{
	for (int planeC = 0; planeC < 6; planeC++) {
		bool intersection = false;
		for (int i = 0; i < 8; i++) {
			if (((fPlanes[planeC].normal.x) * (modelBox->vertices.BBoxPoint[i].x) + (fPlanes[planeC].normal.y) * (modelBox->vertices.BBoxPoint[i].y) + (fPlanes[planeC].normal.z) * (modelBox->vertices.BBoxPoint[i].z) + fPlanes[planeC].distance) <= 0) {
				intersection = true;
				break; 
			}
		}
		if (intersection == false) {
			return false;
		}
	}
	return true;
}
bool GFrustum::hasCollided(GBoundingBox* modelBox)
{
	//FIX THIS THING!
	for (int planeC = 0; planeC < 6; planeC++) {
		//Check this every frustum-plane against all of the points of the cube. If any point is inside the current plane --> intersection for this plane is true.
		//Then go through the rest of the planes. As long as there are boxpoint(s) inside all planes, we have an intersection!
		bool intersection = false;
		for (int i = 0; i < 8; i++) {
			if (((fPlanes[planeC].normal.x) * (modelBox->vertices.BBoxPoint[i].x) + (fPlanes[planeC].normal.y) * (modelBox->vertices.BBoxPoint[i].y) + (fPlanes[planeC].normal.z) * (modelBox->vertices.BBoxPoint[i].z) + fPlanes[planeC].distance) <= 0) {
				intersection = true;
				break; //break finds the closest loop.
			}
		}
		if (intersection == false) {
			//printf("Non-Collision!\n");
			return false;
		}
	}
	return true; //If we haven't already returned false, we have an intersection!
	//if nothing intersected, no collision was made.
}
void GFrustum::QuadTreeCollision(GQuadTreeBoundingBox* rootBox, bool startCollision) //The loops herein could be hairy... Also: alway have the "original root" as input.
{
	if (startCollision) {
		//every time you start over this collision-detection, the previous list needs to be cleared.
		seenObjects.clear();

	}
	startCollision = false;
	//Here check collision against all of the GQuadTreeBoundingBox-children.
	for (int i = 0; i < 4; i++) 
	{
		if (quadTreeCollided(rootBox->GQTBBoxChildren[i])) //if the frustum has collided with the QuadTreeBBox
		{
			if (rootBox->GQTBBoxChildren[i]->hasSplit) //Check if this QuadTreeBBox has split
			{
				//loop again through it's children using this QuadTreeCollision-function.
				QuadTreeCollision(rootBox->GQTBBoxChildren[i], startCollision);
			}
			else //check for object-collisions. 
				 //Since only the lowest box-division has them, objects are found in the boxes that haven't split.
			{
				for (int c = 0; c < rootBox->GQTBBoxChildren[i]->modelChildrenCounter; c++) { //for each model in this bbox...
					bool modelSeen = hasCollided(&rootBox->GQTBBoxChildren[i]->modelChildren[c]->bBox); //Check for collision with this object's bbox.
					if (modelSeen) {
						seenObjects.push_back(rootBox->GQTBBoxChildren[i]->modelChildren[c]);
						rootBox->GQTBBoxChildren[i]->modelChildren[c]->isCulled = false;
					}
				}
			}
		}
	}
}

