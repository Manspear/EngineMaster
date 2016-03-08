#include "GQuadTreeBoundingBox.h"

GQuadTreeBoundingBox::GQuadTreeBoundingBox()
{
}

GQuadTreeBoundingBox::~GQuadTreeBoundingBox()
{
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

void GQuadTreeBoundingBox::splitBox()
{
	//thanks to the way the vertices in
	//GBoundingBox::CreateBBox(minPoint, maxPoint)
	//are saved, maxPoint is saved at position [0]
	//and minPoint at position [7]
	XMFLOAT3 minValue = QTBVertices.BBoxPoint[0]; 
	XMFLOAT3 maxValue = QTBVertices.BBoxPoint[7];

	//now make four child boxes out of the halfpoints of minValue and maxValue.
	//the y-axis is never divided.
	XMFLOAT3 tempMin;
	XMFLOAT3 tempMax;
	objectBBoxChildren = new(GQuadTreeBoundingBox[4]);
	tempMax = maxValue;
	tempMin.x = (maxValue.x / 2) + (minValue.x / 2);
	tempMin.y = tempMin.y;
	tempMin.z = (maxValue.z / 2) + (minValue.z / 2);
	objectBBoxChildren[0].CreateBBox(tempMin, tempMax);

	//continue here
}
