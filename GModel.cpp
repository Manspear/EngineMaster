#include "GModel.h"
#include "WICTextureLoader.h"
#include <limits>
#include <algorithm>
using namespace DirectX;

void GModel::updateJointMatrices(std::vector<FbxDawg::sKeyFrame> inputList, ID3D11DeviceContext * gDeviceContext)
{
	XMMATRIX tMatrices[NUMBEROFJOINTS];
	XMMATRIX finalTMatrices[NUMBEROFJOINTS];
	std::vector<XMFLOAT4X4> finalMatrices;
	finalMatrices.resize(modelLoader.skeleton.joints.size());
	for (int i = 0; i < inputList.size(); i++)
	{
		/*XMVECTOR tempRot = XMVectorSet(inputList[i].rotation.x, inputList[i].rotation.y, inputList[i].rotation.z, inputList[i].rotation.w);
		XMMATRIX rotation = XMMatrixRotationQuaternion(tempRot);*/

		XMMATRIX translation = XMMatrixTranslation(inputList[i].translation.x, inputList[i].translation.y, inputList[i].translation.z);
		
		XMMATRIX rotation;
		makeRotationMatrix(inputList[i].rotation, rotation);
		XMMATRIX scaling  = XMMatrixScaling(inputList[i].scale.x, inputList[i].scale.y, inputList[i].scale.z);
		
		XMMATRIX SRT = scaling * rotation * translation;
		tMatrices[i] = SRT;//XMMatrixTranspose(TRS);
	}
	//starts at 1 to skip the root
	//XMMATRIX worldMat = XMMatrixTranspose(XMLoadFloat4x4(objectWorldMatrix));

	//XMMATRIX worldMat = XMMatrixIdentity();
	//tMatrices[0] = worldMat * tMatrices[0];
	XMMATRIX invertZ = XMMatrixScaling(1, 1, -1);
	
	
	XMMATRIX invBPose = XMLoadFloat4x4(&modelLoader.skeleton.joints[0].globalBindPoseInverse);
	//invBPose = XMMatrixTranspose(invBPose);
	finalTMatrices[0] = XMMatrixMultiply(tMatrices[0], invBPose);//invBPose * tMatrices[0];
	//tMatrices[0] = XMMatrixMultiply(tMatrices[0], invBPose);
																 //finalTMatrices[0] = tMatrices[0];

	for (int i = 1; i < inputList.size(); i++)
	{
		//if no child ever has an uncalculated parent, this works
		int parentIndex = modelLoader.skeleton.joints[i].parentJointIndex;
		tMatrices[i] =  tMatrices[i] * tMatrices[parentIndex];
		XMMATRIX invBPose = XMLoadFloat4x4(&modelLoader.skeleton.joints[i].globalBindPoseInverse);
		finalTMatrices[i] = invBPose * tMatrices[i];
		//finalTMatrices[i] = finalTMatrices[i] * invertZ;
	}
	for (int i = 0; i < inputList.size(); i++)
	{
		/*XMMATRIX popo = XMMatrixTranspose(tMatrices[i]);
		XMStoreFloat4x4(&jointMatrices.jointTransforms[i], popo);*/
		XMStoreFloat4x4(&jointMatrices.jointTransforms[i], finalTMatrices[i]);
	}
	D3D11_MAPPED_SUBRESOURCE subRez;
	jointStruct* dataPtr;
	gDeviceContext->Map(jointBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &subRez);
	dataPtr = (jointStruct*)subRez.pData;
	
	for (int i = 0; i < NUMBEROFJOINTS; i++)
	{
		dataPtr->jointTransforms[i] = jointMatrices.jointTransforms[i];
	}
	gDeviceContext->Unmap(jointBuffer, NULL);
}

void GModel::makeRotationMatrix(XMFLOAT4 in, XMMATRIX & result)
{
	XMMATRIX rotX =
	XMMatrixSet(
		1.f,			0.f,					0.f,				0.f,
		0.f,		    cosf(in.x),	    sinf(in.x),	0.f,
		0.f,		    -sinf(in.x),     cosf(in.x),  0.f,
		0.f,			0.f,					0.f,				1.f
	);
	XMMATRIX rotY =
	XMMatrixSet(
		cosf(in.y),	0.f,			-sinf(in.y),		0.f,
		0.f,				1.f,			0.f,					0.f,
		sinf(in.y),  0.f,			cosf(in.y),		0.f,
		0.f,				0.f,			0.f,					1.f
	);
	XMMATRIX rotZ =
	XMMatrixSet(
		cosf(in.z),	sinf(in.z),		0.f,				0.f,
		-sinf(in.z), cosf(in.z),		0.f,				0.f,
		0.f,				0.f,					1.f,				0.f,
		0.f,				0.f,					0.f,				1.f
	);
	//XMMATRIX resultMat = rotX  * rotY * rotZ;
	/*rotX = XMMatrixRotationX(in.x);
	rotY = XMMatrixRotationY(in.y);
	rotZ = XMMatrixRotationZ(in.z);*/

	XMMATRIX resultMat = rotX * rotY * rotZ;
	//XMMATRIX resultMat = rotZ * rotY * rotX;
	//XMMATRIX resultMat = rotY * rotX * rotZ;
	//XMMATRIX resultMat = rotY * rotZ * rotX;
	//XMMATRIX resultMat = rotX * rotZ * rotY;
	//XMMATRIX resultMat = rotZ * rotX * rotY;
	//I think this si right. THis seems to "mirror" the rotation.
	result = resultMat;
	//result = XMMatrixTranspose(resultMat);
}

FbxDawg::sKeyFrame GModel::interpolateKeys(FbxDawg::sKeyFrame keyOver, FbxDawg::sKeyFrame keyUnder, float targetTime)
{
	float diffKeys = keyOver.keyTime - keyUnder.keyTime;
	if (diffKeys == 0)
	{
		return keyOver;
	}
	float diffUnderTime = abs(targetTime - keyUnder.keyTime);
	float underAffect = diffUnderTime / diffKeys;
	XMVECTOR overTrans;
	XMVECTOR underTrans;
	XMVECTOR overRot;
	XMVECTOR underRot;
	XMVECTOR overScale;
	XMVECTOR underScale;
	overTrans = XMLoadFloat4(&keyOver.translation);
	underTrans = XMLoadFloat4(&keyUnder.translation);
	overRot = XMLoadFloat4(&keyOver.rotation);
	underRot = XMLoadFloat4(&keyUnder.rotation);
	overScale = XMLoadFloat4(&keyOver.scale);
	underScale = XMLoadFloat4(&keyUnder.scale);
	XMVECTOR resTrans = XMVectorLerp(underTrans, overTrans, underAffect);
	XMVECTOR resRot = XMVectorLerp(underRot, overRot, underAffect);
	XMVECTOR resScale = XMVectorLerp(underScale, overScale, underAffect);
	float resTime = (keyUnder.keyTime * (1 - underAffect)) + (keyOver.keyTime * underAffect);
	
	FbxDawg::sKeyFrame interpolatedKey;
	XMStoreFloat4(&interpolatedKey.translation, resTrans);
	XMStoreFloat4(&interpolatedKey.rotation, resRot);
	XMStoreFloat4(&interpolatedKey.scale, resScale);
	interpolatedKey.keyTime = resTime;

	return interpolatedKey;
}

GModel::GModel()
{
	this->objectWorldMatrix = new SimpleMath::Matrix;
	this->objectWorldMatrix[0] = XMMatrixTranspose(DirectX::XMMatrixIdentity()); //DirectX need transposed matrices
	this->blendShape = false;
	animationTime = 0;
	noOfTextures = 1;
	hasSkeleton = false;
}

GModel::~GModel()
{
	//modelVertexBuffer->Release(); //This gets released...Somewhere.
	//modelTextureView[0]->Release();
	//modelTextureView[1]->Release();
	delete objectWorldMatrix;

}
bool GModel::isAnimated()
{
	return hasSkeleton;
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
	pivotPoint = modelLoader.pivotValue;
	this->modelVertices = modelLoader.modelVertexList;
	
	this->modelTextureFilepath = modelLoader.textureFilepath;
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
	pivotPoint = modelLoader.pivotValue;

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
}
void GModel::loadAnimMesh(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath)
{
	hasSkeleton = true;
	modelLoader.loadModels(fbxFilePath);
	pivotPoint = modelLoader.pivotValue;
	//Note: Doing this vvvvvv may cause problems according to Martin, since it's vector = vector
	this->animModelVertices = modelLoader.animModelVertexList;
	

	this->modelTextureFilepath = modelLoader.textureFilepath;
#pragma region VertexBuffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = animModelVertices.size() * sizeof(AnimVertexStruct);//fbxobj->modelVertexList.size()*sizeof(MyVertexStruct);//250 000 verticer * byte-storleken på en vertex för att få den totala byten
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = animModelVertices.data();

	gDevice->CreateBuffer(&bufferDesc, &data, &animModelVertexBuffer);
#pragma endregion VertexBuffer

#pragma region IndexBuffer

	this->IndexArray = modelLoader.FBXIndexArray; //Making it 123... for now. change will be made.
	this->sizeOfIndexArray = modelLoader.sizeOfFBXIndexArray;


	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = this->sizeOfIndexArray * sizeof(int);
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
	makeJointBuffer(gDevice);


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
	for (int i = 0; i < animModelVertices.size(); i++) {
		//get the min and max values of the model
		if (maxX < animModelVertices[i].x)
			maxX = animModelVertices[i].x;
		if (minX > animModelVertices[i].x)
			minX = animModelVertices[i].x;
		if (maxY < animModelVertices[i].y)
			maxY = animModelVertices[i].y;
		if (minY > animModelVertices[i].y)
			minY = animModelVertices[i].y;
		if (maxZ < animModelVertices[i].z)
			maxZ = animModelVertices[i].z;
		if (minZ > animModelVertices[i].z)
			minZ = animModelVertices[i].z;
	}
	//make two XMVECTORs that we will create the bbox from
	XMVECTOR maxPos = XMVectorSet(maxX, maxY, maxZ, 1);
	XMVECTOR minPos = XMVectorSet(minX, minY, minZ, 1);
	//modelBBox.CreateFromPoints(modelBBox, maxPos, minPos);
	//now when I've got a bbox, I can do collision-detection with the frustum in the Frustum-class.

	//Create the bbox (my version)
	bBox.CreateBBox(XMFLOAT3(minX, minY, minZ), XMFLOAT3(maxX, maxY, maxZ));
}
void GModel::makeJointBuffer(ID3D11Device* gDevice)
{
	D3D11_BUFFER_DESC bDesc;
	bDesc.ByteWidth = sizeof(jointStruct);
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &jointMatrices;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr;
	hr = gDevice->CreateBuffer(&bDesc, &InitData, &jointBuffer);
}
void GModel::updateAnimation(ID3D11DeviceContext * gDeviceContext, double dt)
{
	//First get target time
	animationTime += dt * 0.3;
	
	//first find the closest keyframe on each joint
	//keyList is used to fill the matrixList

	//std::vector<FbxDawg::sKeyFrame> keyList;

	//keyList.resize(modelLoader.skeleton.joints.size());
	//for (int i = 0; i < modelLoader.skeleton.joints.size(); i++)
	//{
	//	float targetTime = abs(std::fmod(animationTime, modelLoader.skeleton.joints[i].animLayer[0].keyFrame.back().keyTime));
	//	float timeCompare = FBXSDK_FLOAT_MAX;
	//	for (int j = 0; j < modelLoader.skeleton.joints[i].animLayer[0].keyFrame.size(); j++)
	//	{
	//		float currKeyTime = modelLoader.skeleton.joints[i].animLayer[0].keyFrame[j].keyTime;
	//		float diff = abs(targetTime - currKeyTime);
	//		if (diff < timeCompare)
	//		{
	//			timeCompare = diff;
	//			keyList[i] = modelLoader.skeleton.joints[i].animLayer[0].keyFrame[j];
	//		}	
	//	}
	//}

	std::vector<FbxDawg::sKeyFrame> keyList;
	keyList.resize(modelLoader.skeleton.joints.size());

	for (int i = 0; i < modelLoader.skeleton.joints.size(); i++)
	{
		float jointMaxTime = modelLoader.skeleton.joints[i].animLayer[0].keyFrame.back().keyTime;
		float targetTime = abs(std::fmod(animationTime, jointMaxTime));
		float timeOverCompare = FBXSDK_FLOAT_MAX;
		float timeUnderCompare = -FBXSDK_FLOAT_MAX;
		FbxDawg::sKeyFrame keyOver;
		FbxDawg::sKeyFrame keyUnder;
		const int numberOfKeys = modelLoader.skeleton.joints[i].animLayer[0].keyFrame.size();
		for (int j = 0; j < numberOfKeys; j++)
		{
			//när animationen precis tar slut, beräknas keyframes fel
			float currKeyTime = modelLoader.skeleton.joints[i].animLayer[0].keyFrame[j].keyTime;
			float diff = targetTime - currKeyTime;
			
			if (timeOverCompare == FBXSDK_FLOAT_MAX && timeUnderCompare == -FBXSDK_FLOAT_MAX)
			{
				keyOver = modelLoader.skeleton.joints[i].animLayer[0].keyFrame[j];
				keyUnder = keyOver;
				timeOverCompare = diff;
			}
			//if the keyTime is larger that the targetTime
			if (diff > 0 && diff < timeOverCompare)
			{
				keyUnder = modelLoader.skeleton.joints[i].animLayer[0].keyFrame[j];
				timeOverCompare = diff;
			}
			//if the keytime is less than the targetTime
			else if (diff < 0 && diff > timeUnderCompare)
			{
				keyOver = modelLoader.skeleton.joints[i].animLayer[0].keyFrame[j];
				timeUnderCompare = diff;
			}
		}
		keyList[i] = interpolateKeys(keyOver, keyUnder, targetTime);
	}

	updateJointMatrices(keyList, gDeviceContext);
}
;


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