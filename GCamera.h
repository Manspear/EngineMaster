
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
	GCamera(XMFLOAT4 pos, XMFLOAT4 up, XMFLOAT4 target);
	~GCamera();

	//FUNCTIONS



	//VEIW
	void reset();
	void moveForward(float speed);
	void moveStrafe(float speed);
	void move(XMFLOAT4 direction);
	void rotate(int rotAx, float degrees);
	void setPosition(XMFLOAT4& newPosition);
	const XMFLOAT4 getPosition();
	//jespers Dynamic cube map camera
	void LookAt(XMFLOAT4 pos, XMFLOAT4 target, XMFLOAT4 worldUp);
	void setUp(XMFLOAT4 cUp);
	void setTarget(XMFLOAT4 nTarget);
	const XMVECTOR& getTarget() const { return cTarget; }
	const XMFLOAT4 getUp(); //returns camera up vector
	const XMFLOAT4 getLookAtTarget(); //returns camera look at target vector
	const XMMATRIX getViewMatrix();
	XMFLOAT4 getCameraDirection();


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