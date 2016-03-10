#include "MousePicking.h"
#include <iostream>


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
	//here be variable uppgrades
	this->projectionMatrix = this->camera->getProjMatrix();
	this->viewMatrix = this->camera->getViewMatrix();

	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0) /*if left mouse button down*/
	{
		//std::cout << projectionMatrix->;
		result = calculateCurrentRay();
		
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
	XMMATRIX projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, translateMatrix, inverseWorldMatrix;
	XMFLOAT3 direction, origin, rayOrigin, rayDirection;
	bool intersect, result;
	
	
	
	//1.viewport Space
	result = getCursorPosition(this->MousePos); //printf("(x: %d) (y: %d)\n", this->MousePos.x, this->MousePos.y);
	
	
	XMFLOAT4X4 camProjection;
	XMStoreFloat4x4(&camProjection, this->projectionMatrix);

	float ViewSpaceX = ((2 * MousePos.x) / screenWidth) / camProjection._11;
	float ViewSpaceY = ((2 * MousePos.y) / screenHeight) / camProjection._22;
	float ViewSpaceZ = 1;

	XMVECTOR vector = { ViewSpaceX ,ViewSpaceY ,ViewSpaceZ };

	XMVECTOR* pDeterminant= nullptr;
	inverseViewMatrix = XMMatrixInverse(pDeterminant, this->viewMatrix);

	XMVECTOR pickRayInWorldSpacePos = XMVector3TransformCoord(XMVectorSet(0, 0, 0, 0), inverseViewMatrix);
	XMVECTOR pickRayInWorldSpaceDir = XMVector3TransformNormal(vector, inverseViewMatrix);

	pickRayInWorldSpaceDir = XMVector3Normalize(pickRayInWorldSpaceDir);

	//Microsoft.DirectX.Vector3 ray = 
	
	//3.homogeneous clip space
	//4.eye space
	//5.world space
	
	printf("RayDirection; x: %d y: %d, z: %d \n", XMVectorGetX(pickRayInWorldSpacePos), XMVectorGetY(pickRayInWorldSpaceDir), XMVectorGetZ(pickRayInWorldSpacePos));


	return true;
}



