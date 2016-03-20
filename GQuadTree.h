#pragma once
#include <DirectXMath.h>
#include "GQuadTreeBoundingBox.h"
class GQuadTree {
private:
public:
	GQuadTree();
	GQuadTree(XMFLOAT3 minPoint, XMFLOAT3 maxPoint);
	~GQuadTree();
	GQuadTreeBoundingBox rootBox;
	void makeTree(int numberOfDivisions, GModelList& modelList);
	//Hmm... How to invoke parentage?
	//You do it by either:
	//1. Do it implicitly (by putting the entire hierarchy in a list, and, when needed, calculate the offset to find children and parents)
	//2. Make it happen with a big list
	//3. Have each object having pointers to their children.
	//We're going with option 3. Each GQuadTreeBoundingBox will hold pointers to their children.
};