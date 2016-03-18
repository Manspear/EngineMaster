#ifndef Fbx_h
#define Fbx_h

#include <assert.h>
#include <vector>
#include <fbxsdk.h>

#pragma region structs
struct MyPosition
{
	float pos[3];
	int vertexIndex;
};
struct MyNormal
{
	float direction[3];
	int normalIndex;
};
struct MyUV
{
	float uvCoord[2];
	int uvIndex;
};

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
	int* FBXIndexArray = nullptr;
	void bsLoader(FbxMesh * mesh);


	FbxProperty *DCMmaterial;//Material per object skickar vi. Så jobbigt annars. Eller sök upp bra exempel syntax
	std::vector<MyBSposStruct> blendShapes;
	std::vector<MyVertexStruct> modelVertexList;
	std::vector<FbxVector4 *> bsVert;

	//std::vector<MyPosition> indexedPosList;
	//std::vector<MyNormal> indexedNormalList;
	//std::vector<MyUV> indexedUVList;
	//std::vector<MyIndexStruct> myIndexList;
};
#endif




