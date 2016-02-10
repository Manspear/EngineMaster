#ifndef Fbx_h
#define Fbx_h

#include <assert.h>
#include <vector>
#include <fbxsdk.h>

class FbxDawg
{
private:
public:
	FbxDawg();
	~FbxDawg();
	struct MyVertex
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

	struct vertexStruct
	{
		MyVertex vertVar;
		MyNormal norVar;
		MyUV uvVar;
	};
	void loadModels(const char* filePath);
	std::vector<std::vector<vertexStruct>> modelVertexList;
};
#endif




