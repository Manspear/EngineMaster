#ifndef GMODEL_H
#define GMODEL_H
//#include "Engine.h"
#include "FbxDawg.h"

class GModel
{
private:
	
public:
	GModel();
	~GModel();

	//vertex buffer, constant buffer, 
	//share projection and view, but have different world-view.
	void setPosition(DirectX::XMMATRIX translationMatrix);
	void renderModel();
	std::vector<MyVertexStruct> modelVertices; //This holds the vertices.
	//struct with vertex positions held by FbxDawg
	void load(const char* fbxFilePath); //<-- Loads the model. Means that modelLoader is called.
	XMMATRIX objectWorldMatrix;
};

GModel::GModel()
{
	this->objectWorldMatrix = XMMatrixIdentity();
}

GModel::~GModel()
{
}


#endif // !GMODEL_H

