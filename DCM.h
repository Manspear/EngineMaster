#ifndef DCM_h
#define DCM_h

#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h> 
#include <directXMath.h>

class DCM
{
private:
public:
	DCM();
	~DCM();
	void Dynamic_Cube_Map(ID3D11DeviceContext *gDevice);
};
#endif




