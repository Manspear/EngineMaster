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
	vertices.BBoxPoint[0] = maxPoint;
	vertices.BBoxPoint[1] = XMFLOAT3(minPoint.x, maxPoint.y, maxPoint.z);
	vertices.BBoxPoint[2] = XMFLOAT3(maxPoint.x, maxPoint.y, minPoint.z);
	vertices.BBoxPoint[3] = XMFLOAT3(minPoint.x, maxPoint.y, minPoint.z);

	vertices.BBoxPoint[4] = XMFLOAT3(maxPoint.x, minPoint.y, maxPoint.z);
	vertices.BBoxPoint[5] = XMFLOAT3(minPoint.x, minPoint.y, maxPoint.z);
	vertices.BBoxPoint[6] = XMFLOAT3(maxPoint.x, minPoint.y, minPoint.z);
	vertices.BBoxPoint[7] = minPoint;
}

void GBoundingBox::setAtMeshPosition(XMFLOAT4X4* modelMatrix)
{
	//GBBoxMatrix = modelMatrix; //note: I may have to redo the boundingbox, if you cannot simply multiply it's vertices with the boundingbox's worldMatrix.
	GBBoxMatrix[0] = XMLoadFloat4x4(modelMatrix);
	for (int i = 0; i < 8; i++) {
		XMVECTOR temp = XMVectorSet(vertices.BBoxPoint[i].x, vertices.BBoxPoint[i].y, vertices.BBoxPoint[i].z, 0); //converts the XMFLOAT3 to XMVECTOR
		XMVector4Transform(temp, GBBoxMatrix[0]); //Transforms every box-corner
		XMStoreFloat3(&vertices.BBoxPoint[i], temp); //Stores the transformed XMVECTOR in a FLOAT3.
	}
}

