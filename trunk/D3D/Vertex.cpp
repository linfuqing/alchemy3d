#include "StdAfx.h"
#include "Vertex.h"

VERTEX_TEST::VERTEX_TEST(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v, FLOAT nx, FLOAT ny, FLOAT nz) :
D3DXVECTOR3(x, y, z),
uv(u, v),
normal(nx, ny, nz)
{
}

ColorVertex::ColorVertex(FLOAT x, FLOAT y, FLOAT z, D3DCOLOR color)
{
	D3DXVECTOR3::D3DXVECTOR3(x, y, z);

	this->color = color;
}