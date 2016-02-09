#ifndef Fbx_h
#define Fbx_h

#include <assert.h>
#include <vector>
#include <fbxsdk.h>

class FbxDawg
{
private:

	//std::vector<Engine::MyVertex>* pOutVertexVector = new std::vector<Engine::MyVertex>;
public:
	FbxDawg();
	~FbxDawg();
	struct MyVertex
	{
		float pos[3];
	};

	void loadModels(std::vector<MyVertex>* pOutVertexVector);
};
#endif




