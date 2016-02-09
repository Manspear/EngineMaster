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
	//FbxManager is an object that handles memory management.
	FbxManager *SDK_Manager = FbxManager::Create();
	//FbxIOSettings is a collection of import/export options
	FbxIOSettings *ios = FbxIOSettings::Create(SDK_Manager, IOSROOT);
	SDK_Manager->SetIOSettings(ios);
	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	//FbxScene contains all the nodes, materials, textures, poses, characters
	FbxScene* Fbx_Scene = FbxScene::Create(SDK_Manager, "");
	//FbxImporter is to import an FBX file into SDK objects.
	FbxImporter * Fbx_Importer = FbxImporter::Create(SDK_Manager, "");
	Fbx_Importer->Initialize(".\itsBoxxy.fbx", -1, SDK_Manager->GetIOSettings());// eller ios istället för SDK_M
	Fbx_Importer->Import(Fbx_Scene);
	Fbx_Importer->Destroy();
	// FbxCamera, FbxLight, FbxMesh, etc... elements organized in a hierarchical tree. Root is the mother and by FbxNode::GetChild() we work our way down
	FbxNode* FbxRootNode = Fbx_Scene->GetRootNode();


	if(FbxRootNode)
	{
		for (int i = 0; i < FbxRootNode->GetChildCount(); i++)//For each and every childnode...
		{
			FbxNode* FbxChildNode = FbxRootNode->GetChild(i);//... initialize the childnode we are at
			if (FbxChildNode->GetNodeAttribute() == NULL)//... and then check if its an unset attribute. (special node that we dont want now) (A NULL node attribute is set by calling function FbxNode::SetNodeAttribute() with a NULL pointer)
				continue;//tar nästa child istället
			FbxNodeAttribute::EType AttributeType = FbxChildNode->GetNodeAttribute()->GetAttributeType();//... But if its not unset, we check what type the content is, FbxCamera, FbxSkeleton, FbxMesh, etc...
			if (AttributeType != FbxNodeAttribute::eMesh)//... here we do the Only meshes allowed to enter check.
				continue;//tar nästa child istället


			 //>>>>>>>>>>>VERTEX POSITION<<<<<<<<<<<<<
			FbxMesh* mesh = (FbxMesh*)FbxChildNode->GetNodeAttribute();//... now that we are sure that there was a mesh that went through, we get the content of the node.
			FbxVector4* Vertices = mesh->GetControlPoints();//... and amongs that contet, lays the vertices. 

			for (int t = 0; t < mesh->GetPolygonCount(); t++)//...  for every polygon in the polygonCount tex 25 000 tris or 40 000 quads
			{
				int totalVertices = mesh->GetPolygonSize(t);//.... Here we get the amount of vertices in the polygon. A polygon can be a QUAD or a TRI.
				assert(totalVertices == 3);//....	(We ONLY work with TRIS)
				for (int v = 0; v < totalVertices; v++)//... for every vertex in that TRI
				{
					int controlPointIndex = mesh->GetPolygonVertex(t, v);//t = TRI, v = VERTEX, går igenom en vertex i taget för en triangel.
					MyVertex vertex;
					vertex.pos[0] = (float)Vertices[controlPointIndex].mData[0];
					vertex.pos[1] = (float)Vertices[controlPointIndex].mData[1];
					vertex.pos[2] = (float)Vertices[controlPointIndex].mData[2];
					pOutVertexVector->push_back(vertex);
				}
			}

			//>>>>>>>>>>>>>>NORMALS<<<<<<<<<<<<<<<<<<<<<<<<
			FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();






		}//inside this put code yes
	}
}