#pragma once
#ifndef MOUSEPICKING
#define MOUSEPICKING

#include <d3d11.h>

#include <directXMath.h>
#include <d3dcompiler.h> 

using namespace DirectX;
class MousePicking
{
public:
	struct ScreenPos
	{
		int x=0;
		int y=0;
	};

	struct Ray
	{
		int x=0;
		int y=0;
		int z=0;
	};


	MousePicking();
	~MousePicking();

};
#endif