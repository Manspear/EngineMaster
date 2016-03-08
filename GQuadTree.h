#pragma once
#include <DirectXMath.h>
#include "GQuadTreeBoundingBox.h"
class GQuadTree {
private:
public:
	GQuadTree();
	~GQuadTree();
	GQuadTreeBoundingBox* rootBox;
	void splitTree();
	//Hmm... How to invoke parentage?
};