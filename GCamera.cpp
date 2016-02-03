#include "GCamera.h"

//VEIW
void GCamera::initViewMatrix()
{

}

void GCamera::move(XMFLOAT3 direction)
{

}
void GCamera::rotate(XMFLOAT3 axis, float degrees)
{

}
void GCamera::setPosition(XMFLOAT3& newPosition)
{

}
void GCamera::setTarget(XMFLOAT3 nTarget)
{

}

const XMFLOAT3 GCamera::getUp() //returns camera up vector
{

}
const XMFLOAT3 GCamera::getLookAtTarget() //returns camera look at target vector
{

}
const XMMATRIX GCamera::getViewMatrix()
{

}


//PROJECTION
void GCamera::setFrustAngle(float Angle)
{

}
void GCamera::setNearPlane(float nearest)
{

}
void GCamera::setFarPlane(float farthest)
{

}

const XMMATRIX GCamera::getProjMatrix()
{

}

void GCamera::InitProjMatrix(const float angle, const float height, const float width)
{

}

//CON/DESTRUCTOR
GCamera::GCamera()
{
	cPosition = XMFLOAT3(0.0f, 0.0f, -1.0f);
	cTarget = XMFLOAT3(0.0f, 0.0f, 0.0f);


}
GCamera::~GCamera()
{

}