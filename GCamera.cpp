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
	//cUp = XMVector4Transform(cUp, XMMatrixTranslation(direction.x, direction.y, direction.z));

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
	cUp = (cPosition + FtoV(lookAtUp));

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
	if (XMVector4Equal(FtoV(nTarget), cTarget) || XMVector4Equal(FtoV(nTarget), cPosition))
		return;

	XMFLOAT4 oldTarget = VtoF(cTarget - cPosition);
	XMFLOAT4 newTarget = VtoF(FtoV(nTarget) - cPosition);

	float angle = XMConvertToDegrees(XMVectorGetX(XMVector4AngleBetweenNormals(XMVector4Normalize(FtoV(oldTarget)),
		XMVector4Normalize(FtoV(newTarget)))));

	if (angle != 0.0f && angle != 360.0f && angle != 180.0f)
	{
		XMVECTOR axis = XMVector3Cross(FtoV(oldTarget), FtoV(newTarget));
		rotate(VtoF(axis), angle);
	}
	cTarget = FtoV(nTarget);
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