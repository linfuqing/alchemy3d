#ifndef __RENDER2_H
#define __RENDER2_H

#include "Vertex.h"
#include "Texture.h"

#define FLAT_TOP_TRIANGLE		0	//平顶三角形
#define FLAT_BOTTOM_TRIANGLE	1	//平底三角形
#define GENERAL_TRIANGLE		2	//一般三角形

void scan_flat()
{
}

void draw_flat_flat_top_triangle( float x0, float x1, float x2, float y0, float y1, float y2, float z0, float z1, float z2, int resX, int resY )
{
	int		xStart, xEnd, yStart, yEnd, yi, xi;
	float	dy, dx, dz;
	float	zStart, zEnd, zi, dzdx;

	dy = 1.0f / (y2 - y0);

	dxdyl = (x2 - x0) * dy;	//左斜边倒数
	dxdyr = (x2 - x1) * dy;	//右斜边倒数

	dzdyr = (z2 - z1) * dy;
	dzdyl = (z2 - z0) * dy;

	if ( y0 > 0 )
	{
		//注意平顶和平底这里的区别
		xStart = (int)ceilf(x0);
		zStart = z0;

		xEnd = (int)ceilf(x1);
		zEnd = z1;

		yStart = (int)ceilf(y0);
	}
	else
	{
		xStart = (int)ceilf(x0 - dxdyl * y0);
		zStart = z0 - dzdyl * y0;

		xEnd = (int)ceilf(x1 - dxdyr * y0);
		zEnd = z1 - dzdyr * y0;

		yStart = 0;
	}

	//z值需要裁剪的情况
	//如果第一个顶点越界，由于平顶，因此第二个顶点也会越界
	if ( z0 < 0 )
	{
		//修正xy的起始点
		yStart = (y2 - y0) * ( - z0 ) / (z2 - z0 ) + y0;
		xStart = dxdyl * yStart + x0;

		//修正z的起始点
		z0 = z1 = 0;

		dzdyr = (z2 - z1) * dy;
		dzdyl = (z2 - z0) * dy;
	}

	yEnd = (int)ceilf(y2);
	yEnd = yEnd > resY ? resY : yEnd;

	//x值需要裁剪的情况
	if ( (x0 < 0) || (x0 > resX) || (x1 < 0) || (x1 > resX) || (x2 < 0) || (x2 > resX) )
	{
		for ( yi = yStart; yi < yEnd; y ++ )
		{
			dx = 1.0f / (xEnd - xStart);

			//z步长
			dzdx = (zEnd - zStart) * dx;

			zi = zStart;

			//x初始值需要裁剪
			if ( xStart < 0 )
			{
				zi -= cxStart * dzdx;

				cxStart = 0;
			}
		}
		}
	}
}

void flat( RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2, int resX, int resY, float * zBuf )
{
	RenderVertex	* tmpV;
	int				nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
	float			x0, y0, x1, y1, x2, y2, z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, currZ,
					dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dzdx, temp2;
	WORD			a, r, g, b;

	//离屏检测
	if (((ver0->y < 0) && (ver1->y < 0) && (ver2->y < 0)) ||
		((ver0->y > resY) && (ver1->y > resY) && (ver2->y > resY)) ||
		((ver0->x < 0) && (ver1->x < 0) && (ver2->x < 0)) ||
		((ver0->x > resX) && (ver1->x > resX) && (ver2->x > resX)) ||
		((ver0->z > 1) && (ver1->z > 1) && (ver2->z > 1)) ||
		((ver0->z < 0 ) && (ver1->z < 0) && (ver2->z < 0)))
	{
		return;
	}

	//判断是否是一条线，如果是，则返回。
	if (((ver0->x == ver1->x) && (ver1->x == ver2->x)) || ((ver0->y == ver1->y) && (ver1->y == ver2->y))) return;

	resX --;
	resY --;

	//调整y0,y1,y2,让它们的y值从小到大
	if (ver1->y < ver0->y)
	{
		tmpV = ver1;	ver1 = ver0;	ver0 = tmpV;
	}

	if (ver2->y < ver0->y)
	{
		tmpV = ver0;	ver0 = ver2;	ver2 = tmpV;
	}

	if (ver2->y < ver1->y)
	{
		tmpV = ver1;	ver1 = ver2;	ver2 = tmpV;
	}

	//判断三角形的类型
	if (ver0->y == ver1->y)
	{
		tri_type = FLAT_TOP_TRIANGLE;

		if (ver1->x < ver0->x)
		{
			tmpV = ver1;
			ver1 = ver0;
			ver0 = tmpV;
		}
	}
	else if (ver1->y == ver2->y)
	{
		tri_type = FLAT_BOTTOM_TRIANGLE;

		if (ver2->x < ver1->x)
		{
			tmpV = ver1;
			ver1 = ver2;
			ver2 = tmpV;
		}
	}
	else
	{
		tri_type = GENERAL_TRIANGLE;
	}

	x0 = ver0->x;
	y0 = ver0->y;
	z0 = ver0->z;

	x1 = ver1->x;
	y1 = ver1->y;
	z1 = ver1->z;

	x2 = ver2->x;
	y2 = ver2->y;
	z2 = ver2->z;

	a = ver0->a;
	r = ver0->r;
	g = ver0->g;
	b = ver0->b;

	//转折后的斜边在哪一侧
	side = 0;

	//转折点y坐标
	ys = y1;

	switch( tri_type )
	{
		case FLAT_TOP_TRIANGLE:
			draw_flat_flat_top_triangle();
			break;

		case FLAT_BOTTOM_TRIANGLE:
			draw_flat_flat_bottom_triangle();
			break;

		case GENERAL_TRIANGLE:
			draw_flat_general_triangle();
			break;
	}
}

void gouraud()
{
}

# endif