
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
	
	void move(XMFLOAT3 direction);
	void rotate(XMVECTOR axis, float degrees);
	void setPosition(XMVECTOR& newPosition);
	const XMVECTOR& getPosition() const { return cPosition; }
	void setTarget(XMVECTOR nTarget);
	const XMVECTOR& getTarget() const { return cTarget; }
	const XMVECTOR getUp(); //returns camera up vector
	const XMVECTOR getLookAtTarget(); //returns camera look at target vector
	const XMMATRIX getViewMatrix();

	//PROJECTION
	void setFrustAngle(float Angle);
	const float& getFrustAngle() const { return frustAngle; }
	void setNearPlane(float nearest);
	void setFarPlane(float farthest);
	void InitProjMatrix(const float angle, const float height, const float width,
		const float nearest, const float farthest); //initialize the camera projection matrix

	const XMMATRIX getProjMatrix();

	//VARIABLES
	XMVECTOR cPosition;
	XMVECTOR cTarget;
	XMVECTOR cUp;

	float frustAngle;
	float frustNear;
	float frustFar;

	float cHeight;
	float cWidth;

	XMMATRIX cView;
	XMMATRIX cProj;

private:
	void initViewMatrix();

	


};


#endif GCAMERA_H