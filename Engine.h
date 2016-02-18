#pragma once

#include <windows.h>

#include <d3d11.h>

#include <d3dcompiler.h> 

#include <directXMath.h>

#include "GCamera.h"

#include "GInput.h"

#include "FbxDawg.h"

#include "GModel.h"

#include "GModelList.h"

#include <vector>

#ifndef ENGINE_H
#define ENGINE_H
#define XAXIS 0
#define YAXIS 1
#define MOVESPEED 1
#define MOUSE_SENSITIVITY 100
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;

class Engine
{

private:

	

public:
	struct MyVertexStruct
	{
		float x, y, z, norX, norY, norZ, u, v;
	};
	//numberOfModels is used in Engine::Render, 
	//and is set in the Engine::InitializeModels

	Engine();
	~Engine();

	IDXGISwapChain* gSwapChain = nullptr;
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
	ID3D11RenderTargetView* gBackbufferRTV = nullptr;

	ID3D11Buffer* gVertexBuffer = nullptr;
	ID3D11Buffer* gConstantBuffer = nullptr;

	ID3D11Texture2D* gDepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	//ID3D11ShaderResourceView* gTextureView[2];

	ID3D11InputLayout* gVertexLayout = nullptr;
	ID3D11VertexShader* gVertexShader = nullptr;
	ID3D11PixelShader* gPixelShader = nullptr;
	ID3D11GeometryShader* gGeometryShader = nullptr;

	struct matrixBuffer {
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;

		XMFLOAT4 camPos;
		XMFLOAT4 camDir;
	};

	GModelList * modelListObject = nullptr;
	GCamera * camera = nullptr;
	GInput * input = nullptr;
	

	FbxDawg * fbxobj = nullptr;

	int wHEIGHT = 480;
	int wWIDTH = 640;
	byte * keys;

	//Delta Time Stuff
	double countsPerSecond;
	__int64 counterStart;
	int frameCount;
	int fps;
	__int64 frameTimeOld;
	double dt;
	void renderText(std::wstring text);
	void startTimer();
	double getTime();
	double getFrameTime();
	//end delta dime stuff

	void CreateShaders();
	void CreateTriangleData();
	//void CreateTexture(int modelCounter);
	void CreateConstantBuffer();
	void CreateDepthStencilBuffer();
	void SetViewport();
	void Render();
	void Update();
	void Clean(); //releases all resources
	void Initialize(HWND wndHandle, HINSTANCE hinstance); //Initializes functions you only call once
	void InitializeCamera();
	void InitializeModels();
	HRESULT CreateDirect3DContext(HWND wndHandle);

	/*LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND InitWindow(HINSTANCE hInstance, Engine* engine);*/

	
};

#endif