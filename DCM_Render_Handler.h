#ifndef DCM_Render_Handler_h
#define DCM_Render_Handler_h

#include "GModel.h"
#include "GModelList.h"
#include "Dcm.h"
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h> 
#include <directXMath.h>
#include "GCamera.h"
#include <DirectXColors.h>

class DCM_Render_Handler
{
private:
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
	GModelList modelList;
	GModel * listOfModels;

public:

	DCM_Render_Handler();
	~DCM_Render_Handler();
	void Initialize(ID3D11Device * gDevice, ID3D11DepthStencilView * depthStencilView, ID3D11RenderTargetView * gBackbufferRTV, D3D11_VIEWPORT & vp, ID3D11GeometryShader * gGeometryShader, ID3D11PixelShader * gPixelShader, ID3D11VertexShader * gVertexShader, ID3D11InputLayout * gVertexLayout, ID3D11Buffer * gConstantBuffer, ID3D11VertexShader * gVertexShaderBS, ID3D11InputLayout * gVertexLayoutBS, ID3D11DeviceContext * gDeviceContext);
	void DCM_Render(GModel* listOfModels, DCM *dcm);
	void Render_Enviroment(GModel* listOfModels);

};
#endif