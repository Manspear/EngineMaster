#pragma once
#include <directXMath.h>
#include <DirectXCollision.h>
#include "GBoundingBox.h"
#include "GQuadTreeBoundingBox.h"
//BoundingFrustum functions
//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.directxmath.boundingfrustum(v=vs.85).aspx

class GFrustum
{
public:
	GFrustum();
	~GFrustum();

	struct Plane {
		DirectX::XMFLOAT3 normal;
		float distance;
	};

	Plane fPlanes[6];
	std::vector<GModel*> seenObjects;

	DirectX::XMFLOAT3* cornerList;

	bool quadTreeCollided(GQuadTreeBoundingBox * modelBox);

	bool hasCollided(GBoundingBox* modelBox);
	void QuadTreeCollision(GQuadTreeBoundingBox* rootBox, bool startCollision); //input from GQuadTree is: rootBox-object.
	void updateFrustumPos(const DirectX::XMMATRIX &cameraProjection, const DirectX::XMMATRIX &cameraView);//will add a "boundingbox-list" as paremeter to updateFrustum. Or make a collision-function that updateFrustum calls.
private:

};

