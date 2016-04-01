#pragma once
#ifndef DEFERREDSHADER_H

#include <d3d11.h>
#include <directXMath.h>

using namespace DirectX;

class DeferredShader
{
public:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	DeferredShader();
	DeferredShader(const DeferredShader&);
	~DeferredShader();

	void initialize(ID3D11Device*, ID3D11DeviceContext*, HWND);
	void shutdown();
	bool Render(int, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView*);



private:
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	HWND wnd;

	void initializeShader();
	void shutdownShader();
	
	void outputShaderErrorMessage(ID3DBlob, WCHAR);
	void setShaderParameters(XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView*);


};



#endif