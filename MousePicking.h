#pragma once
#ifndef MOUSEPICKING
#define MOUSEPICKING

#include <d3d11.h>
#include <directXMath.h>
#include <d3dcompiler.h> 
#include <stdio.h>
#include "GCamera.h"

using namespace DirectX;


class MousePicking
{
//http://www.braynzarsoft.net/viewtutorial/q16390-24-picking

//Variables
private:
	POINT MousePos;
	int screenHeight, screenWidth;
	
	bool result;
	XMMATRIX projectionMatrix, viewMatrix;
	GCamera* camera;
	HWND wndHandle2;
	

public:

	struct Ray
	{
		int x=0;
		int y=0;
		int z=0;
	} CurrentRay;

	
//end of Variables

	MousePicking(HWND wndHandle, GCamera* camera, int screenHeight, int screenWidth);
	~MousePicking();

	void updateClass(); 
	bool getCursorPosition(POINT& MousePosSavedHere);
	bool calculateCurrentRay();


};
#endif