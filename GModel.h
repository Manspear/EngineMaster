#ifndef GMODEL_H
#define GMODEL_H
//#include "Engine.h"
#include "FbxDawg.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

using namespace DirectX;


class GModel
{
private:
	DirectX::XMMATRIX objectWorldMatrix;
	int noOfTextures;
	FbxDawg modelLoader;
public:
	struct modelWorldStruct {
		XMMATRIX worldMatrix;
	};
	GModel();
	~GModel();
	ID3D11Buffer* modelVertexBuffer = nullptr;
	ID3D11Buffer* modelConstantBuffer = nullptr;
	ID3D11ShaderResourceView* modelTextureView[2]; //texture then normal map
	//share projection and view, but have different world-view.
	void setPosition(DirectX::XMFLOAT4 position, ID3D11DeviceContext* gDeviceContext);
	XMMATRIX getPosition();
	void renderModel();
	std::vector<MyVertexStruct> modelVertices; //This holds the vertices.
	std::wstring modelTextureFilepath; //THis holds the texture's file-path
	int getNumberOfTextures();
	//struct with vertex positions held by FbxDawg
	void load(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath); //<-- Loads the model. Means that modelLoader is called.

};

//>>>>>>>USER MANUAL<<<<<<<<<<<
//Look at GModel.cpp
//note the load()-function. It loads data from the Fbx-file with the FbxDawg-class-functions
//and creates both a textureView and a vertexBuffer.
//Now look at Engine.cpp
//In InitializeModels, you set the number of models you load, and which Fbx-file yoy load from.
//Now look at Engine::Render()
//Here a new vertexBuffer, and texture-resource is set per object (by looping through the Engine-variable numberOfModels)
//then a draw()-call is initialized, the number of vertices drawn equal to the number of vertices in
//the GModel::modelVertices - vector - array.






#endif // !GMODEL_H

