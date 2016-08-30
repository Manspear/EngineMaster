#include "FbxDawg.h"

//How to organize code for readability:

//
//In h-files:
//- Public before private.
//- Everything only used within the function, should be in private.
//- Have attributes at the top.
//-	Have comments above each function explaining clearly what the parameters and return values are used for.
//	Those comments must be written as /** text **/ and placed above the function. This way the comment
//	will be displayed when hovering pointer over function-name.
//- Organize functions alphabetically, or by frequency of use.
//- Organize attributes by type, alphabetically, or frequency of use.
//- Have the destructors and constructors at the bottom.
//- Come to an agreement on naming standards of functions, booleans, classes, structs, and variables.
//- Come to an agreement on how to write out {}.
//- Generally have short, to-the-point names of functions, variables, and classes.

//In cpp-files:
//- Have the functions appear in the same order as in the header.
//- Have functions be the maximum length of one "screen-height"
//- If some code is within an if-statement: consider making it a function.
//- Have comments describing parts of the code, and what it's used for.

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
		processJointHierarchy(FBXRootNode);

		for (int i = 0; i < FBXRootNode->GetChildCount(); i++)//For each and every childnode...
		{
			FbxNode* FbxChildNode = FBXRootNode->GetChild(i);//... initialize the childnode we are at

			if (FbxChildNode->GetNodeAttribute() == NULL)//... and then check if its an unset attribute. (special node that we dont want now) (A NULL node attribute is set by calling function FbxNode::SetNodeAttribute() with a NULL pointer)
				continue;//tar nästa child istället

			FbxNodeAttribute::EType AttributeType = FbxChildNode->GetNodeAttribute()->GetAttributeType();//... But if its not unset, we check what type the content is, FbxCamera, FbxSkeleton, FbxMesh, etc...

			if (AttributeType != FbxNodeAttribute::eMesh)// Only meshes allowed to enter check.
				continue;// Go to next child/mesh

			FbxMesh* mesh = (FbxMesh*)FbxChildNode->GetNodeAttribute();//we are sure that there was a mesh that went through, we get the content of the node.

			getJointData(mesh, Fbx_Scene); //Here joint data is gathered.

			if (mesh->GetDeformerCount(FbxDeformer::eBlendShape) > 0)
				this->bsLoader(mesh);
			FbxAMatrix pivotMatrix;
			mesh->GetPivot(pivotMatrix);
			FbxVector4 right = pivotMatrix.GetT();
			right[2] *= -1;
			DirectX::XMVECTOR takebro = DirectX::XMVectorSet(right[0], right[1], right[2], right[3]);
			DirectX::XMStoreFloat4(&pivotValue, takebro);

			getMeshData(mesh, FbxChildNode);

		}//for mesh
	}//if fbxnode
	Fbx_Importer->Destroy(); //need be destroyed at the end

} //end of loader

void FbxDawg::getMeshData(FbxMesh* mesh, FbxNode* FbxChildNode)
{
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
	
	MyBSposStruct tempBlendShape;
	MyIndexStruct tempIndex;
	FbxVector4* Vertices;

	Vertices = mesh->GetControlPoints();
	if (mesh->GetDeformerCount(FbxDeformer::eSkin) < 1)
	{
		MyVertexStruct tempVertex;
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

			//This should save the index of the control point. This will be used to get skin-weights 
			//for skeletal animation.
			tempVertex.controlPointIndex = IndexVector[i].posIndex;

			this->modelVertexList.push_back(tempVertex);
		}
	}
	else
	{
		AnimVertexStruct tempVertex;
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

			//tempVertex.controlPointIndex = IndexVector[i].posIndex;

			for (int j = 0; j < 4; j++)
			{
				tempVertex.weights[j] = dataPerControlPoint[IndexVector[i].posIndex].weightData[j].blendingWeight;
				tempVertex.influences[j] = dataPerControlPoint[IndexVector[i].posIndex].weightData[j].jointIndex;
			}
			this->animModelVertexList.push_back(tempVertex);
		}
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
}

void FbxDawg::makeIndexList()
{
	if(isAnimated)
	{ 
		this->FBXIndexArray = new int[this->animModelVertexList.size()];
		this->sizeOfFBXIndexArray = this->animModelVertexList.size();

		for (int i = 0; i < this->sizeOfFBXIndexArray; i++)
			FBXIndexArray[i] = i;

		for (int vertex = 0; vertex< this->animModelVertexList.size(); vertex++)
		{
			for (int other = vertex + 1; other < animModelVertexList.size(); other++)
			{
				if (animModelVertexList[vertex].norX == animModelVertexList[other].norX &&
					animModelVertexList[vertex].norY == animModelVertexList[other].norY &&
					animModelVertexList[vertex].norZ == animModelVertexList[other].norZ
					&&
					animModelVertexList[vertex].u == animModelVertexList[other].u &&
					animModelVertexList[vertex].v == animModelVertexList[other].v
					&&
					animModelVertexList[vertex].x == animModelVertexList[other].x &&
					animModelVertexList[vertex].y == animModelVertexList[other].y &&
					animModelVertexList[vertex].z == animModelVertexList[other].z
					&&
					animModelVertexList[vertex].influences[0] == animModelVertexList[other].influences[0] &&
					animModelVertexList[vertex].influences[1] == animModelVertexList[other].influences[1] &&
					animModelVertexList[vertex].influences[2] == animModelVertexList[other].influences[2] &&
					animModelVertexList[vertex].influences[3] == animModelVertexList[other].influences[3] 
					&&
					animModelVertexList[vertex].weights[0] == animModelVertexList[other].weights[0] &&
					animModelVertexList[vertex].weights[1] == animModelVertexList[other].weights[1] &&
					animModelVertexList[vertex].weights[2] == animModelVertexList[other].weights[2] &&
					animModelVertexList[vertex].weights[3] == animModelVertexList[other].weights[3]
					)
				{
					FBXIndexArray[other] = FBXIndexArray[vertex]; //Remove that index and replace with earlier.
				}
			}//printf("%d\n", FBXIndexArray[vertex]); //Compared with all other.
		}//All vertexes have been compared
	}
	else 
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
}

void FbxDawg::makeControlPointMap(FbxMesh* currMesh)
{
	int aids = currMesh->GetControlPointsCount();

	for (int i = 0; i < aids; ++i) {
		sAnimWeightData iAmHereToFillVector;
		dataPerControlPoint.push_back(iAmHereToFillVector);
	}
}

void FbxDawg::fillOutSkeleton(unsigned int numberOfDeformers) 
{
	for (unsigned int p = 0; p < numberOfDeformers; p++)
	{
		sJoint fillUpSkeleton;
		skeleton.joints.push_back(fillUpSkeleton);
	}
}

DirectX::XMMATRIX FbxDawg::convertFbxMatrixToXMMatrix(FbxAMatrix input)
{
	//THis function assumes row-major matrices.
	float mV[16]; //(00, 01, 02, 03, 10, 11, 12, 13, 20 etc...
	
	unsigned int localCounter = 0;
	for (unsigned int g = 0; g < 4; ++g) {
		for (unsigned int h = 0; h < 4; ++h) {
			mV[localCounter] = input.Get(g, h); //0 1 2 3
			localCounter++;
		}
	}

	DirectX::XMMATRIX output = DirectX::XMMatrixSet(mV[0], mV[1], mV[2], mV[3],
													mV[4], mV[5], mV[6], mV[7], 
													mV[8], mV[9], mV[10],mV[11], 
													mV[12],mV[13],mV[14],mV[15]);
	


	return output;
}

void FbxDawg::makeLH(DirectX::XMMATRIX * input)
{
	//Hope this works as a change of basis from RH to LH...
	//translationTransform[2] *= -1.0;
	//rotationTransform[0] *= -1.0;
	//rotationTransform[1] *= -1.0;
	//scalingTransform[2] *= -1.0;
	DirectX::XMMATRIX newBase = DirectX::XMMatrixSet(1.0, 0.0, 0.0, 0.0,
													 0.0, 1.0, 0.0, 0.0,
													 0.0, 0.0, -1.0, 0.0,
													 0.0, 0.0, 0.0, 1.0);
	*input = DirectX::XMMatrixMultiply(*input, newBase);
	//*input = DirectX::XMMatrixTranspose(*input);
}

void FbxDawg::getJointData(FbxMesh* currMesh, FbxScene* Fbx_Scene)
{
	//Explanation of FBX-terminology regarding Animation-Related things:
	//___________________________________________________________________________________________________________________________
	//Deformers are ways of deforming your mesh. Joints are deformers, but constraints like parent constraints are also deformers.
	//Therefore, one can think of a skeleton as a bunch of deformers of joint-type.
	//Inside each Deformer is a Cluster. Clusters contain "links" to the actual type of deformer. 
	//For a jointDeformer, it's Cluster would contain a link to the actual joint.
	//___________________________________________________________________________________________________________________________

	//FbxMesh* currMesh = rootNode->GetMesh();
	unsigned int deformerCount = currMesh->GetDeformerCount(FbxDeformer::eSkin);

	if (deformerCount > 0) {
		makeControlPointMap(currMesh);
		isAnimated = true;
		//fillOutSkeleton(deformerCount);
	}
	else {
		isAnimated = false;
	}

	

	for (unsigned int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin)
			continue;

		unsigned int numberOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numberOfClusters; ++clusterIndex) 
		{
			FbxCluster* currentCluster = currSkin->GetCluster(clusterIndex);
			//const char* currentJointName = currentCluster->GetName();
			FbxNode* currentJoint = currentCluster->GetLink();
			const char* currentJointName = currentJoint->GetName();
			int currentJointIndex = findJointIndexByName(currentJointName);
			//currentJointIndex = currentJointIndex; //This makes the joint indices properly 0-based.
			
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix GBPIM; //globalBindPoseInverseMatrix
			//FbxAMatrix globalBindposeMatrix;

			//The "parent-chain"-bindpose matrix
			currentCluster->GetTransformMatrix(transformMatrix); //The transform of the MESH at bind-time
			//The matrix local to the joint. Linknodes are joints in this context.
			currentCluster->GetTransformLinkMatrix(transformLinkMatrix); //The transformation of the cluster(joint) in bindtime from localspace to worldspace 
			//Can be used to "nullify" the bindpose-transform in "parent-chain"-space.
			GBPIM = transformLinkMatrix.Inverse() * transformMatrix;
			//globalBindposeMatrix = transformLinkMatrix * transformMatrix;

			DirectX::XMMATRIX bindPoseMatrix;
			//Need to switch from right handed to left handed. Simple change of basis is performed.
			bindPoseMatrix = convertFbxMatrixToXMMatrix(GBPIM);
			//Also, convert the FbxMatrix to a directX matrix.
			//makeLH(&bindPoseMatrix);
			//bindPoseMatrix = DirectX::XMMatrixTranspose(bindPoseMatrix);
			//bindPoseMatrix = bindPoseMatrix * newBase; //Hope this is the right order...
			DirectX::XMFLOAT4X4 finalBindPoseMatrix;
			DirectX::XMStoreFloat4x4(&finalBindPoseMatrix, bindPoseMatrix);
			
			//Update the information in skeleton

			skeleton.joints[currentJointIndex].globalBindPoseInverse = finalBindPoseMatrix;
			currentJoint = currentCluster->GetLink();

			//Associate each joint with the control points it affects
			unsigned int numOfIndices = currentCluster->GetControlPointIndicesCount();

			for (unsigned int i = 0; i < numOfIndices; ++i) 
			{
				//currentCluster->GetControlPointWe
				sBlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.jointIndex = currentJointIndex;
				currBlendingIndexWeightPair.blendingWeight = currentCluster->GetControlPointWeights()[i];
				
				dataPerControlPoint[currentCluster->GetControlPointIndices()[i]].weightData.push_back(currBlendingIndexWeightPair); 
			}

			//Now start getting animation information
			//--> site says "Now only supports one take" in comment here.
			//stack, according to the "comment" in FbxAnimStack describes a stack as a 
			//collection of "animation layers". The Fbx-file can have one or more stacks.
			int count = 0;
			FbxAnimStack* aids;
			int numStacks = Fbx_Scene->GetSrcObjectCount<FbxAnimStack>();
			for (unsigned int stackIndex = 0; stackIndex < numStacks; ++stackIndex) 
			{

				FbxAnimStack* currentAnimStack = Fbx_Scene->GetSrcObject<FbxAnimStack>(stackIndex);


				const unsigned int numAnimLayers = currentAnimStack->GetMemberCount<FbxAnimLayer>();

				for (unsigned int i = 0; i < numAnimLayers; i++)
				{
					FbxAnimLayer* currLayer = currentAnimStack->GetMember<FbxAnimLayer>(i);
					currLayer->Weight = 0;
					currLayer->Mute = false;
					currLayer->Solo = false;
				}

				skeleton.joints[currentJointIndex].animLayer.resize(numAnimLayers);

				for (unsigned int animLayerCounter = 0; animLayerCounter < numAnimLayers; ++animLayerCounter)
				{
					FbxAnimLayer* currAnimLayer = currentAnimStack->GetMember<FbxAnimLayer>(animLayerCounter);
					currAnimLayer->Weight = 100;
					//FbxAnimCurve* curveRotation = skeleton.joints[currentJointIndex]->jointNode->LclTranslation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_TRANSFORM, false);
					//FbxAnimCurve* curveRotation = skeleton.joints[currentJointIndex]->jointNode->LclRotation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_ROTATION, false);


					//get the Translation and Rotation XYZ component curves. Also the scaling curve to "normalize" the scale.
					FbxAnimCurve* translationCurve_X = currentJoint->LclTranslation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);

					FbxAnimEvaluator* animEvaluator = currentJoint->GetAnimationEvaluator();

					FbxString animStackName = currentAnimStack->GetName();
					FbxString temp = animStackName.Buffer();
					animationName.push_back(&temp);

					FbxTakeInfo* takeInfo = Fbx_Scene->GetTakeInfo(animStackName);
					FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
					FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
					animationLength.push_back(end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1);

					//I need to loop through all keyframes, get their respective time-values, and input them into the animEvaluator
					for (long keyIndex = 0; keyIndex < translationCurve_X->KeyGetCount(); ++keyIndex)
					{
						//FbxAnimCurveKey* currKey = &translationCurve_X->KeyGet(keyIndex);
						float asRadians = DirectX::XM_PI / 180;
						FbxAnimCurveKey currKey = translationCurve_X->KeyGet(keyIndex);

						//FbxAMatrix localTransform;
						FbxVector4 translationTransform = animEvaluator->GetNodeLocalTranslation(currentJoint, currKey.GetTime());
						FbxVector4 rotationTransform = animEvaluator->GetNodeLocalRotation(currentJoint, currKey.GetTime());	
						FbxVector4 scalingTransform = animEvaluator->GetNodeLocalScaling(currentJoint, currKey.GetTime());
						//The RotationPivot is gotten outta the eSourcePivot context.
						//FbxVector4 rotationPivot = currentJoint->GetRotationPivot(FbxNode::EPivotSet::eSourcePivot);
				
						//converts the right-handed coordinate system of Maya to the left-handed
						//system of DirectX. 

						translationTransform[2] *= -1.0; 
						rotationTransform[0] *= -1.0;  
						rotationTransform[1] *= -1.0;
						//scalingTransform[2] *= -1.0;

						//localTransform.SetTRS(translationTransform, rotationTransform, scalingTransform);
						//FbxQuaternion quaternionThing = localTransform.GetQ(); <-- Experiment.
						

						DirectX::XMVECTOR translationValues;
						DirectX::XMVECTOR rotationValues;
						DirectX::XMVECTOR scalingValues;

						translationValues = DirectX::XMVectorSet(translationTransform[0], translationTransform[1], translationTransform[2], translationTransform[3]);
						rotationValues = DirectX::XMVectorSet(rotationTransform[0] * asRadians, rotationTransform[1] * asRadians, rotationTransform[2] * asRadians, rotationTransform[3]);
						scalingValues = DirectX::XMVectorSet(scalingTransform[0], scalingTransform[1], scalingTransform[2], scalingTransform[3]);				
						
						DirectX::XMFLOAT4 storeTranslate; 
						DirectX::XMFLOAT4 storeRotate;
						DirectX::XMFLOAT4 storeScale;
						DirectX::XMStoreFloat4(&storeTranslate, translationValues);
						DirectX::XMStoreFloat4(&storeRotate, rotationValues);
						DirectX::XMStoreFloat4(&storeScale, scalingValues);

						FbxTime keyTimeTemp = currKey.GetTime();
						float keyTime = keyTimeTemp.GetSecondDouble();
						sKeyFrame tempKey;
						tempKey.keyTime = keyTime;
						tempKey.translation = storeTranslate;
						tempKey.rotation = storeRotate;

						tempKey.scale = storeScale;
			
						skeleton.joints[currentJointIndex].animLayer[animLayerCounter].keyFrame.push_back(tempKey);
					}
				}
			}	
		}

		for (int i = 0; i < dataPerControlPoint.size(); i++)
		{
			sBlendingIndexWeightPair mordor;
			mordor.blendingWeight = 0;
			mordor.jointIndex = 0;
			while (dataPerControlPoint[i].weightData.size() < 4)
			{
				dataPerControlPoint[i].weightData.push_back(mordor);
			}
		}
		int harvard = 0;
		skeleton;

	}
}

//GRAVEYARD
//THIS IS THE HARD WAY THAT GIVES THE USER MORE CONTROL
//In this for-loop I am trying to figure out how animation curves work...
//
//for (unsigned int animLayerCounter = 0; animLayerCounter < numAnimLayers; ++animLayerCounter)
//{
//	FbxAnimLayer* currAnimLayer = currentAnimStack->GetMember<FbxAnimLayer>();
//	//FbxAnimCurve* curveRotation = skeleton.joints[currentJointIndex]->jointNode->LclTranslation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_TRANSFORM, false);
//	//FbxAnimCurve* curveRotation = skeleton.joints[currentJointIndex]->jointNode->LclRotation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_ROTATION, false);
//
//
//	//get the Translation and Rotation XYZ component curves. Also the scaling curve to "normalize" the scale.
//	FbxAnimCurve* translationCurve_X = skeleton.joints[currentJointIndex]->jointNode->LclTranslation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
//	FbxAnimCurve* translationCurve_Y = skeleton.joints[currentJointIndex]->jointNode->LclTranslation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
//	FbxAnimCurve* translationCurve_Z = skeleton.joints[currentJointIndex]->jointNode->LclTranslation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
//
//	FbxAnimCurve* rotationCurve_X = skeleton.joints[currentJointIndex]->jointNode->LclRotation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
//	FbxAnimCurve* rotationCurve_Y = skeleton.joints[currentJointIndex]->jointNode->LclRotation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
//	FbxAnimCurve* rotationCurve_Z = skeleton.joints[currentJointIndex]->jointNode->LclRotation.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
//
//	FbxAnimCurve* scaleCurve_X = skeleton.joints[currentJointIndex]->jointNode->LclScaling.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
//	FbxAnimCurve* scaleCurve_Y = skeleton.joints[currentJointIndex]->jointNode->LclScaling.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
//	FbxAnimCurve* scaleCurve_Z = skeleton.joints[currentJointIndex]->jointNode->LclScaling.GetCurve(currAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
//	//currAnimCurve->
//	//KeyGet, KeyFind, etc are functions belonging to an animationcurve. Keys are "animation graph keys" that you can find in maya. 
//	//The only thing they contain is a time (x-axis in graph), a value (y-axis in graph), and some info about tangents.
//	//The tangents tells us of how the interpolation behaves between keys.
//
//	//Hmm... But the value held by a key might be relevant, if we've a curve for each individual XYZ curve! HMM!
//	//This is the way to go. Oh my god so EZ!
//}

unsigned int FbxDawg::findJointIndexByName(const char* jointName)
{
	try
	{
		for (unsigned int i = 0; i < skeleton.joints.size(); ++i)
		{
			//* before pointer to get object
			char ass1 = jointName[i];
			char ass2 = skeleton.joints[i].name[i];
			int compareValue = std::strcmp(jointName, skeleton.joints[i].name);
			if (compareValue == 0) { //Apparently no matching name can be found...
				return skeleton.joints[i].jointIndex; //parentIndex + 1 gets the index of this joint.
			}
		}
	}

	catch (const std::exception&)
	{
		printf("Error in FbxDawg::findJointIndexByName(const char* jointName): cannot find matching joint name\n");
	}
}

void FbxDawg::processJointHierarchy(FbxNode * inRootNode)
{
	for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex) {
		FbxNode* currNode = inRootNode->GetChild(childIndex);
		recursiveJointHierarchyTraversal(currNode, 0, -1); 
	}
}

void FbxDawg::recursiveJointHierarchyTraversal(FbxNode * inNode, int currentIndex, int inNodeParentIndex)
{
	//every node will only know about it's direct parent's index. 
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		sJoint currJoint;
		currJoint.parentJointIndex = inNodeParentIndex;
		currJoint.name = inNode->GetName();
		currJoint.jointIndex = currentIndex;
		this->skeleton.joints.push_back(currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); i++) {
		//currentIndex becomes the "old index". And the size of the joint-hierarchy-list "becomes" the currentIndex instead
		//We process each and every child of this node, we search for children of AttributeType eSkeleton to add to the list of joints.
		recursiveJointHierarchyTraversal(inNode->GetChild(i), skeleton.joints.size(), currentIndex); 
	}
	//Erm... Apparently this node ain't got no child
}

bool FbxDawg::hasSkeleton()
{
	return isAnimated;
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




