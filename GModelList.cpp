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
	this->numberOfModels = 2; //VKTIGT
	this->modelList = new GModel[this->numberOfModels];

	this->modelList[0].load(".\\Objects/plane.fbx", gDevice, gDeviceContext, L"./images/Untitled.jpg", NULL);
	//this->modelList[0].load(".\\Objects/box2.fbx", gDevice, gDeviceContext); 
	this->modelList[1].load(".\\Objects/itsBoxxyTextured.fbx", gDevice, gDeviceContext, L"./images/Pokemon.png", L"./Images/normal_map.jpg");
	//this->modelList[1].load(".\\Objects/itsBoxxyTextured.fbx", gDevice, gDeviceContext, L"./images/Chesterfield_texture.jpg", NULL);
	
	//VIKTIGT
	//this->modelList[2].loadBlendShape(".\\Objects/penisblend.fbx", gDevice, gDeviceContext, L"./Images/TestPink.jpg", L"./Images/normal_4.png");
	//this->modelList[3].loadAnimMesh(".\\Objects/manThing.fbx", gDevice, gDeviceContext, L"./images/Chesterfield_texture.jpg", L"./Images/normal_map.jpg");
	
	
	//this->modelList[3].loadAnimMesh(".\\Objects/notCube1Joint.fbx", gDevice, gDeviceContext, L"./images/Chesterfield_texture.jpg", L"./Images/normal_map.jpg");
	//this->modelList[3].loadAnimMesh(".\\Objects/notCube2JointRot.fbx", gDevice, gDeviceContext, L"./images/Chesterfield_texture.jpg", L"./Images/normal_map.jpg");
	//this->modelList[3].loadAnimMesh(".\\Objects/jointBox.fbx", gDevice, gDeviceContext, L"./images/Chesterfield_texture.jpg", L"./Images/normal_map.jpg");
	//this->modelList[3].loadAnimMesh(".\\Objects/boxWith1Joint_7Keys_1stKeyStartAtFrame1_Frames1to30_24fps.fbx", gDevice, gDeviceContext, L"./images/Chesterfield_texture.jpg", L"./Images/normal_map.jpg");
	//this->modelList[3].loadAnimMesh(".\\Objects/twoJointTrans.fbx", gDevice, gDeviceContext, L"./images/Chesterfield_texture.jpg", L"./Images/normal_map.jpg");
	//this->modelList[3].loadAnimMesh(".\\Objects/Cross2.fbx", gDevice, gDeviceContext, L"./images/Chesterfield_texture.jpg", L"./Images/normal_map.jpg");
	//this->modelList[3].load(".\\slak.fbx", gDevice, gDeviceContext, L"./Images/TestPink.jpg", L"./Images/normal_4.png");
	//this->modelList[3].load(".\\itsBoxxy", gDevice);
	
	modelList[0].setPosition(XMFLOAT4(5, -1, 1, 1), gDeviceContext);
	modelList[1].setPosition(XMFLOAT4(1, 0, 1, 1), gDeviceContext);
	//modelList[2].setPosition(XMFLOAT4(-7, 0, 1, 1), gDeviceContext);
	//modelList[3].setPosition(XMFLOAT4(0, 2, 1, 1), gDeviceContext);



}

GModel* GModelList::getModelList() {
	return this->modelList;
}


