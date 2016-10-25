#pragma once

#include <windows.h>

#include <d3d11.h>

#include <d3dcompiler.h> 

#include <directXMath.h>

using namespace DirectX;
class ShadowMap
{
public:
	ShadowMap();
	/*Remember that XMMATRIX cannot be sent between functions of classes. Use XMFLOAT4X4 instead*/
	struct MatrixBufferType
	{
		
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct LightBufferType
	{
		XMMATRIX ambientColor;
		XMMATRIX diffuseColor;
	};

	struct LightBufferType2
	{
		XMMATRIX lightPosition;
		float padding;
	};

	bool initialize(ID3D11Device* device, HWND handle);
	void uninitialize();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMMATRIX lightPosition,
		XMMATRIX ambientColor, XMMATRIX diffuseColor);

	void shutDown();

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMMATRIX lightPosition,
		XMMATRIX ambientColor, XMMATRIX diffuseColor);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);


	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;

	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightBuffer2;
};