#include "FbxDawg.h"
#include <iostream>

FbxDawg::FbxDawg()
{

}

FbxDawg::~FbxDawg()
{

}




void FbxDawg::loadModels(const char* filePath)
{
	#pragma region init+ isMesh
	FbxManager *SDK_Manager = FbxManager::Create(); //FbxManager is an object that handles memory management.

	FbxIOSettings *ios = FbxIOSettings::Create(SDK_Manager, IOSROOT); //FbxIOSettings is a collection of import/export options
	SDK_Manager->SetIOSettings(ios);

	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);


	FbxScene* Fbx_Scene = FbxScene::Create(SDK_Manager, "myScene"); //FbxScene contains all the nodes, materials, textures, poses, characters


	FbxImporter * Fbx_Importer = FbxImporter::Create(SDK_Manager, ""); //FbxImporter is to import an FBX file into SDK objects.

	Fbx_Importer->Initialize(filePath, -1, SDK_Manager->GetIOSettings());// eller ios istället för SDK_M
	Fbx_Importer->Import(Fbx_Scene);
	
	

	FbxNode* FBXRootNode = Fbx_Scene->GetRootNode(); // FbxCamera, FbxLight, FbxMesh, etc... elements organized in a hierarchical tree. Root is the mother and by FbxNode::GetChild() we work our way down




	if (FBXRootNode)
	{

		for (int i = 0; i < FBXRootNode->GetChildCount(); i++)//For each and every childnode...
		{


			FbxNode* FbxChildNode = FBXRootNode->GetChild(i);//... initialize the childnode we are at
			
			if (FbxChildNode->GetNodeAttribute() == NULL)//... and then check if its an unset attribute. (special node that we dont want now) (A NULL node attribute is set by calling function FbxNode::SetNodeAttribute() with a NULL pointer)
				continue;//tar nästa child istället
			
			FbxNodeAttribute::EType AttributeType = FbxChildNode->GetNodeAttribute()->GetAttributeType();//... But if its not unset, we check what type the content is, FbxCamera, FbxSkeleton, FbxMesh, etc...
			
			if (AttributeType != FbxNodeAttribute::eMesh)// Only meshes allowed to enter check.
				continue;// Go to next child/mesh

			FbxMesh* mesh = (FbxMesh*)FbxChildNode->GetNodeAttribute();//we are sure that there was a mesh that went through, we get the content of the node.
			#pragma endregion init+ isMesh

			//These three vectors are reset each loop iteration, that way they
			//1. Don't get unneccesarily big and 
			//2. they get looped through correctly in the ">>>ASSEMBLY<<<"-stage. 
			std::vector<MyPosition> MyPositionVector; //containes all vertex positions
			std::vector<MyNormal> MyNormalVector; //contains all normals.
			std::vector<MyUV> MyUVVector; //contain all UVs

			#pragma region >>VERTEX POSITION<<
			
			FbxVector4* Vertices = mesh->GetControlPoints();//... and amongs that contet, lays the vertices. 


			#pragma region OskGetVertex¨
			
			int vertexCount = mesh->GetControlPointsCount();
			


			/*for (int i = 0; i < sizeof(*Vertices) / sizeof(Vertices);i++)
			{
				int lStartIndex = mesh->GetPolygonVertexIndex(i);
				if (lStartIndex == -1) return;

				int* lVertices = &mesh->GetPolygonVertices()[lStartIndex];
				int tri = mesh->GetPolygonSize(i);

				//std::cout << "vertex" << i + 1 << ", ";
				for (int i = 0; i < tri; ++i)
				{
					int vertexID = lVertices[i];
					//std::cout << vertexID << ", ";
				}
				//printf("\n\n");
			}
			//int controlPointIndex = mesh->GetPolygonVertex(t, v);
			#pragma endregion */
			

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
					vertex.vertexIndex = controlPointIndex;
					MyPositionVector.push_back(vertex);
				}
			}
			#pragma endregion >>VERTEX POSITION<<

			#pragma region >>NORMALS<<

			FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();


			if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				FbxVector4 normals;
				int indexByPolygonVertex = 0;
				MyNormal temp;
				for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++) //For every triangle
				{

					int polygonSize = mesh->GetPolygonSize(polygonIndex);
					for (int i = 0; i < polygonSize; i++) //For every vertex in triangle
					{
						int normalIndex = 0; //reference mode is direct, the normal index is the same as indexByPolygonVertex

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


						indexByPolygonVertex++;
					}
				}
				/*for (int lVertexIndex = 0; lVertexIndex < mesh->GetControlPointsCount(); lVertexIndex++)
				{
					int lNormalIndex = 0;
					
					if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lNormalIndex = lVertexIndex;

					
					if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lNormalIndex = normalElement->GetIndexArray().GetAt(lVertexIndex);

					
					FbxVector4 normals = normalElement->GetDirectArray().GetAt(lNormalIndex);
					//FBXSDK_printf("normals for vertex[%d]: %f %f %f %f \n", lVertexIndex, normals[0], normals[1], normals[2], normals[3]);

					

					
					temp.direction[0] = normals[0];
					temp.direction[1] = normals[1];
					temp.direction[2] = normals[2];

					//MyNormalVector.push_back(temp);
					
				}//end for lVertexIndex*/
			}//end eByControlPoint


			else if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				FbxVector4 normals;
				int indexByPolygonVertex = 0;
				MyNormal temp;

				//normals of polygonvertex are stored per polygon. Meaning that even if a cube's got 8 vertices, it'll get one normal per vertex per triangle. So instead of 24 normals, we'll get 36.
				for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++) //For every triangle
				{
					int polygonSize = mesh->GetPolygonSize(polygonIndex);

					for (int i = 0; i < polygonSize; i++) //For every vertex in triangle
					{
						int normalIndex = 0; //reference mode is direct, the normal index is the same as indexByPolygonVertex

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


						indexByPolygonVertex++;
					}
				}
			}


			
			#pragma endregion >> NORMALS <<

			#pragma region >>UV<<


			FbxStringList UVSetNameList;  //Gets all UV sets
			mesh->GetUVSetNames(UVSetNameList);


			for (int UVSetIndex = 0; UVSetIndex < UVSetNameList.GetCount(); UVSetIndex++) //Iterates through the UV sets. Meshes can have multiple textures, every texture got a different UV set
			{

				char* UVSetName = UVSetNameList.GetStringAt(UVSetIndex); //Gets name of the current UV set

				FbxGeometryElementUV* UVElement = mesh->GetElementUV(UVSetName); //Gets the UV-element with the name that UVSetName holds

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
				if (material)//.. if material
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

			#pragma endregion >>UV<<

			#pragma region >>ASSEMBLY OF VERTEXDATA<<
			MyVertexStruct tempVertex;
			MyIndexStruct tempIndex;

			for (int triangleCounter = 0; triangleCounter < MyPositionVector.size(); triangleCounter += 3)
			{

				static int offsets[] = { 1, 0, 2 }; //offset made because directX is left-hand-oriented else the textures and vertices get mirrored.
				for (int i = 0; i < 3; ++i)
				{
					int vertex = triangleCounter + offsets[i];

					tempVertex.x = MyPositionVector[vertex].pos[0];//v x
					tempVertex.y = MyPositionVector[vertex].pos[1];//v y
					tempVertex.z = -MyPositionVector[vertex].pos[2];//v z

					tempVertex.norX = MyNormalVector[vertex].direction[0];//v x
					tempVertex.norY = MyNormalVector[vertex].direction[1];//v y
					tempVertex.norZ = -MyNormalVector[vertex].direction[2];//v 

					tempVertex.u = MyUVVector[vertex].uvCoord[0];// u
					tempVertex.v = 1 - MyUVVector[vertex].uvCoord[1];// v


					this->modelVertexList.push_back(tempVertex);
				}

				for (int i = 0; i < 3; ++i) //get this triangles indices
				{
					int vertex = triangleCounter + offsets[i];

					tempIndex.posIndex = MyPositionVector[vertex].vertexIndex;
					tempIndex.norIndex = MyNormalVector[vertex].normalIndex;
					tempIndex.uvIndex = MyUVVector[vertex].uvIndex;

					//printf("The indices: %d %d %d\n", tempIndex.posIndex, tempIndex.norIndex, tempIndex.uvIndex);

					this->myIndexList.push_back(tempIndex); //vi samlar alla triangelns indices och lägger dem i listan myIndexList
				}
			}
			#pragma endregion >>ASSEMBLY OF VERTEXDATA<<
				



			makeIndexList(MyPositionVector, MyNormalVector, MyUVVector, myIndexList);
		}//for mesh
	}//if fbxnode
	Fbx_Importer->Destroy(); //need be destroyed at the end

} //end of loader



void FbxDawg::makeIndexList(std::vector<MyPosition> MyPositionVector,std::vector<MyNormal> MyNormalVector, std::vector<MyUV> MyUVVector, std::vector<MyIndexStruct> myIndexList)
{
	//>>>>>>>CREATING ORDERED LISTS OF NEEDED VERTICES FOR INDEX-BUFFER IN MAIN<<<<<<<
	
	#pragma region Vertex
	std::vector <MyPosition> unorderedPosList, orderedPosList; 
	bool indexAlreadyExists = false;

	for (int PosVecCount = 0; PosVecCount < MyPositionVector.size(); PosVecCount++) //per position of MyPositionVector/ per vertex.
	{
		for (int unorderedPosCount = 0; unorderedPosCount <unorderedPosList.size(); unorderedPosCount++) //loop through unorderedNorList to check if it already contains the index of MyPositionVector[NormVecCount ]
		{
			if (unorderedPosList[unorderedPosCount].vertexIndex == MyPositionVector[PosVecCount].vertexIndex) //checks if the index is already saved 
			{
				indexAlreadyExists = true;
				continue;
			}
		}
		if (indexAlreadyExists == false)
			unorderedPosList.push_back(MyPositionVector[PosVecCount]); 

		indexAlreadyExists = false; //reset
	}


	

	int searchedIndex = 0;
	
	while (orderedPosList.size() != unorderedPosList.size()) //until all in unorderdered are in ordered.
	{
		for (int ordCount = 0; ordCount < unorderedPosList.size(); ordCount++)
		{
			if (unorderedPosList[ordCount].vertexIndex == searchedIndex)
			{
				orderedPosList.push_back(unorderedPosList[ordCount]);
				searchedIndex++;
				//printf("Pos: %d \n", unorderedPosList[ordCount].vertexIndex);
				continue;
			}
		}
	}
	#pragma endregion Vertex

	#pragma region make normallist
	std::vector <MyNormal> unorderedNorList, orderedNorList; 

	for (int NorVecCount = 0; NorVecCount  < MyNormalVector.size(); NorVecCount ++) //per position of MyPositionVector/ per vertex.
	{	
		for (int unorderedNorCount = 0; unorderedNorCount < unorderedNorList.size(); unorderedNorCount++) //loop through unorderedNorList to check if it already contains the index of MyPositionVector[NormVecCount ]
		{
			if (unorderedNorList[unorderedNorCount].normalIndex == MyNormalVector[NorVecCount].normalIndex) //checks if the index is already saved 
			{
				indexAlreadyExists = true;
				//printf("kill\n");
				continue;
			} 
		}
		if (indexAlreadyExists == false)
			unorderedNorList.push_back(MyNormalVector[NorVecCount]); //if the index isn't present in unorderedNorList, add the vertex.

		indexAlreadyExists = false; //reset
	}
	

	
	
	searchedIndex = 0;
	while (orderedNorList.size() != unorderedNorList.size()) //until all in unorderdered are in ordered.
		{
			for (int ordCount = 0; ordCount < unorderedNorList.size(); ordCount++)
			{
				if (unorderedNorList[ordCount].normalIndex == searchedIndex)
				{
					orderedNorList.push_back(unorderedNorList[ordCount]);
					searchedIndex++;
					//printf("Normal: %d \n", unorderedNorList[ordCount].normalIndex);
					continue;
				}
			}
		}
	
	#pragma endregion MakeNormalList
	
	#pragma region make UVList

	std::vector <MyUV> unorderedUVList, orderedUVList; //Getting all of the vertices with unique indices.
	//bool indexAlreadyExists = false;

	for (int UVecCount = 0; UVecCount < MyUVVector.size(); UVecCount++) //per position of MyPositionVector/ per vertex.
	{
		for (int unorderedUVCount = 0; unorderedUVCount < unorderedUVList.size(); unorderedUVCount++) //loop through unorderedNorList to check if it already contains the index of MyPositionVector[NormVecCount ]
		{
			if (unorderedNorList[unorderedUVCount].normalIndex == MyUVVector[UVecCount].uvIndex) //checks if the index is already saved 
			{
				indexAlreadyExists = true;
				continue;
			}
		}
		if (indexAlreadyExists == false)
			unorderedUVList.push_back(MyUVVector[UVecCount]); //if the index isn't present in unorderedNorList, add the vertex.

		indexAlreadyExists = false; //reset
	}


	//Now that we've isolated the values needed, we need to order them by index.

	searchedIndex = 0;
	while (orderedUVList.size() != unorderedUVList.size()) //until all in unorderdered are in ordered.
	{
		for (int ordCount = 0; ordCount < unorderedUVList.size(); ordCount++)
		{
			if (unorderedUVList[ordCount].uvIndex == searchedIndex)
			{
				orderedUVList.push_back(unorderedUVList[ordCount]);
				searchedIndex++;
				//printf(" UV: %d \n", unorderedUVList[ordCount].uvIndex);
				continue;
			}
		}
	}
	#pragma endregion MakeUVList
	
	//orderedUVList; orderedNorList; orderedPosList;
	
	//this->modelVertexList
	MyVertexStruct tempVertex;
	//printf()
	//printf(": %d", (myIndexList));

	this->modelVertexList.clear();
	for (int i = 0; i < myIndexList.size(); i++)
	{
		//printf("The indices: %d %d %d\n", myIndexList[i].posIndex, myIndexList[i].norIndex, myIndexList[i].uvIndex);
		//printf("%d", orderedPosList[i].pos[0] );
		tempVertex.x = orderedPosList[myIndexList[i].posIndex].pos[0];
		tempVertex.y = orderedPosList[myIndexList[i].posIndex].pos[1];
		tempVertex.z = orderedPosList[myIndexList[i].posIndex].pos[2];

		tempVertex.norX = orderedNorList[myIndexList[i].norIndex].direction[0];
		tempVertex.norY = orderedNorList[myIndexList[i].norIndex].direction[1];
		tempVertex.norZ = orderedNorList[myIndexList[i].norIndex].direction[2];

		tempVertex.u = orderedUVList[myIndexList[i].uvIndex].uvCoord[0];
		tempVertex.u = orderedUVList[myIndexList[i].uvIndex].uvCoord[1];
		printf("pos: %f \n", tempVertex.y);
		this->modelVertexList.push_back(tempVertex);
	}

	//printf("size of model: %d \n", modelVertexList.size());
	//for (int i = 0; i < modelVertexList.size();i++)
	//{
	//	printf("pos: %f \n", modelVertexList[i].u);
	//}

	int* newIndexArray = new int[modelVertexList.size()];
	for (int i = 0; i < modelVertexList.size(); i++)
		newIndexArray[i]=i;

	//now we make the vertices

	
}




