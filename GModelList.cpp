#include "GModelList.h"

GModelList::GModelList()
{

}

GModelList::~GModelList()
{
	delete[] this->modelList;
}

void GModelList::initializeModels(ID3D11Device* gDevice, ID3D11DeviceContext * gDeviceContext) {
	//Here create the dynamic GModel-Array:
	this->numberOfModels = 4;
	this->modelList = new GModel[this->numberOfModels];

	//this->modelList[0].load(".\\Objects/box2.fbx", gDevice, gDeviceContext); 
	this->modelList[0].load(".\\Objects/itsBoxxyTextured.fbx", gDevice, gDeviceContext, NULL, L"./Images/normal_map.jpg");
	this->modelList[1].load(".\\Objects/box2.fbx", gDevice, gDeviceContext, NULL, L"./Images/normal_map.jpg");
	
	this->modelList[2].load(".\\erect.fbx", gDevice, gDeviceContext, L"./Images/TestPink.jpg", NULL);
	this->modelList[3].load(".\\slak.fbx", gDevice, gDeviceContext, L"./Images/TestPink.jpg", NULL);
	//this->modelList[3].load(".\\itsBoxxy", gDevice);

	modelList[0].setPosition(XMFLOAT4(2, 0, 0, 1), gDeviceContext);
	modelList[1].setPosition(XMFLOAT4(0, 0, 0, 1), gDeviceContext);

	modelList[2].setPosition(XMFLOAT4(-14, 0, 1, 1), gDeviceContext);
	modelList[3].setPosition(XMFLOAT4(-7, 0, 1, 1), gDeviceContext);

}

GModel* GModelList::getModelList() {
	return this->modelList;
}

