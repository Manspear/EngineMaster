#pragma once
#include <vector>
#include "GBoundingBox.h"
#include "GModelList.h" //need listLength too, so that
//I can loop through all bboxes without getting out of range.

//#include "GModel.h"
class GQuadTreeBoundingBox : public GBoundingBox {
private:
public:
	GQuadTreeBoundingBox();
	~GQuadTreeBoundingBox();
	GQuadTreeBoundingBox* GQTBBoxChildren; //This holds pointers to other GQuadTreeBBoxes.
	bool hasSplit = false; //when hasSplit == true, we know that it's got four child-quadtree-bboxes. It is set to true in GQuadTreeBoundingBox::splitBox(int divisionCounter).
	std::vector<GModel*> modelChildren; //With this I have references to the models.
	int modelChildrenCounter = 0;
	//GBoundingBox* objectBBoxChildren; //This holds pointers to the object-bboxes. Only the lowest hierarchy of QuadtreeBboxes bold objectbboxes.
	//int objectBBoxChildCounter = 0;
	
	BBoxPoints vertices; //This is the actual bbox-points.
	
	void splitBox(int divisionCounter, GModelList& modelList);
	void fillBox(GModelList& modelList);
	void CreateBBox(XMFLOAT3 minPoint, XMFLOAT3 maxPoint);
	
};