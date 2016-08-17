#ifndef DCM_h
#define DCM_h

#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h> 
#include <directXMath.h>
#include "GCamera.h"
#include <DirectXColors.h>
#include "GModel.h"
#include "GModelList.h"

class DCM
{
private:
	GCamera DCM_CubeMapCamera[6];
	D3D11_VIEWPORT DCM_CubeMapViewport;
	ID3D11ShaderResourceView* DCM_ShaderResourceView;
	ID3D11RenderTargetView* DCM_RenderTargetView[6];
	ID3D11DepthStencilView* DCM_DepthStencilView;

	ID3D11DeviceContext* gDeviceContext;
	ID3D11Device * gDevice;
	ID3D11DepthStencilView * depthStencilView;
	ID3D11RenderTargetView * gBackbufferRTV;
	D3D11_VIEWPORT vp;

	ID3D11GeometryShader* gGeometryShader;
	ID3D11PixelShader* gPixelShader;
	ID3D11VertexShader* gVertexShader;
	ID3D11InputLayout* gVertexLayout;
	ID3D11Buffer* gConstantBuffer;
	ID3D11VertexShader* gVertexShaderBS;
	ID3D11InputLayout* gVertexLayoutBS;


public:

	void Initialize(ID3D11Device * gDevice, ID3D11DepthStencilView * depthStencilView, ID3D11RenderTargetView * gBackbufferRTV, D3D11_VIEWPORT &vp,
		ID3D11GeometryShader* gGeometryShader, ID3D11PixelShader* gPixelShader, ID3D11VertexShader* gVertexShader, ID3D11InputLayout* gVertexLayout,
		ID3D11Buffer* gConstantBuffer, ID3D11VertexShader* gVertexShaderBS, ID3D11InputLayout* gVertexLayoutBS, ID3D11DeviceContext* gDeviceContext);

	DCM();
	~DCM();
	void Dynamic_Cube_Map(ID3D11Device *gDevice);
	void BuildCubeFaceCamera(float x, float y, float z, float w);
	void DCM_Render_Main(GModel* listOfModels, GModelList *modelListObject);
	void Render_Enviroment(GModel* listOfModels, GModelList *modelListObject);
	D3D11_VIEWPORT getDCM_CubeMapViewport();
	ID3D11ShaderResourceView* getShaderResourceView();
	ID3D11RenderTargetView* getDCM_RenderTargetView(int i);
	ID3D11DepthStencilView* getDCM_DepthStencilView();
	GCamera getDCM_CubeMapCamera(int i);// ifall detta kommer behövas i engine, om inte, ta bort
};
#endif