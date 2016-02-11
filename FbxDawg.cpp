#include "FbxDawg.h"

FbxDawg::FbxDawg()
{

}

FbxDawg::~FbxDawg()
{
}


//Jesper
void FbxDawg::loadModels(const char* filePath)
{
	//FbxManager is an object that handles memory management.
	FbxManager *SDK_Manager = FbxManager::Create();
	//FbxIOSettings is a collection of import/export options
	FbxIOSettings *ios = FbxIOSettings::Create(SDK_Manager, IOSROOT);
	SDK_Manager->SetIOSettings(ios);
	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	//FbxScene contains all the nodes, materials, textures, poses, characters
	FbxScene* Fbx_Scene = FbxScene::Create(SDK_Manager, "myScene");
	//FbxImporter is to import an FBX file into SDK objects.
	FbxImporter * Fbx_Importer = FbxImporter::Create(SDK_Manager, "");
	Fbx_Importer->Initialize(filePath, -1, SDK_Manager->GetIOSettings());// eller ios istället för SDK_M
	Fbx_Importer->Import(Fbx_Scene);
	Fbx_Importer->Destroy();
	// FbxCamera, FbxLight, FbxMesh, etc... elements organized in a hierarchical tree. Root is the mother and by FbxNode::GetChild() we work our way down
	FbxNode* FBXRootNode = Fbx_Scene->GetRootNode();


	//vertexVector, holds complete vertex data. 
	//The data contained in the Nor, Pos, and UV 
	//structs get appended/push_backed into this one.
	//This vector we read in the Engine-class to get
	//models.
	std::vector<MyVertexStruct> MyVertexStructVector;//vertexVector


	//MyVertexStruct
	if(FBXRootNode)
	{
		//int polyIndexCounter = 0; //Think this is a fbx-file-global-thing
		//int indexByPolygonVertex = 0; //Think this is so too
		for (int i = 0; i < FBXRootNode->GetChildCount(); i++)//For each and every childnode...
		{
			//These three vectors are reset each loop iteration, that way they
			//1. Don't get unneccesarily big and 
			//2. they get looped through correctly in the ">>>ASSEMBLY<<<"-stage. 
			std::vector<MyPosition> MyPositionVector;//MyPositionVector
			std::vector<MyNormal> MyNormalVector;//MyNormalVector
			std::vector<MyUV> MyUVVector;//MyUVVector 

			FbxNode* FbxChildNode = FBXRootNode->GetChild(i);//... initialize the childnode we are at
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
					MyPosition vertex;
					vertex.pos[0] = (float)Vertices[controlPointIndex].mData[0];
					vertex.pos[1] = (float)Vertices[controlPointIndex].mData[1];
					vertex.pos[2] = (float)Vertices[controlPointIndex].mData[2];
					MyPositionVector.push_back(vertex);
				}
			}//end vertex

			 //>>>>>>>>>>>>>>NORMALS<<<<<<<<<<<<<<<<<<<<<<<<
			FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();
			if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				FbxVector4 normals;
				int indexByPolygonVertex = 0;
				MyNormal temp;
				//normals of polygonvertex are stored per polygon. Meaning that even if a cube's got 8 vertices, it'll get one normal per vertex per triangle. So instead of 24 normals, we'll get 36. 
				for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++)
				{ //For every triangle
					int polygonSize = mesh->GetPolygonSize(polygonIndex);
					for (int i = 0; i < polygonSize; i++)
					{ //For every vertex in triangle
						int normalIndex = 0;
						//reference mode is direct, the normal index is the same as indexByPolygonVertex
						if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
							normalIndex = indexByPolygonVertex;
						}
						if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
							normalIndex = normalElement->GetIndexArray().GetAt(indexByPolygonVertex);
						}

						normals = normalElement->GetDirectArray().GetAt(normalIndex);

						temp.normalIndex = normalIndex;
						temp.direction[0] = normals[0];
						temp.direction[1] = normals[1];
						temp.direction[2] = normals[2];

						MyNormalVector.push_back(temp);

						//printf("\nNormalz: %f %f %f %f %f %f %f %f %f Number of Vertices: %d", normals[0], normals[1], normals[2], normals[3],normals[4], normals[5], normals[6], normals[7], normals[8], , mesh->GetControlPointsCount());
						//printf("number of normals: %d\n", MyNormalVector.size());
						indexByPolygonVertex++;
					}
				}
			}//end normal

			 //>>>>>>>>>>>>>>UV<<<<<<<<<<<<<<<<<<<<<<<<

			 //Gets all UV sets
			FbxStringList UVSetNameList;
			mesh->GetUVSetNames(UVSetNameList);

			//Iterates through the UV sets. Meshes can have multiple textures, every texture got a different UV set
			for (int UVSetIndex = 0; UVSetIndex < UVSetNameList.GetCount(); UVSetIndex++)
			{
				//Gets name of the current UV set
				char* UVSetName = UVSetNameList.GetStringAt(UVSetIndex);
				//Gets the UV-element with the name that UVSetName holds
				FbxGeometryElementUV* UVElement = mesh->GetElementUV(UVSetName);

				if (!UVElement) {
					continue;
				}
				//Only supports these two modes.
				if (UVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
					UVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
					return;
				bool useIndex = UVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
				int indexCount = (useIndex) ? UVElement->GetIndexArray().GetCount() : 0; //<---- what's happening here??? A questionmark? And the : 0 out of nowhere.

				int polyCount = mesh->GetPolygonCount();

				MyUV temp; //fill this, and then append to MyNormalVector

				if (UVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					for (int polyIndex = 0; polyIndex < polyCount; polyIndex++)
					{
						//build the max index array that is needed
						int polySize = mesh->GetPolygonSize(polyIndex);
						for (int vertIndex = 0; vertIndex < polySize; ++vertIndex)
						{
							FbxVector2 UVValue;
							//get the index of the current current vertex in the vertex array
							int polyVertIndex = mesh->GetPolygonVertex(polyIndex, vertIndex);
							//the UV index depends on the reference mode
							int UVIndex = useIndex ? UVElement->GetIndexArray().GetAt(polyVertIndex) : polyVertIndex;//<----questionmark and : again...

							UVValue = UVElement->GetDirectArray().GetAt(UVIndex);
							temp.uvIndex = UVIndex;
							temp.uvCoord[0] = UVValue[0];
							temp.uvCoord[1] = UVValue[1];
							MyUVVector.push_back(temp);
						}
					}
				}
				else if (UVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					int polyIndexCounter = 0;
					for (int polyIndex = 0; polyIndex < polyCount; ++polyIndex)
					{
						int polySize = mesh->GetPolygonSize(polyIndex);
						for (int vertexIndex = 0; vertexIndex < polySize; ++vertexIndex)
						{
							if (polyIndexCounter < indexCount)
							{
								FbxVector2 UVValue;
								//The UV index depends on the reference-mode
								int UVIndex = useIndex ? UVElement->GetIndexArray().GetAt(polyIndexCounter) : polyIndexCounter;

								UVValue = UVElement->GetDirectArray().GetAt(UVIndex);

								temp.uvIndex = UVIndex;//here i am
								temp.uvCoord[0] = UVValue[0];
								temp.uvCoord[1] = UVValue[1];
								MyUVVector.push_back(temp);

								polyIndexCounter++;
							}
						}
					}
				}

			}//uv
			 //>>>>>>>>>Texture filepath<<<<<<<<<<<<<<<
			int material_Count = mesh->GetSrcObjectCount<FbxSurfaceMaterial>();
			int matCount = FbxChildNode->GetMaterialCount();
			for (int m = 0; m < matCount; m++)//.. for every material attached to the mesh
			{
				FbxSurfaceMaterial* material = FbxChildNode->GetMaterial(m);
				if(material)//.. if material
				{
					FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
					int texture_Count = prop.GetSrcObjectCount<FbxTexture>();
					for (int i = 0; i < texture_Count; i++)// how many texturefiles attached to the material
					{
						const FbxTexture* texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(i));

						wchar_t* wideName;
						FbxUTF8ToWC(((const FbxFileTexture*)texture)->GetFileName(), wideName);

						textureFilepath = wideName;

						FbxFree(wideName);
					}
				}
			}

			 //>>>>>>>>>ASSEMBLY OF VERTEXDATA<<<<<<<<<<<<<<<
			MyVertexStruct tempVertex;
			for (int triangleBase = 0; triangleBase < MyPositionVector.size(); triangleBase += 3)
			{
				static int offsets[] = { 1, 0, 2 };
				for (int i = 0; i < 3; ++i)
				{
					int vertex = triangleBase + offsets[i];

					tempVertex.x = MyPositionVector[vertex].pos[0];//v x
					tempVertex.y = MyPositionVector[vertex].pos[1];//v y
					tempVertex.z = -MyPositionVector[vertex].pos[2];//v z

					tempVertex.norX = MyNormalVector[vertex].direction[0];//v x
					tempVertex.norY = MyNormalVector[vertex].direction[1];//v y
					tempVertex.norZ = -MyNormalVector[vertex].direction[2];//v z

					tempVertex.u = MyUVVector[vertex].uvCoord[0];// u
					tempVertex.v = 1 - MyUVVector[vertex].uvCoord[1];// v

					//tempVertex.vertVar = MyPositionVector[vertex];
					//tempVertex.norVar = MyNormalVector[vertex];
					//tempVertex.uvVar = MyUVVector[vertex];
					this->modelVertexList.push_back(tempVertex);
				}
			}
			//oldLengthOffset += MyPositionVector.size()-1;
			//Now: 2nd time through the loop of the 2nd object it crashes on UV tempVertex.u = MyUVVector[vertex].uvCoord[0];//


			 //Here the class-variable meshVertices 
		}//inside this put code yes
	}

	SDK_Manager->Destroy();
}