#pragma once

#include <windows.h>

#include <d3d11.h>

#include <d3dcompiler.h> 

#include <directXMath.h>

using namespace DirectX;
class ShadowMap
{
public:
	/*Remember that XMMATRIX cannot be sent between functions of classes. Use XMFLOAT4X4 instead*/
	struct MatrixBufferType
	{
		
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct LightBufferType
	{
		XMMATRIX ambientColor;
		XMMATRIX diffuseColor;
	};

	struct LightBufferType2
	{
		XMMATRIX lightPosition;
		float padding;
	};

	void createDepthBuffer();
private:
};