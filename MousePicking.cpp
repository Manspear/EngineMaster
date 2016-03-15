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
	//pickRayInViewSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
	this->currentRay= XMVector3TransformNormal(this->currentRay, this->viewMatrix.Invert());
	this->currentRay = XMVector3Normalize(this->currentRay);


	printf("MouseX: %d , MouseY: %d ::::: ", MousePos.x, MousePos.y);
	printf("X: %.2f Y: %.2f Z: %.2f \n", currentRay.x, currentRay.y, currentRay.z);
	return true;
}

bool MousePicking::CheckBoundingBoxIntersection()
{
	SimpleMath::Matrix;
	BoundingBox Box;

	return false;
}



