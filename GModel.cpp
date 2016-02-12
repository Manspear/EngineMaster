#include "GModel.h"
GModel::GModel()
{

}

GModel::~GModel()
{

}
//void GModel::setPosition(XMMATRIX translationMatrix) 
//{
//	//For this to work, we'll need our own world matrix, which we've got created in the GModel constructor.
//	objectWorldMatrix = XMMatrixMultiply(objectWorldMatrix, translationMatrix);
//};
//struct with vertex positions held by FbxDawg
void GModel::load(const char* fbxFilePath, ID3D11Device* gDevice) //This is used in the default-constructor of Engine.
{
	FbxDawg modelLoader;
	modelLoader.loadModels(fbxFilePath);
	//Note: Doing this vvvvvv may cause problems according to Martin, since it's vector = vector
	modelVertices = modelLoader.modelVertexList; 

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = modelVertices.size()*sizeof(MyVertexStruct);//fbxobj->modelVertexList.size()*sizeof(MyVertexStruct);//250 000 verticer * byte-storleken på en vertex för att få den totala byten

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = modelVertices.data();

	gDevice->CreateBuffer(&bufferDesc, &data, &modelVertexBuffer);
}; 


void GModel::renderModel()
{
	//This here needs... The model's texture.
	//

};