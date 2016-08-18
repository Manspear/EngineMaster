#ifndef GMODEL_H
#define GMODEL_H
//#include "Engine.h"
#include "FbxDawg.h"
#include "GBoundingBox.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <string>
#include "SimpleMath.h"
#include <stdlib.h>

using namespace DirectX;


class GModel
{
private:

	int noOfTextures;
	FbxDawg modelLoader;
	FbxDawg BSLoader;
	bool blendShape;
	bool DCM;
public:
	SimpleMath::Matrix* objectWorldMatrix;
	struct modelWorldStruct {
		XMMATRIX worldMatrix;
	};
	struct bsWeight {
		float weight;
		float padding0;
		float padding1;
		float padding2;
	};
	GModel();
	~GModel();
	bool isCulled = false; //This bool will serve as a tag, it is set by the GFrustum calling the QuadTreeCollision-function.
	ID3D11Buffer* modelVertexBuffer = nullptr;
	ID3D11Buffer* animModelVertexBuffer = nullptr;
	ID3D11Buffer* modelIndexBuffer = nullptr;
	ID3D11Buffer* modelConstantBuffer = nullptr;
	DirectX::XMVECTOR pivotPoint;
	BoundingBox modelBBox;
	GBoundingBox bBox;
	ID3D11Buffer* bsWBuffer = nullptr;
	int* IndexArray = nullptr;
	int sizeOfIndexArray = 0;

	ID3D11ShaderResourceView* modelTextureView[2]; //texture then normal map
												   //share projection and view, but have different world-view.
	void setPosition(DirectX::XMFLOAT4 position, ID3D11DeviceContext* gDeviceContext);
	XMMATRIX getPosition();
	void renderModel();
	std::vector<MyVertexStruct> modelVertices;
	std::vector<AnimVertexStruct> animModelVertices;
	std::vector<MyVertexStruct> BSmodelVertices;
	std::vector<MyBSStruct> modelWithBSstruct;
	std::vector<MyVertexStruct> bsVertices;//This holds the vertices.
	std::wstring modelTextureFilepath; //THis holds the texture's file-path
	int getNumberOfTextures();
	const bool& hasBlendShape() const { return blendShape; }
	const bool& hasDCM() const { return DCM; }
	void setBlendWeight(float weight, ID3D11DeviceContext* gDeviceContext);
	//struct with vertex positions held by FbxDawg
	void load(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath); //<-- Loads the model. Means that modelLoader is called.
	void loadBlendShape(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath);
	void loadAnimMesh(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath);
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

