#include "GQuadTreeBoundingBox.h"

GQuadTreeBoundingBox::GQuadTreeBoundingBox()
{
	GQTBBoxChildren[0] = nullptr;
	GQTBBoxChildren[1] = nullptr;
	GQTBBoxChildren[2] = nullptr;
	GQTBBoxChildren[3] = nullptr;
}

GQuadTreeBoundingBox::~GQuadTreeBoundingBox()
{
	if (GQTBBoxChildren[0] != 0) {
		delete GQTBBoxChildren[0];
		delete GQTBBoxChildren[1];
		delete GQTBBoxChildren[2];
		delete GQTBBoxChildren[3];//this will call the ~GQuadTreeBoundingBox() of the children.
	}
}
void GQuadTreeBoundingBox::CreateBBox(XMFLOAT3 minPoint, XMFLOAT3 maxPoint)
{
	//THis is axis-aligned.
	vertices.BBoxPoint[0] = maxPoint; 
	vertices.BBoxPoint[1] = XMFLOAT3(minPoint.x, maxPoint.y, maxPoint.z); 
	vertices.BBoxPoint[2] = XMFLOAT3(maxPoint.x, maxPoint.y, minPoint.z); 
	vertices.BBoxPoint[3] = XMFLOAT3(minPoint.x, maxPoint.y, minPoint.z); 

	vertices.BBoxPoint[4] = XMFLOAT3(maxPoint.x, minPoint.y, maxPoint.z); 
	vertices.BBoxPoint[5] = XMFLOAT3(minPoint.x, minPoint.y, maxPoint.z);
	vertices.BBoxPoint[6] = XMFLOAT3(maxPoint.x, minPoint.y, minPoint.z); 
	vertices.BBoxPoint[7] = minPoint; //true
}

void GQuadTreeBoundingBox::splitBox(int divisionCounter, GModelList& modelList)
{
	hasSplit = true;

	XMFLOAT3 minValue = vertices.BBoxPoint[7]; 
	XMFLOAT3 maxValue = vertices.BBoxPoint[0];

	//now make four child boxes out of the halfpoints of minValue and maxValue.
	//the y-axis is never divided.
	XMFLOAT3 tempMin = minValue; //to start off, they're the same.
	XMFLOAT3 tempMax = maxValue;
	
	
	//you never divide the y-value... But you need to apply either the min of max y-value,
	//depending on if the point calculated is a min or max value. tempMax.y = maxValue.y. tempMin.y = minValue.y
	
	//Here's a chart of the "min-max-calculation-order"
	//	x.........x...........
	//	.1max     .4max	     .
	//	.         .		     .
	//	.    1min .     4min .
	//	x.........x..........x     1min and 2max only have a differing y-value.  
	//	.3max     . 2max     .
	//	.         .          .
	//	.   3min  .     2min .
	//	..........x..........x

	//GQTBBoxChildren holds pointers to the child-boxes.
	GQTBBoxChildren[0] = new(GQuadTreeBoundingBox);
	GQTBBoxChildren[1] = new(GQuadTreeBoundingBox);
	GQTBBoxChildren[2] = new(GQuadTreeBoundingBox);
	GQTBBoxChildren[3] = new(GQuadTreeBoundingBox);

	float diffX = maxValue.x - minValue.x;
	float diffZ = maxValue.z - minValue.z;

	tempMax = maxValue;
	tempMin.x = maxValue.x - (0.5 * diffX);
	tempMin.z = maxValue.z - (0.5 * diffZ);
	GQTBBoxChildren[0]->CreateBBox(tempMin, tempMax);

	tempMax = tempMin; //tempMin here holds the same values(except for the y) as tempMax will have.
	tempMax.y = maxValue.y;
	tempMin = minValue;
	GQTBBoxChildren[1]->CreateBBox(tempMin, tempMax);

	tempMax.x = maxValue.x;
	tempMax.z = maxValue.z - (0.5 * diffZ);
	tempMin.x = maxValue.x - (0.5 * diffX);
	tempMin.z = minValue.z;
	GQTBBoxChildren[2]->CreateBBox(tempMin, tempMax);

	tempMax.x = maxValue.x - (0.5 * diffX);
	tempMax.z = maxValue.z;
	tempMin.x = minValue.x;
	tempMin.z = maxValue.z - (0.5 * diffZ);
	GQTBBoxChildren[3]->CreateBBox(tempMin, tempMax);

	divisionCounter--;

	if (divisionCounter > 0) {
		//if divisions 
		GQTBBoxChildren[0]->splitBox(divisionCounter, modelList);
		GQTBBoxChildren[1]->splitBox(divisionCounter, modelList);
		GQTBBoxChildren[2]->splitBox(divisionCounter, modelList);
		GQTBBoxChildren[3]->splitBox(divisionCounter, modelList);
	}
	if(divisionCounter == 0)
	{
		//If divisioncounter == 0, I fill THIS box. That is... Wrong. 
		//So if I fill the children I created with models, I'm good 2 go.
		//fillBox(modelList);
		GQTBBoxChildren[0]->fillBox(modelList);
		GQTBBoxChildren[1]->fillBox(modelList);
		GQTBBoxChildren[2]->fillBox(modelList);
		GQTBBoxChildren[3]->fillBox(modelList);
	}
}

void GQuadTreeBoundingBox::fillBox(GModelList & modelList)
{
	//do collision-detection with all the 
	//object-bboxes in the scene.
	//if collision: add that object to the 
	//modelList-object-pointer.

	//adding objects: 
	GModel* tempModelList = modelList.getModelList();
	for (int z = 0; z < modelList.numberOfModels; z++) {
		//Will do a AABB vs AABB intersection-test. It is simple, and cheap, and works for this engine.
		//It is done by comparing the two bboxes' max and min values. If ANY min > max, then they don't intersect.
		XMFLOAT3 modelMax = tempModelList[z].bBox.vertices.BBoxPoint[0]; //the model bbox's MAX-point.
		XMFLOAT3 modelMin = tempModelList[z].bBox.vertices.BBoxPoint[7]; //the model bbox MIN point.

		XMFLOAT3 bboxMax = vertices.BBoxPoint[0];
		XMFLOAT3 bboxMin = vertices.BBoxPoint[7];
		//vertices.BBoxPoint[0]; //QuadTreeBBox maxpoint
		//vertices.BBoxPoint[7]; //QuadTreeBBox minpoint
		if (modelMin.x > bboxMax.x) {
			//non-intersect
			continue;
		}
		if (modelMax.x < bboxMin.x) {
			continue;
		}
		if (modelMin.y > bboxMax.y) {//dumb check? Hmm... No, since models bboxes vary in height.
			//non intersect
			continue;
		}
		if (modelMax.y < bboxMin.y) {
			continue;
		}
		if (modelMin.z > bboxMax.z) {
			//non-intersect
			continue;
		}
		if (modelMax.z < bboxMin.z) {
			continue;
		}
		//Enough tests? Hmm... Should be.

		//if the AABs' have passed the intersection test, it intersects the QuadTreeBBox!
		modelChildren.push_back(&tempModelList[z]); //gives a reference of the GModel to this bbox-object. (a pointer)
	}
	this->modelChildrenCounter = modelChildren.size();
}
