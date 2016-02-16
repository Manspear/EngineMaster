#include "GModel.h"
#include "WICTextureLoader.h"

GModel::GModel()
{
	this->objectWorldMatrix = DirectX::XMMatrixIdentity();

}

GModel::~GModel()
{
	//modelVertexBuffer->Release(); //This gets released...Somewhere.
	//modelTextureView[0]->Release();
	//modelTextureView[1]->Release();
}
void GModel::setPosition(DirectX::XMFLOAT3 position) 
{
	//For this to work, we'll need our own world matrix, which we've got created in the GModel constructor.
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	objectWorldMatrix *= translation;
};
//struct with vertex positions held by FbxDawg
void GModel::load(const char* fbxFilePath, ID3D11Device* gDevice) //This is used in the default-constructor of Engine.
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
	
	#pragma region //Import from File
	HRESULT hr;
	ID3D11ShaderResourceView * Texture;
	CoInitialize(NULL);
	//Need to have this be part of the Render-looping-through-objects-loop. That is: not having modelList[0]
	hr = DirectX::CreateWICTextureFromFile(gDevice, modelTextureFilepath.c_str(), NULL, &modelTextureView[0]);//wstring äger functionen c_str som är en getFucntion till wchar_t* som finns redan
	hr = DirectX::CreateWICTextureFromFile(gDevice, L"./Images/normal_map.jpg", NULL, &modelTextureView[1]);
	//(d3d11DeviceInterface, d3d11DeviceContextInterface, L"test.bmp", 0, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0, 0, &pTex2D, NULL);
	#pragma endregion 
}; 


void GModel::renderModel()
{
	//This here needs... The model's texture.
	//

};