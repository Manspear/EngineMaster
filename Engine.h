#pragma once

#include <windows.h>

#include <d3d11.h>

#include <d3dcompiler.h> 

#include <directXMath.h>

#include "GCamera.h"

#include "MousePicking.h"

#include "GInput.h"

#include "FbxDawg.h"

#include "GModel.h"

#include "GModelList.h"

#include "GFrustum.h"

#include "GQuadTree.h"

#include <vector>

#include "ParticleSystem.h"

#include "DeltaTime.h"

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
	GModel* listOfModels;
	bool mousePickEffectOnClearColor = false;


public:
	struct MyVertexStruct
	{
		float x, y, z, norX, norY, norZ, u, v;
	};
	//numberOfModels is used in Engine::Render, 
	//and is set in the Engine::InitializeModels

	Engine();
	~Engine();

	IDXGISwapChain* gSwapChain = nullptr; //x
	ID3D11Device* gDevice = nullptr; //x
	ID3D11DeviceContext* gDeviceContext = nullptr; //x
	ID3D11RenderTargetView* gBackbufferRTV = nullptr; //x

	ID3D11Buffer* gConstantBuffer = nullptr; //x
	ID3D11Buffer* gShadowBuffer = nullptr; //x

	ID3D11Texture2D* gDepthStencilBuffer = nullptr; //x
	ID3D11DepthStencilView* depthStencilView = nullptr; //x
	//ID3D11ShaderResourceView* gTextureView[2];

	ID3D11SamplerState* gPSTextureSampler = nullptr; //x

	ID3D11InputLayout* gVertexLayout = nullptr; //x
	ID3D11InputLayout* gVertexLayoutBS = nullptr; //x
	ID3D11InputLayout* gVertexLayoutSkeletal = nullptr; //x
	ID3D11VertexShader* gVertexShader = nullptr; //x
	ID3D11VertexShader* gVertexShaderSkeletal = nullptr; //x
	ID3D11VertexShader* gVertexShaderBS = nullptr; //x
	ID3D11PixelShader* gPixelShader = nullptr; //x
	ID3D11GeometryShader* gGeometryShader = nullptr;//x


	struct matrixBuffer 
	{
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
		XMFLOAT4 camPos;
		XMFLOAT4 camDir;
	};

	struct  ShadowMatrixBuffer
	{
		//XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
		XMMATRIX lightViewMatrix;
		XMMATRIX lightProjectionMatrix;
	};


	GModelList modelListObject; //This will be used in GQuadTreeBoundingBox::splitBox()
	GCamera * camera = nullptr;
	MousePicking* MousePickingObject = nullptr;
	GInput * input = nullptr;
	GFrustum * cullingFrustum = nullptr;
	GQuadTree * quadTreeRoot = nullptr;
	FbxDawg * fbxobj = nullptr;
	ParticleSystem * particleSys;

	int wHEIGHT = 480;
	int wWIDTH = 640;//was 640
	byte * keys;

	//Delta Time Stuff
	double countsPerSecond;
	__int64 counterStart;
	int frameCount;
	int fps;
	__int64 frameTimeOld;
	//double dt;
	void renderText(std::wstring text);
	void startTimer();
	double getTime();
	double getFrameTime();
	//end delta dime stuff

	void CreateShaders();
	//void CreateTexture(int modelCounter);
	void CreateConstantBuffer();
	void CreateShadowBuffer();
	void CreateDepthStencilBuffer();
	void SetViewport();
	void Render();
	void Update();
	void Clean(); //releases all resources
				  //void Initialize(HWND wndHandle, HINSTANCE hinstance); 
	void Initialize(HWND wndHandle, HINSTANCE hinstance); //Initializes functions you only call once
	void InitializeFrustum();
	void InitializeCamera();
	void initializeMousePicking(HWND wndHandle);
	void InitializeModels();
	void InitializeQuadTree();
	void initializeParticles();
	HRESULT CreateDirect3DContext(HWND wndHandle);

	/*LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND InitWindow(HINSTANCE hInstance, Engine* engine);*/


};

#endif