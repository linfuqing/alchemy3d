// D3D.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "D3D.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

IDirect3DDevice9 * Device = NULL;

const int Width  = 800;
const int Height = 600;

Viewport* viewport = NULL;
ID3DXMesh* mesh = NULL;


// �˴���ģ���а����ĺ�����ǰ������:
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

 	// TODO: �ڴ˷��ô��롣
	//MSG msg;
	//HACCEL hAccelTable;
	HWND hwnd;
	WPARAM wParam;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_D3D, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if ((hwnd = InitInstance (hInstance, nCmdShow)) == NULL)
	{
		return FALSE;
	}

	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D3D));

	// ����Ϣѭ��:
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

bool Setup()
{
	viewport = new Viewport(Device,new Camera(1.0f, 1000.0f), new Scene(), 200, 200);

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

	m.createMesh(D3DFVF_TEX1, D3DXMESH_MANAGED, Device);

	return true;
}

bool Display(float timeDelta)
{
	if( Device )
	{
		Object* tmd;

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

		Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

		viewport->viewport.X = 300;
		//viewport->viewport.Y = 300;
		viewport->render();

		Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

		viewport->viewport.Y = 300;
		viewport->render();

		Device->Present(0, 0, 0, 0);
	}

	return true;
}


void Cleanup()
{
	Device->Release();
}

//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
		// �����˵�ѡ��:
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
		// TODO: �ڴ���������ͼ����...
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

// �����ڡ������Ϣ�������
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
