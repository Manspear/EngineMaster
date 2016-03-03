#include "GModel.h"
#include "WICTextureLoader.h"
#include <limits>
#include <algorithm>
using namespace DirectX;

GModel::GModel()
{
	this->objectWorldMatrix = new XMMATRIX;
	this->objectWorldMatrix[0] = XMMatrixTranspose(DirectX::XMMatrixIdentity()); //DirectX need transposed matrices
}

GModel::~GModel()
{
	//modelVertexBuffer->Release(); //This gets released...Somewhere.
	//modelTextureView[0]->Release();
	//modelTextureView[1]->Release();
	delete objectWorldMatrix;
}
void GModel::setPosition(DirectX::XMFLOAT4 position, ID3D11DeviceContext* gDeviceContext)
{
	//For this to work, we'll need our own world matrix, which we've got created in the GModel constructor.
	//XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	//translation = XMMatrixTranspose(translation); 
	//XMMATRIX identity = XMMatrixIdentity();
	//identity = XMMatrixTranspose(identity);

	this->objectWorldMatrix[0] = XMMatrixTranspose(XMMatrixIdentity() * XMMatrixTranslation(position.x, position.y, position.z)); //multiply transposed matrix with transposed matrix --> result is transposed 
																																  //this->objectWorldMatrix = XMMatrixTranspose(objectWorldMatrix);


	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	modelWorldStruct* dataPtr;

	gDeviceContext->Map(modelConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);
	dataPtr = (modelWorldStruct*)gMappedResource.pData;

	dataPtr->worldMatrix = objectWorldMatrix[0];

	gDeviceContext->Unmap(modelConstantBuffer, NULL);
	FXMVECTOR ass = XMVectorSet(position.x, position.y, position.z, position.w);
	FXMVECTOR asdf = XMQuaternionIdentity();

	//multiply the BoundingBox with the object's new world-matrix, so that it follows the object. 
	modelBBox.Transform(modelBBox, 1.0f, asdf, ass);

	//my bbox-code
	XMFLOAT4X4 poop;
	XMStoreFloat4x4(&poop, XMMatrixTranspose(XMMatrixIdentity() * XMMatrixTranslation(position.x, position.y, position.z)));
	bBox.setAtMeshPosition(&poop); //send XMMATRIX in a package.
};
XMMATRIX GModel::getPosition() {
	return this->objectWorldMatrix[0];
}
//struct with vertex positions held by FbxDawg
void GModel::load(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext) //This is used in the default-constructor of Engine.
{
	FbxDawg modelLoader;
	modelLoader.loadModels(fbxFilePath);
	//Note: Doing this vvvvvv may cause problems according to Martin, since it's vector = vector
	this->modelVertices = modelLoader.modelVertexList;
	this->modelTextureFilepath = modelLoader.textureFilepath;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = modelVertices.size()*sizeof(MyVertexStruct);//fbxobj->modelVertexList.size()*sizeof(MyVertexStruct);//250 000 verticer * byte-storleken på en vertex för att få den totala byten
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = modelVertices.data();

	gDevice->CreateBuffer(&bufferDesc, &data, &modelVertexBuffer);

	//Creating constant buffer holding only worldmatrix
	//D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(modelWorldStruct);
	gDevice->CreateBuffer(&bufferDesc, NULL, &modelConstantBuffer);

	//Giving the constant buffer data

	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	modelWorldStruct* dataPtr;

	gDeviceContext->Map(modelConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);
	dataPtr = (modelWorldStruct*)gMappedResource.pData;

	dataPtr->worldMatrix = objectWorldMatrix[0];

	gDeviceContext->Unmap(modelConstantBuffer, NULL);

	//Import from File
#pragma region
	HRESULT hr;
	ID3D11ShaderResourceView * Texture;
	CoInitialize(NULL);
	//Need to have this be part of the Render-looping-through-objects-loop. That is: not having modelList[0]
	hr = DirectX::CreateWICTextureFromFile(gDevice, modelTextureFilepath.c_str(), NULL, &modelTextureView[0]);//wstring äger functionen c_str som är en getFucntion till wchar_t* som finns redan
	hr = DirectX::CreateWICTextureFromFile(gDevice, L"./Images/normal_map.jpg", NULL, &modelTextureView[1]);
	//(d3d11DeviceInterface, d3d11DeviceContextInterface, L"test.bmp", 0, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0, 0, &pTex2D, NULL);
#pragma endregion 

	//Loop through the vertices to get the minimum and maximum xyz values to be used for BBox creation.
	//This could be done in the FbxDawg-class, to spare the CPU when you're loading lots of models.
	float maxX = FLT_MIN;
	float minX = FLT_MAX;
	float maxY = FLT_MIN;
	float minY = FLT_MAX;
	float maxZ = FLT_MIN;
	float minZ = FLT_MAX;
	for (int i = 0; i < modelVertices.size(); i++) {
		//get the min and max values of the model
		if (maxX < modelVertices[i].x)
			maxX = modelVertices[i].x;
		if (minX > modelVertices[i].x)
			minX = modelVertices[i].x;
		if (maxY < modelVertices[i].y)
			maxY = modelVertices[i].y;
		if (minY > modelVertices[i].y)
			minY = modelVertices[i].y;
		if (maxZ < modelVertices[i].z)
			maxZ = modelVertices[i].z;
		if (minZ > modelVertices[i].z)
			minZ = modelVertices[i].z;
	}
	//make two XMVECTORs that we will create the bbox from
	XMVECTOR maxPos = XMVectorSet(maxX, maxY, maxZ, 1);
	XMVECTOR minPos = XMVectorSet(minX, minY, minZ, 1);
	modelBBox.CreateFromPoints(modelBBox, maxPos, minPos);
	//now when I've got a bbox, I can do collision-detection with the frustum in the Frustum-class.

	//Create the bbox (my version)
	bBox.CreateBBox(XMFLOAT3(minX, minY, minZ), XMFLOAT3(maxX, maxY, maxZ));
};


void GModel::renderModel()
{
	//This here needs... The model's texture.
	//

};