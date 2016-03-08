#include "GQuadTreeBoundingBox.h"

GQuadTreeBoundingBox::GQuadTreeBoundingBox()
{
}

GQuadTreeBoundingBox::~GQuadTreeBoundingBox()
{
	delete[] GQTBBoxChildren; //this will call the ~GQuadTreeBoundingBox() of the children.
}
void GQuadTreeBoundingBox::CreateBBox(XMFLOAT3 minPoint, XMFLOAT3 maxPoint)
{
	//THis is axis-aligned.
	vertices.BBoxPoint[0] = maxPoint; //false
	vertices.BBoxPoint[1] = XMFLOAT3(minPoint.x, maxPoint.y, maxPoint.z); //false
	vertices.BBoxPoint[2] = XMFLOAT3(maxPoint.x, maxPoint.y, minPoint.z); //true
	vertices.BBoxPoint[3] = XMFLOAT3(minPoint.x, maxPoint.y, minPoint.z); //true

	vertices.BBoxPoint[4] = XMFLOAT3(maxPoint.x, minPoint.y, maxPoint.z); //false
	vertices.BBoxPoint[5] = XMFLOAT3(minPoint.x, minPoint.y, maxPoint.z);//false
	vertices.BBoxPoint[6] = XMFLOAT3(maxPoint.x, minPoint.y, minPoint.z); //true
	vertices.BBoxPoint[7] = minPoint; //true
}

void GQuadTreeBoundingBox::splitBox(int divisionCounter, GModelList& modelList)
{
	hasSplit = true;
	//thanks to the way the vertices in
	//GBoundingBox::CreateBBox(minPoint, maxPoint)
	//are saved, maxPoint is saved at position [0]
	//and minPoint at position [7]
	XMFLOAT3 minValue = vertices.BBoxPoint[0]; 
	XMFLOAT3 maxValue = vertices.BBoxPoint[7];

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
	GQTBBoxChildren = new(GQuadTreeBoundingBox[4]);

	tempMax = maxValue;
	tempMin.x = (maxValue.x / 2.f) + (minValue.x / 2.f);
	tempMin.z = (maxValue.z / 2.f) + (minValue.z / 2.f);
	GQTBBoxChildren[0].CreateBBox(tempMin, tempMax);

	tempMax = tempMin; //tempMin here holds the same values(except for the y) as tempMax will have.
	tempMax.y = maxValue.y;
	tempMin = minValue;
	GQTBBoxChildren[1].CreateBBox(tempMin, tempMax);

	tempMax.x = maxValue.x;
	tempMax.z = (maxValue.z / 2.f) + (minValue.z / 2.f);
	tempMin.x = (maxValue.x / 2.f) + (minValue.x / 2.f);
	tempMin.z = minValue.z;
	GQTBBoxChildren[2].CreateBBox(tempMin, tempMax);

	tempMax.x = (maxValue.x / 2.f) + (minValue.x / 2.f);
	tempMax.z = maxValue.z;
	tempMin.x = minValue.x;
	tempMin.z = (maxValue.z / 2.f) + (minValue.z / 2.f);
	GQTBBoxChildren[3].CreateBBox(tempMin, tempMax);

	divisionCounter--;

	if (divisionCounter > 0) {
		//if divisions 
		GQTBBoxChildren[0].splitBox(divisionCounter, modelList);
		GQTBBoxChildren[1].splitBox(divisionCounter, modelList);
		GQTBBoxChildren[2].splitBox(divisionCounter, modelList);
		GQTBBoxChildren[3].splitBox(divisionCounter, modelList);
	}
	if(divisionCounter == 0)
	{
		fillBox(modelList);
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
		//INSERT COLLISION DETECTION HERE!!!! Between all modelboxes 
		//and this box. Add the 
		//colliding model-boxes to this bbox's modelChildren-struct.
		modelChildren.push_back(&tempModelList[z]); //gives a reference of the GModel to this bbox-object.
	}
	modelChildrenCounter = modelChildren.size();
}
