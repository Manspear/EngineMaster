#include "GQuadTreeBoundingBox.h"

GQuadTreeBoundingBox::GQuadTreeBoundingBox()
{
}

GQuadTreeBoundingBox::~GQuadTreeBoundingBox()
{
	delete[] objectBBoxChildren; //this will call the ~GQuadTreeBoundingBox() of the children.
}
void GQuadTreeBoundingBox::CreateBBox(XMFLOAT3 minPoint, XMFLOAT3 maxPoint)
{
	//THis is axis-aligned.
	QTBVertices.BBoxPoint[0] = maxPoint; //false
	QTBVertices.BBoxPoint[1] = XMFLOAT3(minPoint.x, maxPoint.y, maxPoint.z); //false
	QTBVertices.BBoxPoint[2] = XMFLOAT3(maxPoint.x, maxPoint.y, minPoint.z); //true
	QTBVertices.BBoxPoint[3] = XMFLOAT3(minPoint.x, maxPoint.y, minPoint.z); //true

	QTBVertices.BBoxPoint[4] = XMFLOAT3(maxPoint.x, minPoint.y, maxPoint.z); //false
	QTBVertices.BBoxPoint[5] = XMFLOAT3(minPoint.x, minPoint.y, maxPoint.z);//false
	QTBVertices.BBoxPoint[6] = XMFLOAT3(maxPoint.x, minPoint.y, minPoint.z); //true
	QTBVertices.BBoxPoint[7] = minPoint; //true
}

void GQuadTreeBoundingBox::splitBox(int divisionCounter)
{
	//thanks to the way the vertices in
	//GBoundingBox::CreateBBox(minPoint, maxPoint)
	//are saved, maxPoint is saved at position [0]
	//and minPoint at position [7]
	XMFLOAT3 minValue = QTBVertices.BBoxPoint[0]; 
	XMFLOAT3 maxValue = QTBVertices.BBoxPoint[7];

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

	//objectBBoxChildren holds pointers to the child-boxes.
	objectBBoxChildren = new(GQuadTreeBoundingBox[4]);

	tempMax = maxValue;
	tempMin.x = (maxValue.x / 2.f) + (minValue.x / 2.f);
	tempMin.z = (maxValue.z / 2.f) + (minValue.z / 2.f);
	objectBBoxChildren[0].CreateBBox(tempMin, tempMax);

	tempMax = tempMin; //tempMin here holds the same values(except for the y) as tempMax will have.
	tempMax.y = maxValue.y;
	tempMin = minValue;
	objectBBoxChildren[1].CreateBBox(tempMin, tempMax);

	tempMax.x = maxValue.x;
	tempMax.z = (maxValue.z / 2.f) + (minValue.z / 2.f);
	tempMin.x = (maxValue.x / 2.f) + (minValue.x / 2.f);
	tempMin.z = minValue.z;
	objectBBoxChildren[2].CreateBBox(tempMin, tempMax);

	tempMax.x = (maxValue.x / 2.f) + (minValue.x / 2.f);
	tempMax.z = maxValue.z;
	tempMin.x = minValue.x;
	tempMin.z = (maxValue.z / 2.f) + (minValue.z / 2.f);
	objectBBoxChildren[3].CreateBBox(tempMin, tempMax);
}
