#include "GInput.h"

GInput::GInput()
{
	gDirectInput = 0;
	gKeyboard = 0;
	gMouse = 0;
}
GInput::~GInput()
{
	this->kill();
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

	hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&gDirectInput, NULL);

	//init keyboard
	hr = gDirectInput->CreateDevice(GUID_SysKeyboard, &gKeyboard, NULL);
	hr = gKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = gKeyboard->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
	hr = gKeyboard->Acquire();

	//init mouse
	hr = gDirectInput->CreateDevice(GUID_SysMouse, &gMouse, NULL);
	hr = gMouse->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
	hr = gMouse->SetDataFormat(&c_dfDIMouse);
	hr = gMouse->Acquire();

}
void GInput::getKeyboardState()
{
	HRESULT hr;


	hr = gKeyboard->GetDeviceState((sizeof(unsigned char) << 8), (void*)keyState);
	if (hr != S_OK)
		hr = gKeyboard->Acquire();



}
void GInput::GetMouseLoc()
{
	HRESULT hr;
	hr = gMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&mouseState);
	if (hr != S_OK)
		hr = gMouse->Acquire();

	mouseX = mouseState.lX;
	mouseY = mouseState.lY;

	//if (mouseX <= 0)
	//	mouseX = 0;
	if (mouseX > gScreenWidth)
		mouseX = gScreenWidth;
	//if (mouseY <= 0)
	//	mouseY = 0;
	if (mouseY > gScreenHeight)
		mouseY = gScreenHeight;

}