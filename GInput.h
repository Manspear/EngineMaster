
#ifndef GINPUT_H
#define GINPUT_H
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")



class GInput
{
private:
	IDirectInput8*gDirectInput;
	IDirectInputDevice8*gKeyboard;
	IDirectInputDevice8*gMouse;

	int gScreenHeight, gScreenWidth;
	DIMOUSESTATE mouseState;

public:
	GInput();
	~GInput();
	
	int mouseX, mouseY;
	unsigned char keyState[256];
	void initialize(HINSTANCE, HWND, int, int);
	void getKeyboardState();
	void GetMouseLoc();
	void kill();

};



#endif // !GINPUT_H




