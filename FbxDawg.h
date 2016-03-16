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
	int sizeOfFBXIndexArray = 0;

	std::vector<MyVertexStruct> modelVertexList;
	
};
#endif




