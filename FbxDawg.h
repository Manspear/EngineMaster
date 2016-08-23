#ifndef Fbx_h
#define Fbx_h

#include <assert.h>
#include <vector>
#include <fbxsdk.h>
#include <DirectXMath.h>

#pragma region structs

struct MyVertexStruct
{
	float x, y, z, norX, norY, norZ, u, v;
	int controlPointIndex;
};

struct AnimVertexStruct
{
	float x, y, z, norX, norY, norZ, u, v;
	//int controlPointIndex;
	int influences[4];
	float weights[4];
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
	void fillOutSkeleton(unsigned int numberOfDeformers);
	DirectX::XMMATRIX convertFbxMatrixToXMMatrix(FbxAMatrix input);
	void makeLH(DirectX::XMMATRIX* input);
	bool isAnimated = false;

public:
	FbxDawg();
	~FbxDawg();

	std::wstring textureFilepath;//När denna blir tilldelad så får den en kopia istället. Så vi kan utan problem radera den variablen som var med i tilldelningen.
	void loadModels(const char* filePath);
	void getMeshData(FbxMesh* mesh, FbxNode* FbxChildNode);
	void makeIndexList();

	void getJointData(FbxMesh* currMesh, FbxScene* Fbx_Scene);
	void processJointHierarchy(FbxNode* inRootNode);
	void recursiveJointHierarchyTraversal(FbxNode* inNode, int storedIndex, int inNodeParentIndex);
	bool hasSkeleton();
	int* FBXIndexArray = nullptr;
	int sizeOfFBXIndexArray = 0;
	DirectX::XMFLOAT4 pivotValue;
	void bsLoader(FbxMesh * mesh);

	//Core datatypes: FbxSkeleton, eRoot, eLimb, eEffector


	struct sKeyFrame
	{
		DirectX::XMFLOAT4 translation;
		DirectX::XMFLOAT4 rotation;
		DirectX::XMFLOAT4 scale;
		float keyTime;
	};

	struct sAnimLayer
	{
		std::vector<sKeyFrame> keyFrame;
	};

	struct sJoint { //s as in struct :D
		const char* name;
		int parentJointIndex;
		int jointIndex;
		DirectX::XMFLOAT4X4 globalBindPoseInverse;
		std::vector<sAnimLayer> animLayer;
	};

	struct sSkeleton {
		std::vector<sJoint> joints;
		//more things... Hmm
	};

	struct sBlendingIndexWeightPair {
		int jointIndex;
		double blendingWeight; //Used to blend the animation of two animation layers. An example is transition between walk and run.
	};

	struct sAnimWeightData {
		std::vector<sBlendingIndexWeightPair> weightData;
	};

	std::vector<FbxString*> animationName; //One for each stack, i.e one for each joint
	
	std::vector<long> animationLength; //One for each stack, i.e one for each joint

	std::vector<sAnimWeightData> dataPerControlPoint; //One for each Control point.
	

	unsigned int findJointIndexByName(const char* jointName);

	
	sSkeleton skeleton;
	//std::vector<FbxSkeleton*> skeleton;
	std::vector<MyBSposStruct> blendShapes;
	std::vector<MyVertexStruct> modelVertexList;
	std::vector<AnimVertexStruct> animModelVertexList;
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



