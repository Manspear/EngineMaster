#pragma once
#include <DirectXMath.h>
using namespace DirectX;
class GBoundingBox
{

private:
public:
	GBoundingBox();
	~GBoundingBox();

    virtual struct BBoxPoints
	{
		XMFLOAT3 BBoxPoint[8];
	};

	BBoxPoints vertices;

	XMMATRIX* GBBoxMatrix;

	virtual void CreateBBox(XMFLOAT3 minPoint, XMFLOAT3 maxPoint);

	virtual void setAtMeshPosition(XMFLOAT4X4* objectMatrix); //Takes in the model's worldmatrix, and makes it it's own. Multiplies all BBoxPoints with GBBoxMatrix

													  //The frustum class handles collision. Between it and the boundingboxes.
};

