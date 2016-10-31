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
};