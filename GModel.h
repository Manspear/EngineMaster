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
#include "DeltaTime.h"
#include <cmath>

using namespace DirectX;
#define NUMBEROFJOINTS 36

class GModel
{
private:

	int noOfTextures;
	FbxDawg modelLoader;
	FbxDawg BSLoader;
	bool blendShape;
	bool hasSkeleton;

	void updateJointMatrices(std::vector<FbxDawg::sKeyFrame> inputList, ID3D11DeviceContext* gDeviceContext);


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
	struct jointStruct
	{
		XMFLOAT4X4 jointTransforms[NUMBEROFJOINTS];
	};
	GModel();
	~GModel();
	bool isCulled = false; //This bool will serve as a tag, it is set by the GFrustum calling the QuadTreeCollision-function.
	bool isAnimated();
	ID3D11Buffer* modelVertexBuffer = nullptr;
	ID3D11Buffer* animModelVertexBuffer = nullptr;
	ID3D11Buffer* modelIndexBuffer = nullptr;
	ID3D11Buffer* modelConstantBuffer = nullptr;
	ID3D11Buffer* jointBuffer = nullptr;
	jointStruct jointMatrices;
	DirectX::XMFLOAT4 pivotPoint;
	BoundingBox modelBBox;
	GBoundingBox bBox;
	ID3D11Buffer* bsWBuffer = nullptr;
	int* IndexArray = nullptr;
	int sizeOfIndexArray = 0;
	float animationTime;

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
	void setBlendWeight(float weight, ID3D11DeviceContext* gDeviceContext);
	//struct with vertex positions held by FbxDawg
	void load(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath); //<-- Loads the model. Means that modelLoader is called.
	void loadBlendShape(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath);
	void loadAnimMesh(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath);
	void makeJointBuffer(ID3D11Device* gDevice);
	void updateAnimation(ID3D11DeviceContext* gDeviceContext);
	
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

