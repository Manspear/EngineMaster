#pragma once

#include <windows.h>

#include <d3d11.h>

#include <d3dcompiler.h> 

#include <directXMath.h>

#include "GModelList.h"

using namespace DirectX;
#define LIGHT_POS (4, 3, -3)
#define LIGHT_DIR (0, 1, 1)
/*
MISSION OF THIS CLASS:

Have a render function, that returns/creates a depth buffer saved in a texture
to be used by other functions.

What this class holds:
Information about the light's WVP matrix

*/
class ShadowMap
{
public:
	enum ShaderType
	{
		vanilla,
		bs,
		skeletal
	};

	/*Same dimensions as the viewport renderer*/
	void initializeShadowMap(ID3D11DeviceContext* deviceContext, ID3D11Device* device);
	ShadowMap();
	/*Remember that XMMATRIX cannot be sent between functions of classes. Use XMFLOAT4X4 instead*/

	bool initialize(ID3D11Device* device, HWND handle);
	void uninitialize();

	/*
	This is the first pass, the "shadow pass" of the render, 
	where the depth of the z-buffer is stored in the private
	pDepthStencilBuffer-variable. This depth buffer should be
	incorporated into shadowmap-versions of our existing shaders.

	Put this in a loop looping through all objects in the scene.
	After the loop is done, set the rendertarget to the old one.
	*/
	bool RenderShadowed(
		ID3D11DeviceContext* deviceContext, 
		GModelList modelList,
		ID3D11Buffer* vertexBuffer, //vertexBuffer: The vertices to be rendered shadowed
		ID3D11RenderTargetView* RTV, //RTV: The non-light render target view... i.e the "normal" one
		ShaderType shadTp, //shadTp: vanilla, bs, skeletal (an enum)
		UINT32 vertexSize //vertexSize: Send in the byte-size per-vertex.
		);  

	ID3D11Texture2D* getDepthTexture();

private:
	bool InitializeShader(ID3D11Device* device);
	/*bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMMATRIX lightPosition,
		XMMATRIX ambientColor, XMMATRIX diffuseColor);*/
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
	void createCbuffers(ID3D11Device* device);
	void initializeMatrix();
	/*
	This is the depth-buffer saved from the light-pass
	In the second pass, send this as a shader resource
	*/
	ID3D11Texture2D* pShadowMap = nullptr;
	//This is the depthstencilview used for something...
	/*
	You never want to get to the pixelshader-stage during the first pass. Stay at the vertexShader.
	You use your pDepthStencilview (or maybe pDepthStencilView) as a rendertarget.

	You use a viewport(s) to shit on god. You use a viewport(s) to
	render from the camera's perspective. I.E: You have to temporarily change your viewport 
	before rendering shadow mapping.

	//From the tutorial:
	"points lights are problematic for shadow mapping so for now just assume light sources are either directional lights or spotlights"
	So make a directional light.

	"To render the scene from the viewpoint of the light, we need to create a view-matrix for the light."
	"The second matrix needed is a projection matrix that represents the view volume, or the "light volume"
	emitted by the light. The projection matrix is orthographic for directional lights" 

	during the fist render pass we have void pixel shader, and a null render target.
	*/

	//This seems to "fill" the pShadowMap texture, that then in turn "fills" the shader resource
	ID3D11DepthStencilView* pShadowDSV = nullptr;

	//these shaders makes the lighting texture out of the shadowmap depth buffer
	ID3D11VertexShader* vertexShaderShadow = nullptr;
	ID3D11PixelShader* pixelShaderShadow = nullptr;


	//Holds the "vanilla" geometry, no skeletal or BS stuff
	ID3D11InputLayout* VertexlayoutShadow = nullptr; 

	//Set this as resource l8er
	ID3D11ShaderResourceView* pShadowSRV;
	//samplers
	ID3D11SamplerState* sampleStateWrap;
	ID3D11SamplerState* sampleStateClamp;

	//cbuffers
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* wMatBuffer;
	ID3D11Buffer* lightBuffer;  //hardcode instead
	ID3D11Buffer* lightBuffer2; //hardcode instead

	//cbufferStructs
	struct matrixCbuff
	{
		XMFLOAT4X4 lightViewMatrix;
		XMFLOAT4X4 lightProjectionMatrix;
	};

	matrixCbuff matrix_cbuffer;

	//D3D11_VIEWPORT lightViewport;
};