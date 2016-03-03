#include "MousePicking.h"

MousePicking::MousePicking(HWND wndHandle)
{
	this->wndHandle = &wndHandle;
	this->getCursorPosition();
}

MousePicking::~MousePicking()
{

}

void MousePicking::getCursorPosition()
{
	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
	{
		POINT curMousePos;
		BOOL result = GetCursorPos(&curMousePos);
		if (result)
		{
			//The function call succeeded and curPos has valid data.
			ScreenToClient(*this->wndHandle, &curMousePos);
			printf("(x: %d) (y: %d)\n", curMousePos.x, curMousePos.y);

		}
		else
		{
			printf("Failed to get mouse position data\n");

		}

	}
}



