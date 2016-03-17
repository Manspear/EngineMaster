#include "MousePicking.h"



MousePicking::MousePicking(HWND wndHandle, GCamera* camera, int screenHeight, int screenWidth)
{
	this->camera = camera;
	this->wndHandle2 = wndHandle;
	this->projectionMatrix = camera->getProjMatrix();
	this->viewMatrix = camera->getViewMatrix();
	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;
}

MousePicking::~MousePicking()
{
	 //Pass
}

void MousePicking::updateClass() //updates variables & checks ray
{
	
	this->projectionMatrix = this->camera->getProjMatrix();
	this->viewMatrix = this->camera->getViewMatrix();

	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0) /*if left mouse button down*/
	{
		//std::cout << projectionMatrix->;
		this->result = this->calculateCurrentRay();
		this->CheckBoundingBoxIntersection();
		
	}


}

bool MousePicking::getCursorPosition(POINT& MousePosSavedHere)
{
	
	
	this->result = GetCursorPos(&MousePosSavedHere); 

	if (this->result) /*The function call succeeded and curPos has valid data.*/
	{

		this->result = ScreenToClient(this->wndHandle2, &MousePosSavedHere);
		if (this->result = true)
		{

			if (MousePosSavedHere.x > this->screenWidth)
				MousePosSavedHere.x = this->screenWidth;

			else if (MousePosSavedHere.x < 0)
				MousePosSavedHere.x = 0;

			if (MousePosSavedHere.y > this->screenHeight)
				MousePosSavedHere.y = this->screenHeight;

			else if (MousePosSavedHere.y < 0)
				MousePosSavedHere.y = 0;

			return true;
		}


	}
	else
	{
		printf("Failed to get mouse position data\n");
		return false;
	}

}

bool MousePicking::calculateCurrentRay()
{

	//ViewportSpace
	MousePicking::getCursorPosition(this->MousePos); 
	
	//ViewSpace
	this->currentRay.x = (((2.0f * MousePos.x) / screenWidth) - 1) / this->projectionMatrix._11; 
	this->currentRay.y = -(((2.0f * MousePos.y) / screenHeight) - 1) / this->projectionMatrix._22;
	this->currentRay.z = 1.0f;    //we set 1 since the ray goes "into" the screen
	

	//World space
	
	this->rayOrigin = XMVector3TransformCoord(SimpleMath::Vector4(0,0,0,0) /*viewSpace*/, this->viewMatrix.Invert());

	this->currentRay= XMVector3TransformNormal(this->currentRay, this->viewMatrix.Invert());
	this->currentRay = XMVector3Normalize(this->currentRay);


	//printf("MouseX: %d , MouseY: %d ::::: ", MousePos.x, MousePos.y);
	//printf("X: %.2f Y: %.2f Z: %.2f \n", this->currentRay.x, this->currentRay.y, this->currentRay.z);
	return true;
}

float MousePicking::checkRayIntersectionAgainstObject(std::vector<MyVertexStruct> modelVertices, int* IndexArray,int IndexArraySize, SimpleMath::Matrix worldMatrix)
{
	for (int i = 0; i < IndexArraySize / 3; i++)
	{
		//Triangle's vertices V1, V2, V3
		//SimpleMath::Vector4 tri1V1 = { 0.0f, 0.0f, 0.0f, 0.0f };



		//Temporary 3d floats for each vertex
		//XMFLOAT3 tV1, tV2, tV3;

		//Get triangle Vertexes
		SimpleMath::Vector4 tri1V1 = { modelVertices[IndexArray[(i * 3) + 0]].x, modelVertices[IndexArray[(i * 3) + 0]].y, modelVertices[IndexArray[(i * 3) + 0]].z, 0 };
		SimpleMath::Vector4 tri1V2 = { modelVertices[IndexArray[(i * 3) + 1]].x, modelVertices[IndexArray[(i * 3) + 1]].y, modelVertices[IndexArray[(i * 3) + 1]].z, 0 };
		SimpleMath::Vector4 tri1V3 = { modelVertices[IndexArray[(i * 3) + 2]].x, modelVertices[IndexArray[(i * 3) + 2]].y, modelVertices[IndexArray[(i * 3) + 2]].z, 0 };

		//printf("tri1A: %f tri1B: %f tri1C: %f \n", tri1V1.x, tri1V2.x, tri1V3.x);

		//Transform the vertices to world space
		tri1V1 = XMVector3TransformCoord(tri1V1, worldMatrix);
		tri1V2 = XMVector3TransformCoord(tri1V2, worldMatrix);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldMatrix);
		//printf("tri1V3: %.4f tri1V1: %.4f \n", tri1V3.x, tri1V1.x );

		
		SimpleMath::Vector4 U = {0.0f, 0.0f, 0.0f, 0.0f}; //Find the normal using U, V coordinates (two edges)
		SimpleMath::Vector4 V = { 0.0f, 0.0f, 0.0f, 0.0f };
		SimpleMath::Vector4 faceNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;
		//printf("ANS: %f \n", (tri1V3 - tri1V1));
		//printf("U: %.4f V: %.4f \n", U, V);
		
		//printf("TriVertex: %f TriVertex2: %f \n", tri1V2, tri1V2);
		

		//Compute face normal by crossing U, V
		faceNormal = XMVector3Cross(U, V);
		//printf("faceNormalX: %f, FaceNormalY: %f, FaceNormalZ: %f \n", faceNormal.x, faceNormal.y, faceNormal.z);
		faceNormal = XMVector3Normalize(faceNormal);
		faceNormal = (faceNormal*-1);
		

		//Calculate a point on the triangle for the plane equation
		SimpleMath::Vector4 triPoint = tri1V1;

		//Get plane equation ("Ax + By + Cz + D = 0") Variables
		float tri1A = faceNormal.x;
		float tri1B = faceNormal.y;
		float tri1C = faceNormal.z;
		float tri1D = (-tri1A*triPoint.x - tri1B*triPoint.y - tri1C*triPoint.z);

		//Now we find where (on the ray) the ray intersects with the triangles plane
		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		SimpleMath::Vector4 pointInPlane = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		//printf("tri1A: %f\n", tri1A);
		ep1 = (rayOrigin.x * tri1A) + (rayOrigin.y * tri1B) + (rayOrigin.z * tri1C); //Camera is 0 at origin. Duck
		ep2 = (currentRay.x * tri1A) + ((currentRay.y * tri1B) + (currentRay.z * tri1C));
		//printf("ep2: %f \n", tri1A);

		
		if (ep2 != 0.0f) //Make sure there are no divide-by-zeros
			t = -(ep1 + tri1D) / (ep2);


		if (t > 0.0f)    //Make sure you don't pick objects behind the camera
		{
			//Get the point on the plane
			planeIntersectX = XMVectorGetX(rayOrigin) + XMVectorGetX(currentRay) * t;
			planeIntersectY = XMVectorGetY(rayOrigin) + XMVectorGetY(currentRay) * t;
			planeIntersectZ = XMVectorGetZ(rayOrigin) + XMVectorGetZ(currentRay) * t;

			pointInPlane = XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			//Call function to check if point is in the triangle
			if (PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
			{
				//Return the distance to the hit, so you can check all the other pickable objects in your scene
				//and choose whichever object is closest to the camera
				return t / 2.0f;
			}
		}
	}
	//return the max float value (near infinity) if an object was not picked
	return 1000000;
}

bool  MousePicking::PointInTriangle(SimpleMath::Vector4& triV1, SimpleMath::Vector4& triV2, SimpleMath::Vector4& triV3, SimpleMath::Vector4& point)
{
	//To find out if the point is inside the triangle, we will check to see if the point
	//is on the correct side of each of the triangles edges.

	XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
	if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
		if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
			cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
			if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	return false;
}

bool MousePicking::CheckBoundingBoxIntersection()
{
	//SimpleMath::Matrix;
	//BoundingBox Box;
	//XMFLOAT3 point1, point2, point3, point4;
	//point1 = { 0,0,0 };
	return false;
}



