#include "MousePicking.h"



MousePicking::MousePicking(HWND wndHandle, GCamera* camera, int screenHeight, int screenWidth)
{
	this->camera = camera;
	this->wndHandle2 = wndHandle;
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
	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0) /*if left mouse button down*/
	{
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
	
	result = getCursorPosition(this->MousePos); //1. get the mouse cordinates
												//printf("(x: %d) (y: %d)\n", this->MousePos.x, this->MousePos.y);
	
	
	printf("RayDirection; x: %d y: %d, z: %d \n", this->CurrentRay.x, this->CurrentRay.x, this->CurrentRay.z);
	return true;
}



