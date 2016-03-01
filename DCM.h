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
	GCamera *camera;

	ID3D11RenderTargetView* mRenderTargetView;//mains ller inte?
	ID3D11DepthStencilView* mDepthStencilView;//mains
	IDXGISwapChain* mSwapChain;//mains
	ID3D11DeviceContext *gDeviceContext;//mains





	D3D11_VIEWPORT mCubeMapViewport;
	D3D11_VIEWPORT mScreenViewport;// finns ingen viewport som jag kan skicka med till dcm classen
	ID3D11ShaderResourceView* mDynamicCubeMapSRV;
	ID3D11RenderTargetView* mDynamicCubeMapRTV[6];
	ID3D11DepthStencilView* mDynamicCubeMapDSV;




public:
	DCM();
	~DCM();
	void Dynamic_Cube_Map(ID3D11Device *gDevice);
	void BuildCubeFaceCamera(float x, float y, float z, float w);
	void DrawScene();
	void DrawScene2(const GCamera& mCubeMapCamera, bool drawCenterSphere);

	ID3D11ShaderResourceView *GetSubResourceView();
	
};
#endif




