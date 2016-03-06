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

public:
#pragma region Variables

	struct Ray
	{
		int x=0;
		int y=0;
		int z=0;
	} CurrentRay;
	XMMATRIX projectionMatrix;
	XMMATRIX viewMatrix;
	GCamera* camera;
	HWND wndHandle2;
	int height, width;

#pragma region Variables

	MousePicking(HWND wndHandle, GCamera* camera, int screenHeight, int screenWidth);
	~MousePicking();

	void updateVariables();
	void getCursorPosition();
	void calculateCurrentRay();


};
#endif