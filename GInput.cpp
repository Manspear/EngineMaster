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
	hr = gDirectInput->CreateDevice(GUID_SysKeyboard, &gKeyboard, NULL);
	hr = gKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = gKeyboard->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
	hr = gKeyboard->Acquire();

}
void GInput::getKeyboardState()
{
	HRESULT hr;

	
	hr = gKeyboard->GetDeviceState((sizeof(unsigned char)<<8), (void*)keyState);
	if (hr != S_OK)
	{
		hr = gKeyboard->Acquire();
	}


}
void GInput::GetMouseLoc(int&, int&)
{

}