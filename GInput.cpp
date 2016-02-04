#include "GInput.h"

GInput::GInput()
{
	gDirectInput = 0;
	gKeyboard = 0;
	gMouse = 0;
}
GInput::~GInput()
{

}

void GInput::kill()
{
	if (gKeyboard)
	{
		gKeyboard->Unacquire();
		gKeyboard->Release();
		gKeyboard = 0;
	}

	if (gDirectInput)
	{
		gDirectInput->Release();
		gDirectInput = 0;
	}
}

void GInput::initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT hr;

	gScreenHeight = screenHeight;
	gScreenWidth = screenWidth;

	mouseX = 0;
	mouseY = 0;

	hr = gDirectInput->CreateDevice(GUID_SysKeyboard, &gKeyboard, NULL);
	hr = gKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = gKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	hr = gKeyboard->Acquire();

}
bool GInput::frame()
{
	return true;
}
void GInput::GetMouseLoc(int&, int&)
{

}