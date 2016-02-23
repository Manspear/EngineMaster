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
void GModel::load(const char* fbxFilePath, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext) //This is used in the default-constructor of Engine.
{
	FbxDawg modelLoader;
	modelLoader.loadModels(fbxFilePath);
	//Note: Doing this vvvvvv may cause problems according to Martin, since it's vector = vector
	this->modelVertices = modelLoader.modelVertexList;
	this->IndexArray = modelLoader.indexArray;
	this->modelTextureFilepath = modelLoader.textureFilepath;

	

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = modelVertices.size()*sizeof(MyVertexStruct);//fbxobj->modelVertexList.size()*sizeof(MyVertexStruct);//250 000 verticer * byte-storleken p� en vertex f�r att f� den totala byten
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = modelVertices.data();

	gDevice->CreateBuffer(&bufferDesc, &data, &modelVertexBuffer);
	
#pragma region index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = modelVertices.size()*sizeof(int); //sizeof(int) * len(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = this->IndexArray;
	
	gDevice->CreateBuffer(&indexBufferDesc, &indexInitData, &this->modelIndexBuffer);
	
	//printf("%d\n", sizeof(modelLoader.indexArray));
	for (int i = 0;i < modelVertices.size();i++)
	{
		printf("%d\n", IndexArray[i]);
	}

	//gDeviceContext->IASetIndexBuffer(modelIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

#pragma endregion END OF INDEX BUFFER


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

	//Import from File
	#pragma region
	HRESULT hr;
	ID3D11ShaderResourceView * Texture;
	CoInitialize(NULL);
	//Need to have this be part of the Render-looping-through-objects-loop. That is: not having modelList[0]
	hr = DirectX::CreateWICTextureFromFile(gDevice, modelTextureFilepath.c_str(), NULL, &modelTextureView[0]);//wstring �ger functionen c_str som �r en getFucntion till wchar_t* som finns redan
	hr = DirectX::CreateWICTextureFromFile(gDevice, L"./Images/normal_map.jpg", NULL, &modelTextureView[1]);
	//(d3d11DeviceInterface, d3d11DeviceContextInterface, L"test.bmp", 0, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0, 0, &pTex2D, NULL);
	#pragma endregion 
}; 


void GModel::renderModel()
{
	//This here needs... The model's texture.
	//

};