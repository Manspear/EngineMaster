#include "MousePicking.h"



MousePicking::MousePicking(HWND wndHandle, GCamera* camera, int screenHeight, int screenWidth)
{
	this->camera = camera;
	this->wndHandle2 = wndHandle;
	this->height = screenHeight;
	this->width = screenWidth;
}

MousePicking::~MousePicking()
{
	 //Pass
}

void MousePicking::updateVariables()
{

}

void MousePicking::getCursorPosition()
{
	
	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0) /*if left mouse button down*/{
		POINT curMousePos; 
		BOOL result = GetCursorPos(&curMousePos);

		if (result) /*The function call succeeded and curPos has valid data.*/{
			
			result=ScreenToClient(this->wndHandle2, &curMousePos);

			if (curMousePos.x > width)
				curMousePos.x = width;

			else if (curMousePos.x < 0)
				curMousePos.x = 0;

			if (curMousePos.y > height)
				curMousePos.y = height;

			else if (curMousePos.y < 0)
				curMousePos.y = 0;

			printf("(x: %d) (y: %d)\n", curMousePos.x, curMousePos.y);
		}
		else
		{
			printf("Failed to get mouse position data\n");

		}

	}//End of if(left mouse button down)
}

void MousePicking::calculateCurrentRay()
{

}



