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
	GCamera mCubeMapCamera[6];
	GCamera camera;

	D3D11_VIEWPORT mCubeMapViewport;
	ID3D11ShaderResourceView* mDynamicCubeMapSRV;
	ID3D11RenderTargetView* mDynamicCubeMapRTV[6];
	ID3D11DepthStencilView* mDynamicCubeMapDSV;

	D3D11_VIEWPORT mScreenViewport;//mains
	ID3D11RenderTargetView* mRenderTargetView;//mains
	ID3D11DepthStencilView* mDepthStencilView;//mains
	IDXGISwapChain* mSwapChain;//mains
	ID3D11DeviceContext *gDeviceContext;//mains


public:
	DCM();
	DCM(GCamera &camera);
	~DCM();
	void Dynamic_Cube_Map(ID3D11Device *gDevice);
	void BuildCubeFaceCamera(float x, float y, float z, float w);
	void DrawScene();
	void DrawScene2(const GCamera& mCubeMapCamera, bool drawCenterSphere);
};
#endif




