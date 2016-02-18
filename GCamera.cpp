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
	XMFLOAT4 moveVec = VtoF((cTarget - cPosition)*speed);

	cPosition = XMVector4Transform(cPosition, XMMatrixTranslation(moveVec.x, moveVec.y, moveVec.z));
	cTarget = XMVector4Transform(cTarget, XMMatrixTranslation(moveVec.x, moveVec.y, moveVec.z));
	//cUp = XMVector4Transform(cUp, XMMatrixTranslation(direction.x, direction.y, direction.z)); //uncomment for roll instead of jaw

	this->initViewMatrix();
}

void GCamera::moveStrafe(float speed) //positive speed is forward, negative is backwards
{
	XMFLOAT4 moveVec = VtoF((XMVector3Cross((cTarget - cPosition), cUp)*speed));

	cPosition = XMVector4Transform(cPosition, XMMatrixTranslation(moveVec.x, moveVec.y, moveVec.z));
	cTarget = XMVector4Transform(cTarget, XMMatrixTranslation(moveVec.x, moveVec.y, moveVec.z));
	//cUp = XMVector4Transform(cUp, XMMatrixTranslation(direction.x, direction.y, direction.z)); //uncomment for roll instead of jaw

	this->initViewMatrix();
}

void GCamera::rotate(XMFLOAT4 axis, float degrees)
{
	if (XMVector4Equal(FtoV(axis), XMVectorZero()) || degrees == 0.0f)
		return;

	XMFLOAT4 lookAtTarget = VtoF(cTarget - cPosition);
	XMFLOAT4 lookAtUp = VtoF(cUp - cPosition);

	lookAtTarget = VtoF(XMVector4Transform(FtoV(lookAtTarget), XMMatrixRotationAxis(FtoV(axis), XMConvertToRadians(degrees))));
	lookAtUp = VtoF(XMVector4Transform(FtoV(lookAtUp), XMMatrixRotationAxis(FtoV(axis), XMConvertToRadians(degrees))));

	cTarget = (cPosition + FtoV(lookAtTarget));
	//cUp = (cPosition + FtoV(lookAtUp)); //uncomment for roll instead of jaw

	this->initViewMatrix();
}
void GCamera::setPosition(XMFLOAT4& newPosition)
{
	XMFLOAT4 move = VtoF(FtoV(newPosition) - cPosition);
	XMFLOAT4 target = VtoF(cTarget);

	this->move(move);
	this->setTarget(target);
}
void GCamera::setTarget(XMFLOAT4 nTarget)
{
	cTarget = FtoV(nTarget);
}

//jespers
void GCamera::LookAt(XMFLOAT4 pos, XMFLOAT4 target, XMFLOAT4 worldUp)
{
	GCamera::setPosition(pos);
	GCamera::setTarget(target);
	GCamera::setUp(worldUp);
}

void GCamera::setUp(XMFLOAT4 cUp)
{
	this-> cUp = FtoV(cUp);
}

const XMFLOAT4 GCamera::getUp() //returns camera up vector
{
	return VtoF(cUp);
}
const XMFLOAT4 GCamera::getLookAtTarget() //returns camera look at target vector
{
	return VtoF(cTarget);
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

GCamera::GCamera(XMFLOAT4 pos, XMFLOAT4 up, XMFLOAT4 target)
{
	cPosition = FtoV(pos);
	cTarget = FtoV(target);
	cUp = FtoV(up);

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