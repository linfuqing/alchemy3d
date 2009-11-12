#pragma once

class Camera;
class Scene;

class Viewport
{
private:
	D3DXMATRIX matrix;

public:
	Viewport(DEVICE* device,Camera* camera, Scene* scene, DWORD fWidth, DWORD fHeight,DWORD fX = 0, DWORD fY = 0);
	~Viewport(void);

	DEVICE* device;

	Camera* camera;

	Scene* scene;

	VIEWPORT viewport;

	D3DXMATRIX& getProjectionMatrix();

	void render();
};
