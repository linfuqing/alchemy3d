#pragma once

typedef struct VERTEX_TEST : public D3DXVECTOR3
{
	VERTEX_TEST(FLOAT x = 0, FLOAT y = 0, FLOAT z = 0, FLOAT u = 0, FLOAT v = 0, FLOAT nx = 0, FLOAT ny = 0, FLOAT nz = 0);

	VECTOR3D normal;

	VECTOR2D uv;

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL;
}VERTEX_TEST, * LPVERTEX_TEST;

struct NormalVertex : public D3DXVECTOR3
{
	NormalVertex(FLOAT x = 0, FLOAT y = 0, FLOAT z = 0, FLOAT nx = 0, FLOAT ny = 0, FLOAT nz = 0 );

	D3DXVECTOR3 normal;

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
};

struct ColorVertex : public D3DXVECTOR3
{
	D3DCOLOR color;

	ColorVertex(FLOAT x = 0, FLOAT y = 0, FLOAT z = 0, D3DCOLOR color = 0);

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

