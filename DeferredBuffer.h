#pragma

#ifndef DEFERREDBUFFER_H

#include <d3d11.h>
#include <directXMath.h>

const int BUFFER_COUNT = 2;

class DeferredBuffer
{
public:
	DeferredBuffer();
	DeferredBuffer(const DeferredBuffer&);
	~DeferredBuffer();

	void initialize(ID3D11Device *, ID3D11DeviceContext *, int, int, float, float);
	void Shutdown();

	void setRenderTarget();
	void clearRenderTargets(float, float, float, float);

	ID3D11ShaderResourceView * getShaderResource(int);

private:
	int textureWidth, textureHeight;

	ID3D11Device * gDevice;
	ID3D11DeviceContext * gDeviceContext;

	ID3D11Texture2D* renderTargerTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* renderTargetVíewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT viewport;
};

#endif DEFERREDBUFFER_H