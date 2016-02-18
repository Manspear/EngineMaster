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
	this->numberOfModels = 3;
	this->modelList = new GModel[this->numberOfModels];

	this->modelList[0].load(".\\Objects/ItsBoxxyTextured.fbx", gDevice, gDeviceContext); 
	this->modelList[1].load(".\\Objects/itsBoxxyTextured.fbx", gDevice, gDeviceContext);
	this->modelList[2].load(".\\Objects/ItsSofty.fbx", gDevice, gDeviceContext);
	
	//this->modelList[3].load(".\\itsBoxxy", gDevice);

	modelList[0].setPosition(XMFLOAT4(2, 0, 0, 1), gDeviceContext);
	modelList[1].setPosition(XMFLOAT4(0, 0, 2, 1), gDeviceContext);
	modelList[2].setPosition(XMFLOAT4(-2, 0, 2, 1), gDeviceContext);
}

GModel* GModelList::getModelList() {
	return this->modelList;
}

