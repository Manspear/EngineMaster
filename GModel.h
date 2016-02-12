#ifndef GMODEL_H
#define GMODEL_H
//#include "Engine.h"
#include "FbxDawg.h"
#include <d3d11.h>
#include <DirectXMath.h>

class GModel
{
private:
	
public:
	GModel();
	~GModel();
	ID3D11Buffer* modelVertexBuffer = nullptr;
	ID3D11ShaderResourceView* modelTextureView[2]; //texture and normal map
	//vertex buffer, constant buffer, 
	//share projection and view, but have different world-view.
	//void setPosition(DirectX::XMMATRIX translationMatrix);
	void renderModel();
	std::vector<MyVertexStruct> modelVertices; //This holds the vertices.
	std::wstring modelTextureFilepath; //THis holds the texture's file-path
	//struct with vertex positions held by FbxDawg
	void load(const char* fbxFilePath, ID3D11Device* gDevice); //<-- Loads the model. Means that modelLoader is called.
	//DirectX::XMMATRIX objectWorldMatrix;
};






#endif // !GMODEL_H

