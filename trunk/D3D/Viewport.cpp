#include "StdAfx.h"
#include "Viewport.h"
#include "Camera.h"
#include "Scene.h"

Viewport::Viewport(DEVICE* device,Camera* camera, Scene* scene, DWORD fWidth, DWORD fHeight,DWORD fX, DWORD fY):
device(device),
camera(camera),
scene(scene)
{
	this->viewport.X = fX;
	this->viewport.Y = fY;
	this->viewport.Width  = fWidth;
	this->viewport.Height = fHeight;
	this->viewport.MaxZ   = 1.0f;
	this->viewport.MinZ   = 0.0f;
}

Viewport::~Viewport(void)
{
}

D3DXMATRIX& Viewport::getProjectionMatrix()
{
	if(!camera)
		return matrix;

#ifdef RIGHT_HANDED
	D3DXMatrixPerspectiveFovRH(&matrix, camera->fieldOfView, viewport.Width * 1.0f / viewport.Height, camera->zoomNear, camera->zoomFar);
#else
	D3DXMatrixPerspectiveFovLH(&matrix, camera->fieldOfView, viewport.Width * 1.0f / viewport.Height, camera->zoomNear, camera->zoomFar);
#endif

	return matrix;
}

void Viewport::render()
{
	if(!camera || !scene)
		return;

	D3DXMATRIX world, view, projection;

	
	device->GetTransform(D3DTS_WORLD, &world);
	device->GetTransform(D3DTS_VIEW, &view);
	device->GetTransform(D3DTS_PROJECTION, &projection);

	device->SetViewport(&viewport);

	camera->render(this);

	device->SetTransform( D3DTS_PROJECTION, &getProjectionMatrix() );

	device->BeginScene();

	scene->render(this);

	device->EndScene();

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &projection);
}
