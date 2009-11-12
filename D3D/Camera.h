#pragma once
#include "object.h"

class Camera :
	public Object
{
public:
	Camera(FLOAT fZoomNear, FLOAT fZoomFar, FLOAT fFieldOfView = D3DX_PI * .5f);
	~Camera(void);

	virtual void render(Viewport* pViewport);

	float fieldOfView, zoomNear, zoomFar;
	//virtual D3DXMATRIX& getTransform();
};
