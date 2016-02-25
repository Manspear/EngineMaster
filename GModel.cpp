#include "GModel.h"
#include "WICTextureLoader.h"

GModel::GModel()
{

	this->objectWorldMatrix = XMMatrixTranspose(DirectX::XMMatrixIdentity()); //DirectX need transposed matrices
}

GModel::~GModel()
{
	//modelVertexBuffer->Release(); //This gets released...Somewhere.
	//modelTextureView[0]->Release();
	//modelTextureView[1]->Release();


}
void GModel::setPosition(DirectX::XMFLOAT4 position, ID3D11DeviceContext* gDeviceContext)
{
	//For this to work, we'll need our own world matrix, which we've got created in the GModel constructor.
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	translation = XMMatrixTranspose(translation);
	XMMATRIX identity = XMMatrixIdentity();
	identity = XMMatrixTranspose(identity);
	objectWorldMatrix = identity * translation; //multiply transposed matrix with transposed matrix --> result is transposed (I think)

	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	modelWorldStruct* dataPtr;

	gDeviceContext->Map(modelConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &gMappedResource);
	dataPtr = (modelWorldStruct*)gMappedResource.pData;

	dataPtr->worldMatrix = objectWorldMatrix;

	gDeviceContext->Unmap(modelConstantBuffer, NULL);
};
XMMATRIX GModel::getPosition() {
	return this->objectWorldMatrix;
}
//struct with vertex positions held by FbxDawg
void GModel::load(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, const wchar_t* diffusePath, const wchar_t* normalPath) //give diffuse/normal NULL for default values
{
	modelLoader.loadModels(fbxFilePath);
	//Note: Doing this vvvvvv may cause problems according to Martin, since it's vector = vector
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
	
	this->IndexArray= modelLoader.FBXIndexArray; //Making it 123... for now. change will be made.


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

	dataPtr->worldMatrix = objectWorldMatrix;

	gDeviceContext->Unmap(modelConstantBuffer, NULL);
#pragma endregion ConstantBuffer
	//Import from File
	#pragma region
	HRESULT hr;
	ID3D11ShaderResourceView * Texture;
	CoInitialize(NULL);
	//Need to have this be part of the Render-looping-through-objects-loop. That is: not having modelList[0]
	if (diffusePath == NULL)
		hr = DirectX::CreateWICTextureFromFile(gDevice, modelTextureFilepath.c_str(), NULL, &modelTextureView[0]);
	else
		hr = DirectX::CreateWICTextureFromFile(gDevice, diffusePath, NULL, &modelTextureView[0]);

	if (normalPath == NULL)
	{
		noOfTextures = 1;
	}else{
		noOfTextures = 2;
		//wstring äger functionen c_str som är en getFucntion till wchar_t* som finns redan
		hr = DirectX::CreateWICTextureFromFile(gDevice, normalPath, NULL, &modelTextureView[1]);
		//(d3d11DeviceInterface, d3d11DeviceContextInterface, L"test.bmp", 0, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0, 0, &pTex2D, NULL);
	}
	#pragma endregion 
}; 



int GModel::getNumberOfTextures()
{
	return noOfTextures;
}



void GModel::renderModel()
{
	//This here needs... The model's texture.
	//

};