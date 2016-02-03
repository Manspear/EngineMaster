//#pragma once

#include <windows.h>

#include <d3d11.h>

#include <d3dcompiler.h> 

#include <directXMath.h>

#include "GCamera.h"

#ifndef ENGINE_H
#define ENGINE_H

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;

class Engine
{

private:
	


public:
	Engine();
	struct matrixBuffer {
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projectionMatrix;
	};

	IDXGISwapChain* gSwapChain = nullptr;
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
	ID3D11RenderTargetView* gBackbufferRTV = nullptr;

	ID3D11Buffer* gVertexBuffer = nullptr;
	ID3D11Buffer* gConstantBuffer = nullptr; 
											
	ID3D11Texture2D* gDepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11ShaderResourceView* gTextureView = nullptr;

	ID3D11InputLayout* gVertexLayout = nullptr;
	ID3D11VertexShader* gVertexShader = nullptr;
	ID3D11PixelShader* gPixelShader = nullptr;
	ID3D11GeometryShader* gGeometryShader = nullptr;

	void CreateShaders();
	void CreateTriangleData();
	void CreateTexture();
	void CreateConstantBuffer();
	void CreateDepthStencilBuffer();
	void SetViewport();
	void Render();
	void Update();
	void Clean(); //releases all resources
	void Initialize(HWND wndHandle); //Initializes functions you only call once
	HRESULT CreateDirect3DContext(HWND wndHandle);

	/*LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND InitWindow(HINSTANCE hInstance, Engine* engine);*/

	
};

#endif