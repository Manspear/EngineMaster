#include "Fbx.h"

FbxDawg::FbxDawg()
{

}

FbxDawg::~FbxDawg()
{
}



//Jesper
void FbxDawg::loadModels(std::vector<MyVertex>* pOutVertexVector)
{
	FbxManager *SDK_Manager = FbxManager::Create();

	FbxIOSettings *ios = FbxIOSettings::Create(SDK_Manager, IOSROOT);

	SDK_Manager->SetIOSettings(ios);

	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);



	FbxScene* Fbx_Scene = FbxScene::Create(SDK_Manager, "");

	FbxImporter * Fbx_Importer = FbxImporter::Create(SDK_Manager, "");
	Fbx_Importer->Initialize("D:\skolan\3D PROGRAMMERING\Programmering\Projektet\itsBoxxy.fbx", -1, SDK_Manager->GetIOSettings());// eller ios istället för SDK_M
	Fbx_Importer->Import(Fbx_Scene);
	Fbx_Importer->Destroy();

	FbxNode* FbxRootNode = Fbx_Scene->GetRootNode();

	if (FbxRootNode)
	{
		for (int i = 0; i < FbxRootNode->GetChildCount(); i++)
		{
			FbxNode* FbxChildNode = FbxRootNode->GetChild(i);

			if (FbxChildNode->GetNodeAttribute() == NULL)
				continue;
			FbxNodeAttribute::EType AttributeType = FbxChildNode->GetNodeAttribute()->GetAttributeType();
			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;
			FbxMesh* mesh = (FbxMesh*)FbxChildNode->GetNodeAttribute();
			FbxVector4* Vertices = mesh->GetControlPoints();

			for (int j = 0; j < mesh->GetPolygonCount(); j++)
			{
				int totalVertices = mesh->GetPolygonSize(j);
				assert(totalVertices == 3);
				for (int k = 0; k < totalVertices; k++)
				{
					int controlPointIndex = mesh->GetPolygonVertex(j, k);//j = polygon, k = vertex, går igenom en vertex i taget för en triangel.
					MyVertex vertex;
					vertex.pos[0] = (float)Vertices[controlPointIndex].mData[0];
					vertex.pos[1] = (float)Vertices[controlPointIndex].mData[1];
					vertex.pos[2] = (float)Vertices[controlPointIndex].mData[2];
					pOutVertexVector->push_back(vertex);
				}
			}
		}
	}
}