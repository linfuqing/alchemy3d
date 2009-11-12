// D3D.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "D3D.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

IDirect3DDevice9 * Device = NULL;

const int Width  = 800;
const int Height = 600;

Viewport* viewport = NULL;
ID3DXMesh* mesh = NULL;

 
ID3DXMesh*   Teapot = 0;
D3DMATERIAL9 TeapotMtrl;

IDirect3DVertexBuffer9* BkGndQuad = 0;
IDirect3DTexture9*      BkGndTex  = 0;
D3DMATERIAL9            BkGndMtrl;


// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

bool Setup();
void Cleanup();
bool Display(float timeDelta);


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	//MSG msg;
	//HACCEL hAccelTable;
	HWND hwnd;
	WPARAM wParam;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_D3D, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if ((hwnd = InitInstance (hInstance, nCmdShow)) == NULL)
	{
		return FALSE;
	}

	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D3D));

	// 主消息循环:
	/*while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}*/

	if(!d3d::InitD3D(hwnd, Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		return FALSE;
	}

	if(!Setup())
	{
		return FALSE;
	}

	wParam = d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return (int) wParam;
}

//
// Classes and Structures
//
struct Vertex
{
	Vertex(){}
	Vertex(
		float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x  = x;  _y  = y;  _z  = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u  = u;  _v  = v;
	}
    float _x, _y, _z;
    float _nx, _ny, _nz;
    float _u, _v; // texture coordinates

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

bool Setup()
{
	/*viewport = new Viewport(Device,new Camera(1.0f, 1000.0f), new Scene(), 200, 200);

	//
	// Create vertex and index buffers.
	//

	LPD3DXMESH mesh = NULL;

	D3DXCreateMeshFVF(36, 24, D3DXMESH_MANAGED, VERTEX_TEST::FVF, Device, &mesh);

	//
	// Fill the buffers with the cube data.
	//

	// define unique vertices:
	LPVERTEX_TEST v = NULL;

	mesh->LockVertexBuffer(0, (void**)&v);

	
	// build box

	// fill in the front face vertex data
	v[0] = VERTEX_TEST(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f);
	v[1] = VERTEX_TEST(-1.0f,  1.0f, -1.0f, 0.0f, 1.0f);
	v[2] = VERTEX_TEST( 1.0f,  1.0f, -1.0f, 1.0f, 1.0f);
	v[3] = VERTEX_TEST( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f);

	// fill in the back face vertex data
	v[4] = VERTEX_TEST(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f);
	v[5] = VERTEX_TEST( 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	v[6] = VERTEX_TEST( 1.0f,  1.0f, 1.0f, 1.0f, 1.0f);
	v[7] = VERTEX_TEST(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f);

	// fill in the top face vertex data
	v[8]  = VERTEX_TEST(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f);
	v[9]  = VERTEX_TEST(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f);
	v[10] = VERTEX_TEST( 1.0f, 1.0f,  1.0f, 1.0f, 1.0f);
	v[11] = VERTEX_TEST( 1.0f, 1.0f, -1.0f, 1.0f, 0.0f);

	// fill in the bottom face vertex data
	v[12] = VERTEX_TEST(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f);
	v[13] = VERTEX_TEST( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f);
	v[14] = VERTEX_TEST( 1.0f, -1.0f,  1.0f, 1.0f, 1.0f);
	v[15] = VERTEX_TEST(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f);

	// fill in the left face vertex data
	v[16] = VERTEX_TEST(-1.0f, -1.0f,  1.0f, 0.0f, 0.0f);
	v[17] = VERTEX_TEST(-1.0f,  1.0f,  1.0f, 0.0f, 1.0f);
	v[18] = VERTEX_TEST(-1.0f,  1.0f, -1.0f, 1.0f, 1.0f);
	v[19] = VERTEX_TEST(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f);

	// fill in the right face vertex data
	v[20] = VERTEX_TEST( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f);
	v[21] = VERTEX_TEST( 1.0f,  1.0f, -1.0f, 0.0f, 1.0f);
	v[22] = VERTEX_TEST( 1.0f,  1.0f,  1.0f, 1.0f, 1.0f);
	v[23] = VERTEX_TEST( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f);


	mesh->UnlockVertexBuffer();

	// define the triangles of the cube:
	WORD* i = 0;
	mesh->LockIndexBuffer(0, (void**)&i);

	// fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	mesh->UnlockIndexBuffer();

	D3DXComputeNormals(mesh, NULL);

	
	LPTEXTURE texture = NULL;
	D3DXCreateTextureFromFile(Device, L"desert.bmp", &texture);
	

	Geometry* g = new Geometry(mesh);

	g->getRenderManager().getTextureManagerByStage(0).texture = texture;

	g->getRenderManager().lightEnable = true;
	g->getRenderManager().renderState.state = D3DRS_SHADEMODE;
	g->getRenderManager().renderState.value = D3DSHADE_GOURAUD;
	g->getRenderManager().specularEnable = true;

	//viewport->scene->addChild(g);

	D3DXCreateTeapot(Device, &mesh, NULL);

	g = new Geometry(mesh);

	viewport->scene->addChild(g);

	LPLIGHT light = new LIGHT;

	COLOR color   = WHITE;

	//::ZeroMemory(&light, sizeof(light));

	light->Type      = D3DLIGHT_DIRECTIONAL;
	light->Ambient   = color * .0f;
	light->Diffuse   = color;
	light->Specular  = color * .6f;
	light->Direction = VECTOR3D(1.0f, -0.0f, 2.5f);

	viewport->scene->addLight(light);

	g->getRenderManager().lightEnable = true;
	//g->getRenderManager().getTextureManagerByStage(0).texture = texture;
	g->getRenderManager().renderState.state = D3DRS_SHADEMODE;
	g->getRenderManager().renderState.value = D3DSHADE_GOURAUD;
	//g->getRenderManager().specularEnable = true;

	//Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	//Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Position and aim the camera.
	//
	viewport->camera->position().z = -5.0f;
	viewport->camera->setTarget( new D3DXVECTOR3(0,0,0) );

	//Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	MeshManager m(4, 2);

	m.getVertices()[0] = VERTEX(0, 0, 0);
	m.getVertices()[1] = VERTEX(1, 0, 0);
	m.getVertices()[2] = VERTEX(0, 1, 0);
	m.getVertices()[3] = VERTEX(1, 1, 0);

	m.getTriangles()[0] = TRIANGLE(0, 1, 2);
	m.getTriangles()[1] = TRIANGLE(1, 2, 3);

	//m.getTriangles()[0].uv[1][0] = D3DXVECTOR2(1,1);
	//m.getTriangles()[0].uv[2][0] = D3DXVECTOR2(1,1);

	m.createMesh(D3DFVF_TEX1, D3DXMESH_MANAGED, Device);*/

	// 
	// Init Materials
	//

	TeapotMtrl.Ambient   = RED;
	TeapotMtrl.Emissive  = BLACK;
	TeapotMtrl.Specular  = RED;
	TeapotMtrl.Diffuse   = RED;
	TeapotMtrl.Power     = 2.0f;
	TeapotMtrl.Diffuse.a = 0.5f; // set alpha to 50% opacity

	BkGndMtrl.Ambient   = WHITE;
	BkGndMtrl.Emissive  = BLACK;
	BkGndMtrl.Specular  = WHITE;
	BkGndMtrl.Diffuse   = WHITE;
	BkGndMtrl.Power     = 2.0f;
	//
	// Create the teapot.
	//

	D3DXCreateTeapot(Device, &Teapot, 0);

	//
	// Create the background quad.
	//

	Device->CreateVertexBuffer(
		6 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&BkGndQuad,
		0);

	Vertex* v;
	BkGndQuad->Lock(0, 0, (void**)&v, 0);

	v[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex( 10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex( 10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex( 10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	BkGndQuad->Unlock();

	//
	// Setup a directional light.
	//

	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type      = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse   = WHITE;
	dir.Specular  = WHITE * 0.2f;
	dir.Ambient   = WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);

	Device->SetLight(0, &dir);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Create texture and set texture filters.
	//

	D3DXCreateTextureFromFile(
		Device,
		L"crate.jpg",
		&BkGndTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	//
	// Set alpha blending states.
	//

	// use alpha in material's diffuse component for alpha
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// set blending factors so that alpha component determines transparency
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//
	// Set camera.
	//

	D3DXVECTOR3 pos(0.0f, 0.0f, -3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

bool Display(float timeDelta)
{
	if( Device )
	{
		/*Object* tmd;

		//tmd = viewport->scene->getChildByIndex(1);
		//tmd->rotation().y += .01f;

		tmd = viewport->scene->getChildByIndex(0);
		tmd->rotation().y += .001f;
		tmd->position().x += .001f; 
		//
		// draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

		viewport->viewport.X = 0;
		viewport->viewport.Y = 0;
		viewport->render();

		viewport->viewport.X = 300;
		//viewport->viewport.Y = 300;
		viewport->render();

		viewport->viewport.Y = 300;
		viewport->render();

		Device->Present(0, 0, 0, 0);*/
		//
		// Update
		//

		// increase/decrease alpha via keyboard input
		if( ::GetAsyncKeyState('A') & 0x8000f )
			TeapotMtrl.Diffuse.a += 0.01f;
		if( ::GetAsyncKeyState('S') & 0x8000f )
			TeapotMtrl.Diffuse.a -= 0.01f;

		// force alpha to [0, 1] interval
		if(TeapotMtrl.Diffuse.a > 1.0f)
			TeapotMtrl.Diffuse.a = 1.0f;
		if(TeapotMtrl.Diffuse.a < 0.0f)
			TeapotMtrl.Diffuse.a = 0.0f;

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		// Draw the background
		D3DXMATRIX W;
		D3DXMatrixIdentity(&W);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetFVF(Vertex::FVF);
		Device->SetStreamSource(0, BkGndQuad, 0, sizeof(Vertex));
		Device->SetMaterial(&BkGndMtrl);
		Device->SetTexture(0, BkGndTex);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// Draw the teapot
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

		D3DXMatrixScaling(&W, 1.5f, 1.5f, 1.5f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&TeapotMtrl);
		Device->SetTexture(0, 0);
		Teapot->DrawSubset(0);  

		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}

	return true;
}


void Cleanup()
{
	Device->Release();
}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_D3D);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, Width, Height, NULL, NULL, hInstance, NULL);

   if (hWnd)
   {
		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
   }

   return hWnd;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	//PAINTSTRUCT ps;
	//HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	/*case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;*/
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
