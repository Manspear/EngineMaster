#include "MousePicking.h"



MousePicking::MousePicking(HWND wndHandle, GCamera* camera, int screenHeight, int screenWidth)
{
	this->camera = camera;
	this->wndHandle2 = wndHandle;
	this->projectionMatrix = camera->getProjMatrix();
	this->viewMatrix = camera->getViewMatrix();
	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;

	this->previousLeftMouseButtonDown = false;
}

MousePicking::~MousePicking()
{
	//Pass
}

void MousePicking::updateClass(std::vector<GModel*> ObjectsforIntersectionTest, bool& BoolAffectingClearColor) //updates variables & checks ray
{
	if (isLeftMouseButtonClicked())
	{
		if (ObjectsforIntersectionTest.size() > 0)
		{
			this->projectionMatrix = this->camera->getProjMatrix();
			this->viewMatrix = this->camera->getViewMatrix();
			this->result = this->calculateCurrentRay();

			//this->CheckBoundingBoxIntersection();
			for (int i = 1; i < ObjectsforIntersectionTest.size(); i++)
			{
				if(ObjectsforIntersectionTest[i]->isAnimated())
				{ 
					if ("%f\n", this->checkRayIntersectionAgainstObject(ObjectsforIntersectionTest[i]->animModelVertices, ObjectsforIntersectionTest[i]->IndexArray, ObjectsforIntersectionTest[i]->sizeOfIndexArray, XMMatrixTranspose(*ObjectsforIntersectionTest[i]->objectWorldMatrix))
						< 10000)
					{
						//printf("hit\n");
						BoolAffectingClearColor = true;
						return;
					}
					BoolAffectingClearColor = false;
				}
				else
				{
					if ("%f\n", this->checkRayIntersectionAgainstObject(ObjectsforIntersectionTest[i]->modelVertices, ObjectsforIntersectionTest[i]->IndexArray, ObjectsforIntersectionTest[i]->sizeOfIndexArray, XMMatrixTranspose(*ObjectsforIntersectionTest[i]->objectWorldMatrix))
						< 10000)
					{
						//printf("hit\n");
						BoolAffectingClearColor = true;
						return;
					}
					BoolAffectingClearColor = false;
				}
			}
		}
		else
		{
			;//printf("No objects in scene\n");
		}
	}

}

bool MousePicking::isLeftMouseButtonClicked()
{

	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0 && previousLeftMouseButtonDown == false) /*if left mouse button down && was up*/
	{
		previousLeftMouseButtonDown = true;
		return true;
	}

	else if ((GetKeyState(VK_LBUTTON) & 0x100) == 0) //if not down
	{
		previousLeftMouseButtonDown = false;
	}

	return false;

}

bool MousePicking::getCursorPosition(POINT& MousePosSavedHere)
{

	//Gets the cursor pos in screen coordinates
	this->result = GetCursorPos(&MousePosSavedHere);

	if (this->result) /*The function call succeeded and curPos has valid data.*/
	{
		//Converts the cursor pos from "big" screen coordinates to client coordinates.
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
	MousePicking::getCursorPosition(this->MousePos); //ViewportSpace
	//Normalizes the mouseposition, and makes it become a value between -1 and 1
	this->currentRay.x = (((2.0f * MousePos.x) / screenWidth) - 1); //får oss till NDC space, som med ett w-värde är
	this->currentRay.y = -(((2.0f * MousePos.y) / screenHeight) - 1); //detsamma som homogenous clip space.

	this->currentRay.w = 0;

	//We could get from clip space to view space by multiplying with the inverse projection matrix, 
	//we only need to un-project the x, y part though (since the z part is just a vector pointing 
	//straight into the screen)
	//So we are effective by only dividing the x and y parts of the ray. 
	this->currentRay.x /= this->projectionMatrix._11; //får oss till ViewSpace
	this->currentRay.y /= this->projectionMatrix._22;

	

	this->currentRay.x = (((2.0f * MousePos.x) / screenWidth) - 1) / this->projectionMatrix._11; //får oss till ViewSpace
	this->currentRay.y = -(((2.0f * MousePos.y) / screenHeight) - 1) / this->projectionMatrix._22;
	this->currentRay.z = 1.0f;    //we set 1 since the ray goes "into" the screen


	//Gets the camera's position in the world by re-interpreting it's origin to that of the world. The ray's origin
	this->rayOrigin = XMVector3TransformCoord(SimpleMath::Vector4(0, 0, 0, 0), this->viewMatrix.Invert());
	//gets ray into worldspace
	this->currentRay = XMVector3TransformNormal(this->currentRay, this->viewMatrix.Invert()); 
	//Normalize the vector since we set a manual z value.
	this->currentRay = XMVector3Normalize(this->currentRay);

	return true;
}

float MousePicking::checkRayIntersectionAgainstObject(std::vector<MyVertexStruct> modelVertices, int* IndexArray, int IndexArraySize, SimpleMath::Matrix worldMatrix)
{

	//currentRay=XMVector3TransformCoord(currentRay, worldMatrix.Invert());
	//rayOrigin=XMVector3TransformCoord(rayOrigin, worldMatrix.Invert());

	for (int i = 0; i < IndexArraySize / 3; i++)
	{
		//Get triangle Vertexes
		SimpleMath::Vector4 tri1V1 = { modelVertices[IndexArray[(i * 3) + 0]].x, modelVertices[IndexArray[(i * 3) + 0]].y, modelVertices[IndexArray[(i * 3) + 0]].z, 1 };
		SimpleMath::Vector4 tri1V2 = { modelVertices[IndexArray[(i * 3) + 1]].x, modelVertices[IndexArray[(i * 3) + 1]].y, modelVertices[IndexArray[(i * 3) + 1]].z, 1 };
		SimpleMath::Vector4 tri1V3 = { modelVertices[IndexArray[(i * 3) + 2]].x, modelVertices[IndexArray[(i * 3) + 2]].y, modelVertices[IndexArray[(i * 3) + 2]].z, 1 };

		//Transform the vertices to world space
		tri1V1 = XMVector3TransformCoord(tri1V1, worldMatrix);
		tri1V2 = XMVector3TransformCoord(tri1V2, worldMatrix);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldMatrix);



		SimpleMath::Vector4 U = { 0.0f, 0.0f, 0.0f, 0.0f }; //Find the normal using U, V coordinates (two edges)
		SimpleMath::Vector4 V = { 0.0f, 0.0f, 0.0f, 0.0f };
		SimpleMath::Vector4 faceNormal = { 0.0f, 0.0f, 0.0f, 0.0f };

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;
		faceNormal = XMVector3Cross(U, V); //Compute face normal by crossing U, V
		faceNormal = XMVector3Normalize(faceNormal);

		SimpleMath::Vector4 triPoint = tri1V1; //Calculate a point on the triangle for the plane equation


		float tri1A = faceNormal.x; //Get plane equation ("Ax + By + Cz + D = 0") Variables
		float tri1B = faceNormal.y;
		float tri1C = faceNormal.z;
		float tri1D = (-tri1A*triPoint.x - tri1B*triPoint.y - tri1C*triPoint.z);


		float ep1, ep2, t = 0.0f; //Now we find where (on the ray) the ray intersects with the triangles plane
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;


		SimpleMath::Vector4 pointInPlane = { 0.0f, 0.0f, 0.0f, 0.0f };

		ep1 = (rayOrigin.x * tri1A) + (rayOrigin.y * tri1B) + (rayOrigin.z * tri1C); //Camera is 0 at origin. Duck
		ep2 = (currentRay.x * tri1A) + ((currentRay.y * tri1B) + (currentRay.z * tri1C));



		if (ep2 != 0.0f) //Make sure there are no divide-by-zeros
			t = -(ep1 + tri1D) / (ep2);


		if (t > 0.0f)    //Make sure you don't pick objects behind the camera
		{
			//Get the point on the plane
			planeIntersectX = rayOrigin.x + currentRay.x * t;
			planeIntersectY = rayOrigin.y + currentRay.y * t;
			planeIntersectZ = rayOrigin.z + currentRay.z * t;

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

float MousePicking::checkRayIntersectionAgainstObject(std::vector<AnimVertexStruct> modelVertices, int* IndexArray, int IndexArraySize, SimpleMath::Matrix worldMatrix)
{
	for (int i = 0; i < IndexArraySize / 3; i++)
	{
		//Get triangle Vertexes
		SimpleMath::Vector4 tri1V1 = { modelVertices[IndexArray[(i * 3) + 0]].x, modelVertices[IndexArray[(i * 3) + 0]].y, modelVertices[IndexArray[(i * 3) + 0]].z, 1 };
		SimpleMath::Vector4 tri1V2 = { modelVertices[IndexArray[(i * 3) + 1]].x, modelVertices[IndexArray[(i * 3) + 1]].y, modelVertices[IndexArray[(i * 3) + 1]].z, 1 };
		SimpleMath::Vector4 tri1V3 = { modelVertices[IndexArray[(i * 3) + 2]].x, modelVertices[IndexArray[(i * 3) + 2]].y, modelVertices[IndexArray[(i * 3) + 2]].z, 1 };

		//Transform the vertices to world space
		tri1V1 = XMVector3TransformCoord(tri1V1, worldMatrix);//get triangle into world. having ray in object space would be more optimized
		tri1V2 = XMVector3TransformCoord(tri1V2, worldMatrix);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldMatrix);
		
		SimpleMath::Vector4 U = { 0.0f, 0.0f, 0.0f, 0.0f }; //Find the normal using U, V coordinates (two edges)
		SimpleMath::Vector4 V = { 0.0f, 0.0f, 0.0f, 0.0f };
		SimpleMath::Vector4 faceNormal = { 0.0f, 0.0f, 0.0f, 0.0f }; //find the normal of the triangle

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;
		faceNormal = XMVector3Cross(U, V); //Compute face normal by crossing U, V
		faceNormal = XMVector3Normalize(faceNormal);

		SimpleMath::Vector4 triPoint = tri1V1; //Calculate a point on the triangle for the plane equation

		float tri1A = faceNormal.x; //Get plane equation ("Ax + By + Cz + D = 0") Variables
		float tri1B = faceNormal.y;
		float tri1C = faceNormal.z;
		//Get D by: D = -Ax - By - Cz
		float tri1D = (-tri1A*triPoint.x - tri1B*triPoint.y - tri1C*triPoint.z);

		float ep1, ep2, t = 0.0f; //Now we find where (on the ray) the ray intersects with the triangles plane
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;

		SimpleMath::Vector4 pointInPlane = { 0.0f, 0.0f, 0.0f, 0.0f };

		/*
		Ray - Plane intersection

		Ray: point = origin + t * V
		Plane: dot(P, normal) + d = 0

		The equation for a plane: APx + BPy + CPz + D = 0
		Can be rewritten as: dot(P, normal) + D = 0
		The equation of a ray: P = Po + t*V

		You substitute the P in the plane's equation with the ray equation:
		dot((Po + tV), normal) + D = 0
		Break out t:
		t = -(dot(Po, normal) + D) / dot(V, normal)

		And there we've got our t-value. Easy as pie.
		*/
		

		//Here we calculate the scalar product of the ray origin against the  triangle normal. 
		//And also the scalar product of the ray direction, against the triangle normal.
		ep1 = (rayOrigin.x * tri1A) + (rayOrigin.y * tri1B) + (rayOrigin.z * tri1C); //Camera is 0 at origin. Duck
		ep2 = (currentRay.x * tri1A) + ((currentRay.y * tri1B) + (currentRay.z * tri1C));

		//In other words: if the triangle normal and ray direction aren't parallell
		if (ep2 != 0.0f) //Make sure there are no divide-by-zeros
			//angle between origin and normal + offset from origin / angle between 
			t = -(ep1 + tri1D) / (ep2);

		if (t > 0.0f)    //Make sure you don't pick objects behind the camera
		{
			//Get the point on the plane
			planeIntersectX = rayOrigin.x + currentRay.x * t;
			planeIntersectY = rayOrigin.y + currentRay.y * t;
			planeIntersectZ = rayOrigin.z + currentRay.z * t;

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

	SimpleMath::Vector3 cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	SimpleMath::Vector3 cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
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



