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
	GCamera *mCubeMapCamera[6];
	D3D11_VIEWPORT mScreenViewport;
	D3D11_VIEWPORT mCubeMapViewport;
	
	ID3D11RenderTargetView* mDynamicCubeMapRTV[6];
	ID3D11DepthStencilView* mDynamicCubeMapDSV;
	ID3D11DeviceContext *gDeviceContext;


public:
	DCM();
	~DCM();
	void Dynamic_Cube_Map(ID3D11Device *gDevice);
	void BuildCubeFaceCamera(float x, float y, float z, float w);
	void DrawScene(GCamera *mCubeMapCamera, bool answer);
};
#endif




