#pragma once
#ifndef MOUSEPICKING
#define MOUSEPICKING

#include <d3d11.h>

#include <directXMath.h>
#include <d3dcompiler.h> 
#include <stdio.h>

using namespace DirectX;
class MousePicking
{
//http://www.braynzarsoft.net/viewtutorial/q16390-24-picking

public:
	struct Ray
	{
		int x=0;
		int y=0;
		int z=0;
	};

	HWND wndHandle2;
	int height, width;
	MousePicking(HWND wndHandle,int screenHeight, int screenWidth);
	~MousePicking();
	void getCursorPosition();

};
#endif