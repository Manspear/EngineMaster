#include "GQuadTree.h"
GQuadTree::GQuadTree() {
	rootBox.CreateBBox(XMFLOAT3(-100.f, -100.f, -100.f), XMFLOAT3(100.f, 100.f, 100.f));
}
GQuadTree::GQuadTree(XMFLOAT3 minPoint, XMFLOAT3 maxPoint) {
	rootBox.CreateBBox(minPoint, maxPoint);
};
GQuadTree::~GQuadTree() {

}

void GQuadTree::makeTree(int numberOfDivisions, GModelList& modelList)
{
	rootBox.splitBox(numberOfDivisions, modelList);
}
