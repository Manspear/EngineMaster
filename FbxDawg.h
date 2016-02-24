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

struct MyIndexStruct
{
	int posIndex, norIndex, uvIndex;
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
	//void makeIndexList(std::vector<MyPosition> MyPositionVector, std::vector<MyNormal> MyNormalVector, std::vector<MyUV> MyUVVector);
	int* indexArray = nullptr;

	std::vector<MyVertexStruct> modelVertexList;
	std::vector<MyPosition> indexedPosList;
	std::vector<MyNormal> indexedNormalList;
	std::vector<MyUV> indexedUVList;
	std::vector<MyIndexStruct> myIndexList;
};
#endif




