#include "GCamera.h"

//VEIW
void GCamera::initViewMatrix()
{
	cView = XMMatrixLookAtLH(cPosition, cTarget, cUp);
}

void GCamera::move(XMFLOAT4 direction)
{
	cPosition = XMVector4Transform(cPosition, XMMatrixTranslation(direction.x, direction.y, direction.z));
	cTarget = XMVector4Transform(cTarget, XMMatrixTranslation(direction.x, direction.y, direction.z));
	//cUp = XMVector4Transform(cUp, XMMatrixTranslation(direction.x, direction.y, direction.z)); //uncomment for roll instead of jaw

	this->initViewMatrix();
}

void GCamera::moveForward(float speed) //positive speed is forward, negative is backwards
{
	XMFLOAT4 moveVec = VtoF((XMVector4Normalize(cTarget - cPosition))*speed);

	cPosition = XMVector3Transform(cPosition, XMMatrixTranslation(moveVec.x, moveVec.y, moveVec.z));
	cTarget = XMVector3Transform(cTarget, XMMatrixTranslation(moveVec.x, moveVec.y, moveVec.z));
	//cUp = XMVector4Transform(cUp, XMMatrixTranslation(direction.x, direction.y, direction.z)); //uncomment for roll instead of jaw

	this->initViewMatrix();
}

void GCamera::moveStrafe(float speed) //positive speed is forward, negative is backwards
{
	XMFLOAT4 moveVec = VtoF((XMVector3Cross((XMVector4Normalize(cTarget - cPosition)), cUp)*speed));

	cPosition = XMVector3Transform(cPosition, XMMatrixTranslation(moveVec.x, moveVec.y, moveVec.z));
	cTarget = XMVector3Transform(cTarget, XMMatrixTranslation(moveVec.x, moveVec.y, moveVec.z));
	//cUp = XMVector4Transform(cUp, XMMatrixTranslation(direction.x, direction.y, direction.z)); //uncomment for roll instead of jaw

	this->initViewMatrix();
}

void GCamera::rotate(int rotAx, float degrees)
{
	//degrees = degrees * 0.001;
	XMFLOAT4 axis;

	if (rotAx == 0)
		axis = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (rotAx == 1)
	{
		 axis = XMFLOAT4(VtoF(XMVector3Cross((cTarget - cPosition), cUp)));
	}

	static float degreecheck;
	degreecheck += degrees;
	if (degreecheck < 80 && axis.y != -1.0f);
	{
		if (XMVector4Equal(FtoV(axis), XMVectorZero()) || degrees == 0.0f)
			return;

		XMFLOAT4 lookAtTarget = VtoF(cTarget - cPosition);
		XMFLOAT4 lookAtUp = VtoF(cUp - cPosition);


		lookAtTarget = VtoF(XMVector4Transform(FtoV(lookAtTarget), XMMatrixRotationAxis(FtoV(axis), XMConvertToRadians(degrees))));
		lookAtUp = VtoF(XMVector4Transform(FtoV(lookAtUp), XMMatrixRotationAxis(FtoV(axis), XMConvertToRadians(degrees))));

		cTarget = (cPosition + FtoV(lookAtTarget));
		
		this->initViewMatrix();

	}
}


void GCamera::setPosition(XMFLOAT4& newPosition)
{
	XMFLOAT4 move = VtoF(FtoV(newPosition) - cPosition);
	XMFLOAT4 target = VtoF(cTarget);

	this->move(move);
	//this->setTarget(target);
}

const XMVECTOR GCamera::getUp() //returns camera up vector
{
	return cUp;
}
const XMVECTOR GCamera::getLookAtTarget() //returns camera look at target vector
{
	return cTarget;
}
const XMMATRIX GCamera::getViewMatrix()
{
	return cView;
}


//PROJECTION
void GCamera::setFrustAngle(float Angle)
{
	frustAngle = Angle;
}
void GCamera::setNearPlane(float nearest)
{
	frustNear = nearest;
}
void GCamera::setFarPlane(float farthest)
{
	frustFar = farthest;
}

const XMMATRIX GCamera::getProjMatrix()
{
	return cProj;
}

void GCamera::InitProjMatrix(const float angle, const float height, const float width,
	const float nearest, const float farthest)
{
	frustAngle = angle;
	cWidth = width;
	cHeight = height;
	frustFar = farthest;
	frustNear = nearest;

	cProj = XMMatrixPerspectiveFovLH(frustAngle, cWidth / cHeight, frustNear, frustFar);
}

//CON/DESTRUCTOR
GCamera::GCamera()
{
	cPosition = XMVectorSet(0.0f, 0.0f, -2.0f, 1.0f);
	cTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	cUp = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	this->initViewMatrix();

	frustAngle = 0.0f;
	frustFar = 0.0f;
	frustNear = 0.0f;

	cHeight = 0.0f;
	cWidth = 0.0f;
}

void GCamera::reset()
{
	cPosition = XMVectorSet(0.0f, 0.0f, -2.0f, 1.0f);
	cTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	cUp = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	this->initViewMatrix();
}

XMVECTOR GCamera::FtoV(XMFLOAT4& flo)
{
	return XMLoadFloat4(&flo);
}

XMFLOAT4 GCamera::VtoF(XMVECTOR& vec)
{
	XMFLOAT4 flo;
	XMStoreFloat4(&flo, vec);
	return flo;
}

GCamera::~GCamera()
{

}