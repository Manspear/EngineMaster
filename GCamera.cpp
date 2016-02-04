#include "GCamera.h"

//VEIW
void GCamera::initViewMatrix()
{
	cView = XMMatrixLookAtLH(cPosition, cTarget, cUp);
}

void GCamera::move(XMFLOAT3 direction)
{
	cPosition = XMVector4Transform(cPosition, XMMatrixTranslation(direction.x, direction.y, direction.z));
	cTarget = XMVector4Transform(cTarget, XMMatrixTranslation(direction.x, direction.y, direction.z));
	cUp = XMVector4Transform(cUp, XMMatrixTranslation(direction.x, direction.y, direction.z));

	this->initViewMatrix();
}
void GCamera::rotate(XMVECTOR axis, float degrees)
{

}
void GCamera::setPosition(XMVECTOR& newPosition)
{
	cPosition = newPosition;
}
void GCamera::setTarget(XMVECTOR nTarget)
{
	cTarget = nTarget;
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
GCamera::~GCamera()
{

}