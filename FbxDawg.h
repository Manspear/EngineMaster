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
	void makeControlPointMap(FbxMesh* currMesh); //Only call once per mesh. Makes a list with length equal to number of control points.

public:
	FbxDawg();
	~FbxDawg();

	std::wstring textureFilepath;//När denna blir tilldelad så får den en kopia istället. Så vi kan utan problem radera den variablen som var med i tilldelningen.
	void loadModels(const char* filePath);
	void makeIndexList();

	void getJointData(FbxNode* rootNode, FbxScene* Fbx_Scene);
	void processJointHierarchy(FbxNode* inRootNode);
	void recursiveJointHierarchyTraversal(FbxNode* inNode, int storedIndex, int inNodeParentIndex);

	int* FBXIndexArray = nullptr;
	int sizeOfFBXIndexArray = 0;
	void bsLoader(FbxMesh * mesh);

	//Core datatypes: FbxSkeleton, eRoot, eLimb, eEffector
	struct sJoint { //s as in struct :D
		const char* name;
		int parentIndex;
		FbxAMatrix* globalBindPoseInverse;
		FbxNode* jointNode;
	};

	struct sSkeleton {
		std::vector<sJoint*> joints;
		//more things... Hmm
	};

	struct sBlendingIndexWeightPair {
		int affectingJointIndex;
		double blendingWeight;
	};

	struct sAnimationData {
		std::vector<sBlendingIndexWeightPair*> weightData;
	};

	std::vector<sAnimationData*> dataPerControlPoint;
	

	unsigned int findJointIndexByName(const char* jointName);

	sSkeleton skeleton;
	//std::vector<FbxSkeleton*> skeleton;
	std::vector<MyBSposStruct> blendShapes;
	std::vector<MyVertexStruct> modelVertexList;
	std::vector<FbxVector4 *> bsVert;

};
#endif




