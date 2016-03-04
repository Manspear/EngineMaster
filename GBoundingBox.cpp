#include "GBoundingBox.h"

GBoundingBox::GBoundingBox()
{
	GBBoxMatrix = new XMMATRIX;
	GBBoxMatrix[0] = XMMatrixIdentity();
}

GBoundingBox::~GBoundingBox()
{
	delete GBBoxMatrix;
}

void GBoundingBox::CreateBBox(XMFLOAT3 minPoint, XMFLOAT3 maxPoint)
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

void GBoundingBox::setAtMeshPosition(XMFLOAT4X4* modelMatrix)
{
	GBBoxMatrix[0] = XMLoadFloat4x4(modelMatrix);
	for (int i = 0; i < 8; i++) {
		XMVECTOR temp = XMVectorSet(vertices.BBoxPoint[i].x, vertices.BBoxPoint[i].y, vertices.BBoxPoint[i].z, 1); //converts the XMFLOAT3 to XMVECTOR
		temp = XMVector4Transform(temp, GBBoxMatrix[0]); //Transforms every box-corner. Supposedly.
		XMStoreFloat3(&vertices.BBoxPoint[i], temp); //Stores the transformed XMVECTOR in a FLOAT3.
	}
}

