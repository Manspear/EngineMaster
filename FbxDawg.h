#ifndef Fbx_h
#define Fbx_h

#include <assert.h>
#include <vector>
#include <fbxsdk.h>

//GLOBALSvvvvvv
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
//GLOBALS^^^^^^^^

class FbxDawg
{
private:
public:
	FbxDawg();
	~FbxDawg();

	void loadModels(const char* filePath);
	std::vector<MyVertexStruct> modelVertexList;
};
#endif




