#pragma once
#include "GBoundingBox.h"
class GQuadTreeBoundingBox : public GBoundingBox {
private:
public:
	GQuadTreeBoundingBox();
	~GQuadTreeBoundingBox();
	GQuadTreeBoundingBox* GQTBBoxChildren; //This holds pointers to other GQuadTreeBBoxes
	GBoundingBox* objectBBoxChildren; //This holds pointers to the object-bboxes. Only the lowest hierarchy of QuadtreeBboxes bold objectbboxes.
	BBoxPoints QTBVertices;
	void splitBox(int divisionCounter);
	void CreateBBox(XMFLOAT3 minPoint, XMFLOAT3 maxPoint);
	
};