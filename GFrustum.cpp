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
	//Uhmm...What to do when a collision has actually happened? I cannot do it here, can I? No. I gotta send back a bool to 
	, and there handle the "model-ignoring"	
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
bool GFrustum:: quadTreeCollided(GQuadTreeBoundingBox* modelBox)
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
			//printf("Non-Collision QUADTREE!\n");
			return false;
		}
	}
	//printf("Collision QUADTREE\n");
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
	//printf("Collision\n");
	return true; //If we haven't already returned false, we have an intersection!
	//if nothing intersected, no collision was made.

//if even one whole point is inside the frustum, we have an intersection.
//if all of the points are inside the frustum, we have a contain.
//if no point is inside the frustum, we have a nonCollision.
//if even one whole point is inside the frustum, we have an intersection.
//if all of the points are inside the frustum, we have a contain.
//if no point is inside the frustum, we have a nonCollision.


	//bool intersectionJudge[8] = { true, true, true, true, true, true, true, true }; //I never set this to true... Hmm...

	//bool containmentJudge[6];

	//bool contains = true;

	//for (int i = 0; i < 8; i++) //for every point in bBox
	//{
	//	//herein do the collision-test... See if the points of the cube are contained inside the frustum.
	//	//modelBox.vertices.BBoxPoint[i];
	//	for (int k = 0; k < 6; k++) //check against the frustum planes
	//	{
	//		float indicator = (fPlanes[k].normal.x) * (modelBox.vertices.BBoxPoint[i].x) + (fPlanes[k].normal.y) * (modelBox.vertices.BBoxPoint[i].y) + (fPlanes[k].normal.z) * (modelBox.vertices.BBoxPoint[i].z) + fPlanes[k].distance;
	//		if (indicator <= 0) //if the point is inside or behind this plane
	//			containmentJudge[k] = true;
	//		else
	//			containmentJudge[k] = false;

	//	}
	//	for (int p = 0; p < 6; p++) {
	//		//if any bool is false, no containment.
	//		if (!containmentJudge[p]) {
	//			contains = false;
	//			intersectionJudge[i] = false;
	//		}
	//	}
	//}
	//if (contains) {
	//	printf("Contains!\n");
	//	return true;
	//}
	//else
	//{
	//	for (int c = 0; c < 8; c++) {
	//		if (intersectionJudge[c]) { //if any point is inside the frustum, we have an intersection.
	//			printf("Intersects!\n");
	//			return true;
	//		}
	//	}
	//}
	////if nothing intersected, no collision was made.
	//printf("Non-Collision!\n");
	//return false;
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
					//else {
						//rootBox.GQTBBoxChildren[i].modelChildren[c]->isCulled = true;
					//}
					//Hmm... I am now looping through all bboxes, including model-boxes, 
					//registering detection with a bool called isCulled.
					//in Engine::Render() we'll loop through the complete modelList, 
					//and ignore rendering the models with the "isCulled == true" tag.
					//the bottleneck here could be the loop through all the models.
					//Solution is to add all non-culled objects into a list that gets rendered 
					//somehow. It must be done in this GFrustum-class. Otherwise we'll 
					//still have to loop through all models to assemble the list(that'd be even slower).
					
				}
			}
		}
	}
}

