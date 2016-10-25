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
	/*Same dimensions as the viewport renderer*/
	void createDepthBuffer(ID3D11DeviceContext* deviceContext, ID3D11Device* device);
	ShadowMap();
	/*Remember that XMMATRIX cannot be sent between functions of classes. Use XMFLOAT4X4 instead*/

	bool initialize(ID3D11Device* device, HWND handle);
	void uninitialize();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMMATRIX lightPosition,
		XMMATRIX ambientColor, XMMATRIX diffuseColor);

	ID3D11Texture2D* getDepthTexture();

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMMATRIX lightPosition,
		XMMATRIX ambientColor, XMMATRIX diffuseColor);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

	/*This is the depth-buffer saved from the light-pass*/
	ID3D11Texture2D* pDepthStencil;

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;

	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightBuffer2;
};