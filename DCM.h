#ifndef DCM_h
#define DCM_h

#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h> 
#include <directXMath.h>
#include "GCamera.h"
#include <DirectXColors.h>

class DCM
{

private:
	GCamera DCM_CubeMapCamera[6];
	GCamera *camera;

	ID3D11RenderTargetView* mRenderTargetView;//mains ller inte?
	ID3D11DepthStencilView* mDepthStencilView;//mains
	IDXGISwapChain* mSwapChain;//mains
	ID3D11DeviceContext *gDeviceContext;//mains

	D3D11_VIEWPORT DCM_CubeMapViewport;
	D3D11_VIEWPORT mScreenViewport;// finns ingen viewport som jag kan skicka med till dcm classen
	ID3D11ShaderResourceView* DCM_ShaderResourceView;
	ID3D11RenderTargetView* DCM_RenderTargetView[6];
	ID3D11DepthStencilView* DCM_DepthStencilView;


	

public:
	DCM();
	~DCM();
	void Dynamic_Cube_Map(ID3D11Device *gDevice);
	void BuildCubeFaceCamera(float x, float y, float z, float w);
	void DrawScene();
	D3D11_VIEWPORT getDCM_CubeMapViewport();
	ID3D11ShaderResourceView* getSubResourceView();
	ID3D11RenderTargetView* getDCM_RenderTargetView(int i);
	ID3D11DepthStencilView* getDCM_DepthStencilView();
	GCamera getDCM_CubeMapCamera(int i);// ifall detta kommer behövas i engine, om inte, ta bort
};
#endif




