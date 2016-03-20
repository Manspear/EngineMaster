#pragma once
#ifndef MOUSEPICKING
#define MOUSEPICKING

#include <d3d11.h>
#include <d3dcompiler.h> 
#include <directXMath.h>
#include "SimpleMath.h"
#include "DirectXCollision.h"
#include <vector>
#include <stdio.h>
#include <iostream>

#include "GCamera.h"
#include "FbxDawg.h"
#include "GModel.h"


using namespace DirectX;


class MousePicking
{
//http://www.braynzarsoft.net/viewtutorial/q16390-24-picking

//Variables
private:
	bool previousLeftMouseButtonDown;
	int screenHeight, screenWidth;
	bool result;
	SimpleMath::Matrix projectionMatrix, viewMatrix;
	GCamera* camera;
	HWND wndHandle2;
	POINT MousePos;
	bool CheckBoundingBoxIntersection();
	
public:

	
	SimpleMath::Vector4 currentRay;
	SimpleMath::Vector4 rayOrigin;
	
	
	
//end of Variables
	
	
	MousePicking(HWND wndHandle, GCamera* camera, int screenHeight, int screenWidth);
	~MousePicking();
	void MousePicking::updateClass(std::vector<GModel*> ObjectsforIntersectionTest, bool& BoolAffectingClearColor);
	bool isLeftMouseButtonClicked();
	bool getCursorPosition(POINT& MousePosSavedHere);
	bool calculateCurrentRay();
	float checkRayIntersectionAgainstObject(std::vector<MyVertexStruct> modelVertices, int* IndexArray, int IndexArraySize, SimpleMath::Matrix worldMatrix); //std::vector<MyVertexStruct> modelVertices,
	bool PointInTriangle(SimpleMath::Vector4& triV1, SimpleMath::Vector4& triV2, SimpleMath::Vector4& triV3, SimpleMath::Vector4& point);
	
};
#endif