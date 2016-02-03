#include "GCamera.h"

//VEIW
void GCamera::initViewMatrix()
{
	cView = XMMatrixLookAtLH(XMLoadFloat3(&cPosition), XMLoadFloat3(&cTarget), XMLoadFloat3(&cUp));
}

void GCamera::move(XMFLOAT3 direction)
{

}
void GCamera::rotate(XMFLOAT3 axis, float degrees)
{

}
void GCamera::setPosition(XMFLOAT3& newPosition)
{
	cPosition = newPosition;
}
void GCamera::setTarget(XMFLOAT3 nTarget)
{
	cTarget = nTarget;
}

const XMFLOAT3 GCamera::getUp() //returns camera up vector
{
	return cUp;
}
const XMFLOAT3 GCamera::getLookAtTarget() //returns camera look at target vector
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
	frustFar = farthest
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

	cProj = XMMatrixPerspectiveFovLH(frustAngle, cHeight / cWidth, frustNear, frustFar);
}

//CON/DESTRUCTOR
GCamera::GCamera()
{
	cPosition = XMFLOAT3(0.0f, 0.0f, -1.0f);
	cTarget = XMFLOAT3(0.0f, 0.0f, 0.0f);
	cUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

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