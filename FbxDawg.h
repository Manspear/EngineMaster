#ifndef Fbx_h
#define Fbx_h

#include <assert.h>
#include <vector>
#include <fbxsdk.h>

#pragma region structs

struct MyVertexStruct
{
	float x, y, z, norX, norY, norZ, u, v;
};

struct MyBSposStruct
{
	float x, y, z;
};

struct MyBSStruct
{
	float x, y, z, norX, norY, norZ, u, v;
	float bsx, bsy, bsz, bsnorX, bsnorY, bsnorZ, bsu, bsv;
};

struct MyIndexStruct
{
	int posIndex, norIndex, uvIndex;
	char* UVSetName;
	FbxGeometryElementUV* UVElement;
};
#pragma endregion structs

class FbxDawg
{
private:

public:
	FbxDawg();
	~FbxDawg();

	std::wstring textureFilepath;//När denna blir tilldelad så får den en kopia istället. Så vi kan utan problem radera den variablen som var med i tilldelningen.
	void loadModels(const char* filePath);
	void makeIndexList();

	void getJointData(FbxNode* rootNode);
	void processJointHierarchy(FbxNode* inRootNode);
	void recursiveJointHierarchyTraversal(FbxNode* inNode, int storedIndex, int inNodeParentIndex);

	int* FBXIndexArray = nullptr;
	int sizeOfFBXIndexArray = 0;
	void bsLoader(FbxMesh * mesh);

	//Core datatypes: FbxSkeleton, eRoot, eLimb, eEffector
	struct sJoint { //s as in struct :D
		const char* name;
		int parentIndex;
	};

	struct sSkeleton {
		std::vector<sJoint*> joints;
		//more things... Hmm
	};

	sSkeleton skeleton;
	//std::vector<FbxSkeleton*> skeleton;
	std::vector<MyBSposStruct> blendShapes;
	std::vector<MyVertexStruct> modelVertexList;
	std::vector<FbxVector4 *> bsVert;

};
#endif




