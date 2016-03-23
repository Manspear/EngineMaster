#include "FbxDawg.h"

FbxDawg::FbxDawg()
{

}

FbxDawg::~FbxDawg()
{
	delete this->FBXIndexArray;

}




void FbxDawg::loadModels(const char* filePath)
{

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


			if (mesh->GetDeformerCount(FbxDeformer::eBlendShape) > 0)
				this->bsLoader(mesh);

			std::vector<MyIndexStruct> IndexVector; IndexVector.resize(mesh->GetPolygonCount() * 3);

			static int offsets[] = { 1, 0, 2 }; //offset made because directX is left-hand-oriented else the textures and vertices get mirrored.

#pragma region >>VERTEX POSITION<<

			for (int t = 0; t < mesh->GetPolygonCount(); t++)//
			{
				int totalVertices = mesh->GetPolygonSize(t);//
				assert(totalVertices == 3);//

				for (int v = 0; v < totalVertices; v++)//
				{
					int controlPointIndex = mesh->GetPolygonVertex(t, v);
					IndexVector[(t * 3) + offsets[v]].posIndex = controlPointIndex; //adding index to a list. To create vertex later.

				}

			}
#pragma endregion >>VERTEX POSITION<<

#pragma region >>NORMALS<<
			FbxVector4 normals;
			FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();
			if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				FbxVector4 normals;
				int indexByPolygonVertex = 0;

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
						IndexVector[polygonIndex * 3 + offsets[i]].norIndex = normalIndex;
						indexByPolygonVertex++;
					}
				}
			}


			else if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				FbxVector4 normals;
				int indexByPolygonVertex = 0;


				for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++) //For every triangle
				{

					int polygonSize = mesh->GetPolygonSize(polygonIndex);

					for (int i = 0; i < polygonSize; i++) //For every vertex in triangle
					{
						int normalIndex = 0;
						//reference mode is direct, the normal index is the same as indexByPolygonVertex
						if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
							normalIndex = indexByPolygonVertex;
						}
						if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
							normalIndex = normalElement->GetIndexArray().GetAt(indexByPolygonVertex);
						}

						normals = normalElement->GetDirectArray().GetAt(normalIndex);
						IndexVector[polygonIndex * 3 + offsets[i]].norIndex = normalIndex;
						indexByPolygonVertex++;
					}

				}
			}
#pragma endregion >> NORMALS <<

#pragma region >>UV<<


			FbxStringList UVSetNameList;  //Gets all UV sets
			mesh->GetUVSetNames(UVSetNameList);
			FbxGeometryElementUV* UVElement;


			for (int UVSetIndex = 0; UVSetIndex < UVSetNameList.GetCount(); UVSetIndex++) //Iterates through the UV sets. Meshes can have multiple textures, every texture got a different UV set
			{

				char* UVSetName = UVSetNameList.GetStringAt(UVSetIndex); //Gets name of the current UV set

				UVElement = mesh->GetElementUV(UVSetName); //Gets the UV-element with the name that UVSetName holds

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

				int polyIndexCounter = 0;
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
							IndexVector[polyIndex*polySize + offsets[vertIndex]].uvIndex = UVIndex;
							IndexVector[polyIndex*polySize + offsets[vertIndex]].UVSetName = UVSetName;
							IndexVector[polyIndex*polySize + offsets[vertIndex]].UVElement = UVElement;
							polyIndexCounter++;

						}
					}
				}

				else if (UVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{

					for (int polyIndex = 0; polyIndex < polyCount; ++polyIndex)
					{
						int polySize = mesh->GetPolygonSize(polyIndex);
						for (int vertIndex = 0; vertIndex < polySize; ++vertIndex)
						{
							if (polyIndexCounter < indexCount)
							{
								FbxVector2 UVValue;
								int UVIndex = useIndex ? UVElement->GetIndexArray().GetAt(polyIndexCounter) : polyIndexCounter;

								UVValue = UVElement->GetDirectArray().GetAt(UVIndex);

								IndexVector[polyIndex*polySize + offsets[vertIndex]].uvIndex = UVIndex;
								IndexVector[polyIndex*polySize + offsets[vertIndex]].UVSetName = UVSetName;
								IndexVector[polyIndex*polySize + offsets[vertIndex]].UVElement = UVElement;
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
			MyBSposStruct tempBlendShape;
			MyIndexStruct tempIndex;
			FbxVector4* Vertices;

			Vertices = mesh->GetControlPoints();

			for (int i = 0; i < IndexVector.size(); i++)
			{
				//printf("pos %d nor %d uv %d\n", IndexVector[i].posIndex, IndexVector[i].norIndex, IndexVector[i].uvIndex);
				normals = normalElement->GetDirectArray().GetAt(IndexVector[i].norIndex);
				tempVertex.norX = normals[0];
				tempVertex.norY = normals[1];
				tempVertex.norZ = (-1)*(normals[2]);

				tempVertex.x = (float)Vertices[IndexVector[i].posIndex].mData[0];
				tempVertex.y = (float)Vertices[IndexVector[i].posIndex].mData[1];
				tempVertex.z = -1 * ((float)Vertices[IndexVector[i].posIndex].mData[2]);


				FbxVector2 UVValue = IndexVector[i].UVElement->GetDirectArray().GetAt(IndexVector[i].uvIndex);
				tempVertex.u = UVValue.mData[0];
				tempVertex.v = 1 - UVValue.mData[1];

				this->modelVertexList.push_back(tempVertex);
			}

			for (int j = 0; j < bsVert.size(); j++)
				Vertices = bsVert[j];
			for (int i = 0; i < IndexVector.size(); i++)
			{

				//normals = normalElement->GetDirectArray().GetAt(IndexVector[i].norIndex);
				//tempVertex.norX = normals[0];
				//tempVertex.norY = normals[1];
				//tempVertex.norZ = (-1)*(normals[2]);

				tempBlendShape.x = (float)Vertices[IndexVector[i].posIndex].mData[0];
				tempBlendShape.y = (float)Vertices[IndexVector[i].posIndex].mData[1];
				tempBlendShape.z = -1 * ((float)Vertices[IndexVector[i].posIndex].mData[2]);

				//FbxVector2 UVValue = IndexVector[i].UVElement->GetDirectArray().GetAt(IndexVector[i].uvIndex);
				//tempVertex.u = UVValue.mData[0];
				//tempVertex.v = 1 - UVValue.mData[1];

				this->blendShapes.push_back(tempBlendShape);
			}

			this->makeIndexList();


#pragma endregion >>ASSEMBLY OF VERTEXDATA<<





		}//for mesh
	}//if fbxnode
	Fbx_Importer->Destroy(); //need be destroyed at the end

} //end of loader

void FbxDawg::makeIndexList()
{
	this->FBXIndexArray = new int[this->modelVertexList.size()];
	this->sizeOfFBXIndexArray = this->modelVertexList.size();

	for (int i = 0; i < this->sizeOfFBXIndexArray; i++)
		FBXIndexArray[i] = i;

	for (int vertex = 0; vertex< this->modelVertexList.size(); vertex++)
	{
		for (int other = vertex + 1; other < modelVertexList.size(); other++)
		{
			if (modelVertexList[vertex].norX == modelVertexList[other].norX &&
				modelVertexList[vertex].norY == modelVertexList[other].norY &&
				modelVertexList[vertex].norZ == modelVertexList[other].norZ
				&&

				modelVertexList[vertex].u == modelVertexList[other].u &&
				modelVertexList[vertex].v == modelVertexList[other].v
				&&

				modelVertexList[vertex].x == modelVertexList[other].x &&
				modelVertexList[vertex].y == modelVertexList[other].y &&
				modelVertexList[vertex].z == modelVertexList[other].z)
			{
				FBXIndexArray[other] = FBXIndexArray[vertex]; //Remove that index and replace with earlier.
			}


		}//printf("%d\n", FBXIndexArray[vertex]); //Compared with all other.



	}//All vertexes have been compared


}



void FbxDawg::getJointData(FbxNode* rootNode)
{
	//Explanation of FBX-terminology regarding Animation-Related things:
	//___________________________________________________________________________________________________________________________
	//Deformers are ways of deforming your mesh. Joints are deformers, but constraints like parent constraints are also deformers.
	//Therefore, one can think of a skeleton as a bunch of deformers of joint-type.
	//Inside each Deformer is a Cluster. Clusters contain "links" to the actual type of deformer. 
	//For a jointDeformer, it's Cluster would contain a link to the actual joint.
	//___________________________________________________________________________________________________________________________

}

void FbxDawg::processJointHierarchy(FbxNode * inRootNode)
{
	//for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex) {
	//	FbxNode* currNode = inRootNode->GetChild(childIndex);
	//	recursiveJointHierarchyTraversal(currNode, 0, -1); 
	//}
}

void FbxDawg::recursiveJointHierarchyTraversal(FbxNode * inNode, int currentIndex, int inNodeParentIndex)
{
	//every node will only know about it's direct parent's index. 
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		sJoint currJoint;
		currJoint.parentIndex = inNodeParentIndex;
		currJoint.name = inNode->GetName();
		this->skeleton.joints.push_back(&currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); i++) {
		//currentIndex becomes the "old index". And the size of the joint-hierarchy-list "becomes" the currentIndex instead
		//We process each and every child of this node, we search for children of AttributeType eSkeleton to add to the list of joints.
		recursiveJointHierarchyTraversal(inNode->GetChild(i), skeleton.joints.size(), currentIndex); 
	}
}

void FbxDawg::bsLoader(FbxMesh * mesh)
{
	int bShapeCount = mesh->GetDeformerCount(FbxDeformer::eBlendShape);

	if (bShapeCount > 0)
	{
		printf("model has %d BlendShapes\n", bShapeCount);
		for (int bsIndex = 0; bsIndex < bShapeCount; bsIndex++)
		{
			FbxBlendShape* lBlendShape = (FbxBlendShape*)mesh->GetDeformer(bsIndex, FbxDeformer::eBlendShape);

			int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
			printf("bShape has %d BlendShape Channels\n", lBlendShapeChannelCount);

			for (int lChannelIndex = 0; lChannelIndex < lBlendShapeChannelCount; lChannelIndex++)
			{
				FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);

				int lShapesCount = lChannel->GetTargetShapeCount();

				for (int lShapeIndex = 0; lShapeIndex < lShapesCount; lShapeIndex++)
				{
					FbxShape * lShape = lChannel->GetTargetShape(lShapeIndex);
					bsVert.push_back(lShape->GetControlPoints());
				}
			}
		}
	}
}


