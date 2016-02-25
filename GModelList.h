#pragma once
#include "GModel.h"

class GModelList
{
public:
	GModelList();
	~GModelList();
	int numberOfModels;
	

	void initializeModels(ID3D11Device* gDevice, ID3D11DeviceContext * gDeviceContext);
	GModel* getModelList();

private:
	GModel * modelList = nullptr;
	BoundingBox * boundingBoxList = nullptr;
};

