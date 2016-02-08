
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
	
	void move(XMFLOAT4 direction);
	void rotate(XMFLOAT4 axis, float degrees);
	void setPosition(XMFLOAT4& newPosition);
	const XMVECTOR& getPosition() const { return cPosition; }
	void setTarget(XMFLOAT4 nTarget);
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
	XMVECTOR FtoV(XMFLOAT4& flo);
	XMFLOAT4 VtoF(XMVECTOR& vec);
	


};


#endif GCAMERA_H