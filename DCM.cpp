#include "DCM.h"

DCM::DCM()
{
}

DCM::~DCM()
{
}

void DCM::Dynamic_Cube_Map(ID3D11DeviceContext *gDevice)
{
	static const int CubeMapSize = 256;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = CubeMapSize;
	texDesc.Height = CubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
	ID3D11Texture2D* cubeTex = 0;
	hr = gDevice->CreateTexture2D(&textureDesc, &data, &pTexture);





}
