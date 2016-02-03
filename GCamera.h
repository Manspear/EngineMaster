
#include <windows.h>

#include <d3d11.h>

#include <d3dcompiler.h> 

#include <directXMath.h>


#ifndef GCAMERA_H
#define GCAMERA_H

using namespace DirectX;
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

class GCamera
{
public:
	//CON/DESTRUCTOR
	GCamera();
	~GCamera();

	//FUNCTIONS

	//VEIW
	void initViewMatrix();
	void move(XMFLOAT3 direction);
	void rotate(XMFLOAT3 axis, float degrees);
	void setPosition(XMFLOAT3& newPosition);
	const XMFLOAT3& getPosition() const { return cPosition; }
	void setTarget(XMFLOAT3 nTarget);
	const XMFLOAT3& getTarget() const { return cTarget; }
	const XMFLOAT3 getUp(); //returns camera up vector
	const XMFLOAT3 getLookAtTarget(); //returns camera look at target vector
	const XMMATRIX getViewMatrix();

	//PROJECTION
	void setFrustAngle(float Angle);
	const float& getFrustAngle() const { return frustAngle; }
	void setNearPlane(float nearest);
	void setFarPlane(float farthest);

	const XMMATRIX getProjMatrix();

	//VARIABLES
	XMFLOAT3 cPosition;
	XMFLOAT3 cTarget;
	XMFLOAT3 cUp;

	float frustAngle;
	float frustNear;
	float frustFar;

	XMMATRIX cVeiw;
	XMMATRIX cProj;

private:
	void InitProjMatrix(const float angle, const float height, const float width); //initialize the camera projection matrix
	


};


#endif GCAMERA_H