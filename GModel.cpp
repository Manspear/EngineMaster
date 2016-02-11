#include "GModel.h"

void GModel::setPosition(XMMATRIX translationMatrix) 
{
	//For this to work, we'll need our own world matrix, which we've got created in the GModel constructor.
	objectWorldMatrix = XMMatrixMultiply(objectWorldMatrix, translationMatrix);
};
//struct with vertex positions held by FbxDawg
void GModel::load(const char* fbxFilePath)
{
	FbxDawg modelLoader;
	modelLoader.loadModels(fbxFilePath);
	//Note: Doing this vvvvvv may cause problems according to Martin, since it's vector = vector
	modelVertices = modelLoader.modelVertexList; 
}; 

void GModel::renderModel() 
{
	//This here needs... The model's texture.
	//

};