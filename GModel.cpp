#include "GModel.h"
#include "WICTextureLoader.h"
#include <limits>
#include <algorithm>
using namespace DirectX;

GModel::GModel()
{
	this->objectWorldMatrix = new SimpleMath::Matrix;
	this->objectWorldMatrix[0] = XMMatrixTranspose(DirectX::XMMatrixIdentity()); //DirectX need transposed matrices
	this->blendShape = false;
	this->DCM = false;
	noOfTextures = 1;
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
	XMStoreFloat4x4(&poop, XMMatrixTranslation(position.x, position.y, position.z));
	bBox.setAtMeshPosition(&poop); //send XMMATRIX in a package.
};
XMMATRIX GModel::getPosition() {
	return this->objectWorldMatrix[0];
}
//struct with vertex positions held by FbxDawg
void GModel::load(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath) //give diffuse/normal NULL for default values
{
	modelLoader.loadModels(fbxFilePath);
	//Note: Doing this vvvvvv may cause problems according to Martin, since it's vector = vector
	this->modelVertices = modelLoader.modelVertexList;
	this->modelTextureFilepath = modelLoader.textureFilepath;
	if (modelLoader.DCMmaterial->IsValid())
	{
		this->DCM = true;
	}
#pragma region VertexBuffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = modelVertices.size()*sizeof(MyVertexStruct);//fbxobj->modelVertexList.size()*sizeof(MyVertexStruct);//250 000 verticer * byte-storleken på en vertex för att få den totala byten
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = modelVertices.data();

	gDevice->CreateBuffer(&bufferDesc, &data, &modelVertexBuffer);
#pragma endregion VertexBuffer

#pragma region IndexBuffer

	this->IndexArray = modelLoader.FBXIndexArray; //Making it 123... for now. change will be made.
	this->sizeOfIndexArray = modelLoader.sizeOfFBXIndexArray;


	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = this->sizeOfIndexArray*sizeof(int);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = this->IndexArray;

	gDevice->CreateBuffer(&indexBufferDesc, &indexInitData, &this->modelIndexBuffer);
#pragma endregion IndexBuffer
#pragma region ConstantBuffer
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
#pragma endregion ConstantBuffer
	//Import from File
#pragma region
	HRESULT hr;

	CoInitialize(NULL);
	//Need to have this be part of the Render-looping-through-objects-loop. That is: not having modelList[0]
	if (diffusePath == NULL)
		hr = DirectX::CreateWICTextureFromFile(gDevice, modelTextureFilepath.c_str(), NULL, &modelTextureView[0]);
	else
		hr = DirectX::CreateWICTextureFromFile(gDevice, diffusePath, NULL, &modelTextureView[0]);

	if (normalPath == NULL)
	{
		noOfTextures = 1;
		hr = DirectX::CreateWICTextureFromFile(gDevice, L"finland", NULL, &modelTextureView[1]);
	}
	else {
		noOfTextures = 2;
		hr = DirectX::CreateWICTextureFromFile(gDevice, normalPath, NULL, &modelTextureView[1]);
	}
#pragma endregion

	//Making bbox
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
	//modelBBox.CreateFromPoints(modelBBox, maxPos, minPos);
	//now when I've got a bbox, I can do collision-detection with the frustum in the Frustum-class.

	//Create the bbox (my version)
	bBox.CreateBBox(XMFLOAT3(minX, minY, minZ), XMFLOAT3(maxX, maxY, maxZ));
};

void GModel::loadBlendShape(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath)
{
	this->blendShape = true;
	modelLoader.loadModels(fbxFilePath);


	this->modelVertices = modelLoader.modelVertexList;
	this->modelTextureFilepath = modelLoader.textureFilepath;
	for (int i = 0; i < this->modelVertices.size(); i++)
	{
		MyBSStruct temp;

		temp.x = modelLoader.modelVertexList[i].x;
		temp.y = modelLoader.modelVertexList[i].y;
		temp.z = modelLoader.modelVertexList[i].z;
		temp.norX = modelLoader.modelVertexList[i].norX;
		temp.norY = modelLoader.modelVertexList[i].norY;
		temp.norZ = modelLoader.modelVertexList[i].norZ;
		temp.u = modelLoader.modelVertexList[i].u;
		temp.v = modelLoader.modelVertexList[i].v;
		temp.bsx = modelLoader.blendShapes[i].x;
		temp.bsy = modelLoader.blendShapes[i].y;
		temp.bsz = modelLoader.blendShapes[i].z;
		//temp.bsnorX = BSmodelVertices[i].norX;
		//temp.bsnorY = BSmodelVertices[i].norY;
		//temp.bsnorZ = BSmodelVertices[i].norZ;
		//temp.bsu = BSmodelVertices[i].u;
		//temp.bsv = BSmodelVertices[i].v;

		modelWithBSstruct.push_back(temp);

	}



#pragma region VertexBuffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = modelWithBSstruct.size()*sizeof(MyBSStruct);//fbxobj->modelVertexList.size()*sizeof(MyVertexStruct);//250 000 verticer * byte-storleken på en vertex för att få den totala byten
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = modelWithBSstruct.data();

	gDevice->CreateBuffer(&bufferDesc, &data, &modelVertexBuffer);
#pragma endregion VertexBuffer

#pragma region IndexBuffer

	this->IndexArray = modelLoader.FBXIndexArray; //Making it 123... for now. change will be made.
	this->sizeOfIndexArray = modelLoader.sizeOfFBXIndexArray;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = modelVertices.size()*sizeof(int); //This is the size of the Index Array. 23 feb
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = this->IndexArray;

	gDevice->CreateBuffer(&indexBufferDesc, &indexInitData, &this->modelIndexBuffer);
#pragma endregion IndexBuffer
#pragma region ConstantBuffer


	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(bsWeight);
	gDevice->CreateBuffer(&bufferDesc, NULL, &bsWBuffer);


	//Creating constant buffer holding only worldmatrix
	//D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
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
#pragma endregion ConstantBuffer
#pragma region
	HRESULT hr;

	CoInitialize(NULL);
	if (diffusePath == NULL)
		hr = DirectX::CreateWICTextureFromFile(gDevice, modelTextureFilepath.c_str(), NULL, &modelTextureView[0]);
	else
		hr = DirectX::CreateWICTextureFromFile(gDevice, diffusePath, NULL, &modelTextureView[0]);

	if (normalPath == NULL)
	{
		noOfTextures = 1;
		hr = DirectX::CreateWICTextureFromFile(gDevice, L"finland", NULL, &modelTextureView[1]);
	}
	else {
		noOfTextures = 2;
		hr = DirectX::CreateWICTextureFromFile(gDevice, normalPath, NULL, &modelTextureView[1]);

	}
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
	//modelBBox.CreateFromPoints(modelBBox, maxPos, minPos);
	//now when I've got a bbox, I can do collision-detection with the frustum in the Frustum-class.

	//Create the bbox (my version)
	bBox.CreateBBox(XMFLOAT3(minX, minY, minZ), XMFLOAT3(maxX, maxY, maxZ));
};


int GModel::getNumberOfTextures()
{
	return noOfTextures;
}

void GModel::setBlendWeight(float weight, ID3D11DeviceContext* gDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	bsWeight* dataPtr;

	HRESULT hr = gDeviceContext->Map(bsWBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);
	dataPtr = (bsWeight*)gMappedResource.pData;

	dataPtr->weight = weight;

	gDeviceContext->Unmap(modelConstantBuffer, NULL);
}



void GModel::renderModel()
{
	//This here needs... The model's texture. Among other things.
	//

};