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
	XMVECTOR pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	float PRVecX, PRVecY, PRVecZ;
	
	MousePicking::getCursorPosition(this->MousePos); printf("MouseX: %d , MouseY: %d ::::: ", MousePos.x, MousePos.y);
	
	XMFLOAT4X4 camProjection;
	XMStoreFloat4x4(&camProjection, this->projectionMatrix);
	



	//Transform 2D pick position on screen space to 3D ray in View space
	PRVecX = (((2.0f * MousePos.x) / screenWidth) - 1) / camProjection(0, 0);
	PRVecY = -(((2.0f * MousePos.x) / screenHeight) - 1) / camProjection(1, 1);
	PRVecZ = 1.0f;    //View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

	pickRayInViewSpaceDir = XMVectorSet(PRVecX, PRVecY, PRVecZ, 0.0f);

	//Uncomment this line if you want to use the center of the screen (client area)
	//to be the point that creates the picking ray (eg. first person shooter)
	//pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Transform 3D Ray from View space to 3D ray in World space
	XMMATRIX pickRayToWorldSpaceMatrix;
	XMVECTOR matInvDeter;    //We don't use this, but the xna matrix inverse function requires the first parameter to not be null

	pickRayToWorldSpaceMatrix = XMMatrixInverse(&matInvDeter, this->viewMatrix);    //Inverse of View Space matrix is World space matrix

	//pickRayInViewSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
	//pickRayInViewSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
	pickRayInViewSpaceDir = XMVector3Normalize(pickRayInViewSpaceDir);
	
	printf("X: %d Y: %d Z: %d \n", XMVectorGetX(pickRayInViewSpaceDir), XMVectorGetY(pickRayInViewSpaceDir), XMVectorGetZ(pickRayInViewSpaceDir));
	return true;
}



