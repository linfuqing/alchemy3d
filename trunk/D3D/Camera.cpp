#include "StdAfx.h"
#include "Camera.h"

Camera::Camera(FLOAT fZoomNear, FLOAT fZoomFar, FLOAT fFieldOfView):
zoomNear(fZoomNear),
zoomFar(fZoomFar),
fieldOfView(fFieldOfView)
{
}

Camera::~Camera(void)
{
}

void Camera::render(Viewport* pViewport)
{
	pViewport->device->SetTransform( D3DTS_VIEW, &getTransform() );
}