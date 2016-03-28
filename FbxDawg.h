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
	/****/
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
	FbxProperty *DCMmaterial;//Material per object skickar vi. Så jobbigt annars. Eller sök upp bra exempel syntax

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
		int affectedJointIndex;
		double blendingWeight; //Used to blend the animation of two animation layers. An example is transition between walk and run.
	};

	struct sAnimationData {
		std::vector<sBlendingIndexWeightPair*> weightData;
	};

	std::vector<FbxString*> animationName;
	std::vector<long> animationLength;

	std::vector<sAnimationData*> dataPerControlPoint;
	

	unsigned int findJointIndexByName(const char* jointName);

	
	sSkeleton skeleton;
	//std::vector<FbxSkeleton*> skeleton;
	std::vector<MyBSposStruct> blendShapes;
	std::vector<MyVertexStruct> modelVertexList;
	std::vector<FbxVector4 *> bsVert;

};
#endif

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



