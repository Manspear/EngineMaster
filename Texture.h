#pragma once
#ifndef TEXTURE
#define TEXTURE

#include <d3d11.h>
#include <stdio.h>
#include <d3dcompiler.h> 


class TextureClass
{

public:

	TextureClass();
	
	~TextureClass();

	//bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	//void shutdown();
	void LoadTexture();
	//ID3D11ShaderResourceView* GetTexture();




};
#endif