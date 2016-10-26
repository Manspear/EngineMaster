#pragma once

#include <windows.h>

#include <d3d11.h>

#include <d3dcompiler.h> 

#include <directXMath.h>

using namespace DirectX;

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
	*/
	bool RenderShadowed(ID3D11DeviceContext* deviceContext, ID3D11Buffer* vertexBuffer, ShaderType shadTp, UINT32 vertexSize);

	ID3D11Texture2D* getDepthTexture();

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	/*bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMMATRIX lightPosition,
		XMMATRIX ambientColor, XMMATRIX diffuseColor);*/
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

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
	ID3D11DepthStencilView* pDepthStencilView = nullptr;

	//This shader calculates the depth buffer used for the depth-compare
	ID3D11VertexShader* firstPassShader = nullptr;
	//Holds the "vanilla" geometry, no skeletal or BS stuff
	ID3D11InputLayout* firstPassVertexLayout = nullptr; 

	ID3D11ShaderResourceView* pShadowResource;

	//D3D11_VIEWPORT lightViewport;
};