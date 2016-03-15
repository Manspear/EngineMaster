#pragma once
#ifndef MOUSEPICKING
#define MOUSEPICKING

#include <d3d11.h>
#include <d3dcompiler.h> 
#include <directXMath.h>
#include "SimpleMath.h"
#include "DirectXCollision.h"

#include <stdio.h>
#include <iostream>

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
	SimpleMath::Matrix projectionMatrix, viewMatrix;
	GCamera* camera;
	HWND wndHandle2;
	

public:

	
	SimpleMath::Vector3 currentRay;
	
	
	
//end of Variables

	MousePicking(HWND wndHandle, GCamera* camera, int screenHeight, int screenWidth);
	~MousePicking();

	void updateClass(); 
	bool getCursorPosition(POINT& MousePosSavedHere);
	bool calculateCurrentRay();
	bool CheckBoundingBoxIntersection(); //std::vector<MyVertexStruct> modelVertices,


};
#endif