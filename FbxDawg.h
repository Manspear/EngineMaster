#ifndef Fbx_h
#define Fbx_h

#include <assert.h>
#include <vector>
#include <fbxsdk.h>

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


class FbxDawg
{
private:
public:
	FbxDawg();
	~FbxDawg();

	std::wstring textureFilepath;//N�r denna blir tilldelad s� f�r den en kopia ist�llet. S� vi kan utan problem radera den variablen som var med i tilldelningen.
	void loadModels(const char* filePath);
	std::vector<MyVertexStruct> modelVertexList;
};
#endif




