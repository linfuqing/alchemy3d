#ifndef __RENDER_H
#define __RENDER_H

#include "Viewport.h"
#include "Vertex.h"
#include "Texture.h"

#define FLAT_TOP_TRIANGLE		0	//平顶三角形
#define FLAT_BOTTOM_TRIANGLE	1	//平底三角形
#define GENERAL_TRIANGLE		2	//一般三角形

INLINE DWORD computePixelColor( float r, float g, float b, float a, float u, float v, Texture * texture )
{
	WORD * ARGBBuffer = texture->ARGBBuffer;
	int pos = ((int)(u * (texture->width - 1) + 0.5) + ( (int)(v * (texture->height - 1) + 0.5) * texture->width )) << 2;

	a *= ARGBBuffer[pos];
	r *= ARGBBuffer[++pos];
	g *= ARGBBuffer[++pos];
	b *= ARGBBuffer[++pos];

	return ((int)a << 24) + ((int)r << 16) + ((int)g << 8) + (int)b;
}

INLINE DWORD getPixelFromTexture( float u, float v, Texture * texture)
{
	WORD * ARGBBuffer = texture->ARGBBuffer;

	int pos = ((int)(u * (texture->width - 1) + 0.5) + ( (int)(v * (texture->height - 1) + 0.5) * texture->width )) << 2;

	return ((int)ARGBBuffer[pos] << 24) + ((int)ARGBBuffer[++pos] << 16) + ((int)ARGBBuffer[++pos] << 8) + (int)ARGBBuffer[++pos];
}

INLINE DWORD computePixelColor2( float u, float v, Texture * texture )
{
	WORD * ARGBBuffer = texture->ARGBBuffer;
	int pos;

	pos = ((int)(u * (texture->width - 1) + 0.5) + ( (int)(v * (texture->height - 1) + 0.5) * texture->width )) << 2;

	return ((int)ARGBBuffer[pos] << 24) + ((int)ARGBBuffer[++pos] << 16) + ((int)ARGBBuffer[++pos] << 8) + (int)ARGBBuffer[++pos];
	return 0;
}

void triangle_rasterize( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2 )
{
	RenderVertex	* tmpV;
	DWORD			* colorBuffer = view->colorBuffer, color;
	float			* zBuffer = view->zBuffer;

	int				resX = (int)view->width, resY = (int)view->height;
	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
	float			z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, currZ, dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dzdx, temp2;


	//背面剔除
	if ( BACKFACE_CULLING_MODE == 2)
	{
		if ( ( ver2->x - ver0->x ) * ( ver1->y - ver2->y ) > ( ver2->y - ver0->y ) * ( ver1->x - ver2->x ) )
			return;
	}

	if (((ver0->y < 0) && (ver1->y < 0) && (ver2->y < 0)) ||
		((ver0->y > resY) && (ver1->y > resY) && (ver2->y > resY)) ||
		((ver0->x < 0) && (ver1->x < 0) && (ver2->x < 0)) ||
		((ver0->x > resX) && (ver1->x > resX) && (ver2->x > resX)))
	{
		return;
	}

	//判断是否是一条线，如果是，则返回。
	if (((ver0->x == ver1->x) && (ver1->x == ver2->x)) || ((ver0->y == ver1->y) && (ver1->y == ver2->y)))
	{
		return;
	}

	//*************************************** start draw ************************************
	nw = resX - 1;
	nh = resY - 1;

	//调整y0,y1,y2,让它们的y值从小到大
	if (ver1->y < ver0->y)
	{
		tmpV = ver1;
		ver1 = ver0;
		ver0 = tmpV;
	}
	if (ver2->y < ver0->y)
	{
		tmpV = ver0;
		ver0 = ver2;
		ver2 = tmpV;
	}
	if (ver2->y < ver1->y)
	{
		tmpV = ver1;
		ver1 = ver2;
		ver2 = tmpV;
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

	x0 = (int)ver0->x;
	y0 = (int)ver0->y;
	z0 = ver0->z;

	x1 = (int)ver1->x;
	y1 = (int)ver1->y;
	z1 = ver1->z;

	x2 = (int)ver2->x;
	y2 = (int)ver2->y;
	z2 = ver2->z;

	color = ((int)ver0->a << 24) + ((int)ver0->r << 16) + ((int)ver0->g << 8) + (int)ver0->b;

	side = 0;

	//转折后的斜边在哪一侧
	ys = y1;

	//转折点y坐标
	if (tri_type == FLAT_TOP_TRIANGLE || tri_type == FLAT_BOTTOM_TRIANGLE)
	{
		if (tri_type == FLAT_TOP_TRIANGLE)
		{
			dy = 1.0f / (y2 - y0);

			dxdyl = (x2 - x0) * dy;	//左斜边倒数
			dzdyl = (z2 - z0) * dy;

			dxdyr = (x2 - x1) * dy;	//右斜边倒数
			dzdyr = (z2 - z1) * dy;

			//y0小于视窗顶部y坐标，调整左斜边和右斜边当前值,y值开始值
			if (y0 < 0)
			{
				dy = - (float)y0;

				xStart = dxdyl * dy + x0;
				zStart = dzdyl * dy + z0;

				xEnd = dxdyr * dy + x1;
				zEnd = dzdyr * dy + z1;

				yStart = 0;
			}
			else
			{
				//注意平顶和平底这里的区别
				xStart = (float)x0;
				zStart = z0;

				xEnd = (float)x1;
				zEnd = z1;

				yStart = (float)y0;
			}
		}
		else
		{
			//平底三角形
			dy = 1.0f / (y1 - y0);

			dxdyl = (x1 - x0) * dy;
			dzdyl = (z1 - z0) * dy;

			dxdyr = (x2 - x0) * dy;
			dzdyr = (z2 - z0) * dy;

			if (y0 < 0)
			{
				dy = - (float)y0;

				xStart = dxdyl * dy + x0;
				zStart = dzdyl * dy + z0;

				xEnd = dxdyr * dy + x0;
				zEnd = dzdyr * dy + z0;

				yStart = 0.0f;
			}
			else
			{
				xStart = (float)x0;
				zStart = z0;

				xEnd = (float)x0;
				zEnd = z0;

				yStart = (float)y0;
			}
		}
		//y2>视窗高度时，大于rect.height部分就不用画出了
		cyStart = (int)yStart;
		cyEnd = (int)( y2 > nh ? nh : y2 );
		ypos = cyStart * resX;

		//x值需要裁剪的情况
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)xStart;
				cxEnd = xEnd > nw ? nw : (int)xEnd;

				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
				{
					if (cxStart == cxEnd)
					{
						pos = cxStart + ypos;

						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							zBuffer[pos] = zStart;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;

						currZ = zStart;

						//初始值需要裁剪
						if (cxStart < 0)
						{
							currZ -= cxStart * dzdx;

							cxStart = 0;
						}
						//绘制扫描线
						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = color;
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							zBuffer[pos] = zEnd;
						}
					}
				}

				//y每增加1时,xl和xr分别加上他们的递增量
				xStart += dxdyl;
				zStart += dzdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;

				ypos += resX;
			}
		}
		else
		{
			//x不需要裁剪的情况
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if ( zStart > 0 || zEnd > 0 )
				{
					cxStart = (int)xStart;
					cxEnd = (int)xEnd;

					currZ = zStart;

					if (cxStart == cxEnd)
					{
						pos = cxStart + ypos;
						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							zBuffer[pos] = currZ;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = color;
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;

				ypos += resX;
			}
		}
	}
	else
	{
		//普通三角形
		yEnd = (float)(y2 > nh ? nh : y2);

		if (y1 < 0)
		{
			//由于y0<y1,这时相当于平顶三角形
			//计算左右斜边的斜率的倒数
			dyl = 1.0f / (y2 - y1);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x2 - x1) * dyl;
			dzdyl = (z2 - z1) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;

			//计算左右斜边初始值
			dyr = - (float)y0;
			dyl = - (float)y1;

			xStart = dxdyl * dyl + x1;
			zStart = dzdyl * dyl + z1;

			xEnd = dxdyr * dyr + x0;
			zEnd = dzdyr * dyr + z0;

			yStart = 0;

			if (dxdyr > dxdyl)
			{
				//交换斜边
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;

				side = 1;
			}
		}
		else if (y0 < 0)
		{
			dyl = 1.0f / (y1 - y0);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dxdyr = (x2 - x0) * dyr;

			dzdyl = (z1 - z0) * dyl;

			dzdyr = (z2 - z0) * dyr;

			dy = - (float)y0;

			xStart = dxdyl * dy + x0;
			zStart = dzdyl * dy + z0;

			xEnd = dxdyr * dy + x0;
			zEnd = dzdyr * dy + z0;

			yStart = 0;

			if (dxdyr < dxdyl)
			{
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;

				side = 1;
			}
		}
		else
		{
			//y值都大于0
			dyl = 1.0f / (y1 - y0);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dzdyl = (z1 - z0) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;

			xStart = (float)x0;
			zStart = z0;

			xEnd = (float)x0;
			zEnd = z0;

			yStart = (float)y0;

			if (dxdyr < dxdyl)
			{
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				side = 1;
			}
		}

		cyStart = (int)yStart;
		cyEnd = (int)yEnd;
		ypos = cyStart * resX;

		//x需要裁剪
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)xStart;
				cxEnd = xEnd > nw ? nw :(int)xEnd;

				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
				{
					if ( cxStart == cxEnd )
					{
						pos = cxStart + ypos;

						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							zBuffer[pos] = zStart;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;

						currZ = zStart;

						//初始值需要裁剪
						if (cxStart < 0)
						{
							currZ -= cxStart * dzdx;

							cxStart = 0;
						}

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = color;
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;

				ypos += resX;

				//转折点
				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dzdyl = (z2 - z1) * dyl;

						xStart = (float)x1;
						zStart = z1;
					}
					else
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dzdyr = (z1 - z2) * dyr;

						xEnd = (float)x2;
						zEnd = z2;
					}
				}
			}
		}
		else
		{
			//不需要裁剪
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if ( zStart > 0 || zEnd > 0 )
				{
					cxStart = (int)xStart;
					cxEnd = (int)xEnd;

					currZ = zStart;

					if (xStart == xEnd)
					{
						pos = cxStart + ypos;
						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							zBuffer[pos] = currZ;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = color;
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;

				ypos += resX;

				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dzdyl = (z2 - z1) * dyl;

						xStart = x1 + dxdyl;
						zStart = z1 + dzdyl;
					}
					else
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dzdyr = (z1 - z2) * dyr;

						xEnd = x2 + dxdyr;
						zEnd = z2 + dzdyr;
					}
				}
			}
		}
	}
}

void triangle_rasterize_texture( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2, Texture * texture )
{
	RenderVertex	* tmpV;
	DWORD			* colorBuffer = view->colorBuffer;
	DWORD			* textureBuffer = view->textureBuffer;
	float			* zBuffer = view->zBuffer;
	DWORD			color;

	int				resX = (int)view->width, resY = (int)view->height;
	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
	float			u0, v0, u1, v1, u2, v2, z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, uStart, uEnd, vStart, vEnd, currZ,
					currU, currV, dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dudyl, dudyr, dvdyl, dvdyr, dzdx, dudx, dvdx, temp2;


	//背面剔除
	if ( BACKFACE_CULLING_MODE == 2)
	{
		if ( ( ver2->x - ver0->x ) * ( ver1->y - ver2->y ) > ( ver2->y - ver0->y ) * ( ver1->x - ver2->x ) )
			return;
	}

	if (((ver0->y < 0) && (ver1->y < 0) && (ver2->y < 0)) ||
		((ver0->y > resY) && (ver1->y > resY) && (ver2->y > resY)) ||
		((ver0->x < 0) && (ver1->x < 0) && (ver2->x < 0)) ||
		((ver0->x > resX) && (ver1->x > resX) && (ver2->x > resX)))
	{
		return;
	}

	//判断是否是一条线，如果是，则返回。
	if (((ver0->x == ver1->x) && (ver1->x == ver2->x)) || ((ver0->y == ver1->y) && (ver1->y == ver2->y)))
	{
		return;
	}

	//*************************************** start draw ************************************
	nw = resX - 1;
	nh = resY - 1;

	//调整y0,y1,y2,让它们的y值从小到大
	if (ver1->y < ver0->y)
	{
		tmpV = ver1;
		ver1 = ver0;
		ver0 = tmpV;
	}
	if (ver2->y < ver0->y)
	{
		tmpV = ver0;
		ver0 = ver2;
		ver2 = tmpV;
	}
	if (ver2->y < ver1->y)
	{
		tmpV = ver1;
		ver1 = ver2;
		ver2 = tmpV;
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

	x0 = (int)ver0->x;
	y0 = (int)ver0->y;
	z0 = ver0->z;

	x1 = (int)ver1->x;
	y1 = (int)ver1->y;
	z1 = ver1->z;

	x2 = (int)ver2->x;
	y2 = (int)ver2->y;
	z2 = ver2->z;

	u0 = ver0->u;
	v0 = ver0->v;
	u1 = ver1->u;
	v1 = ver1->v;
	u2 = ver2->u;
	v2 = ver2->v;

	color = ((int)ver0->a << 24) + ((int)ver0->r << 16) + ((int)ver0->g << 8) + (int)ver0->b;

	side = 0;

	//转折后的斜边在哪一侧
	ys = y1;

	//转折点y坐标
	if (tri_type == FLAT_TOP_TRIANGLE || tri_type == FLAT_BOTTOM_TRIANGLE)
	{
		if (tri_type == FLAT_TOP_TRIANGLE)
		{
			dy = 1.0f / (y2 - y0);

			dxdyl = (x2 - x0) * dy;	//左斜边倒数
			dzdyl = (z2 - z0) * dy;
			dudyl = (u2 - u0) * dy;
			dvdyl = (v2 - v0) * dy;

			dxdyr = (x2 - x1) * dy;	//右斜边倒数
			dzdyr = (z2 - z1) * dy;
			dudyr = (u2 - u1) * dy;
			dvdyr = (v2 - v1) * dy;

			//y0小于视窗顶部y坐标，调整左斜边和右斜边当前值,y值开始值
			if (y0 < 0)
			{
				dy = - (float)y0;

				xStart = dxdyl * dy + x0;
				zStart = dzdyl * dy + z0;
				uStart = dudyl * dy + u0;
				vStart = dvdyl * dy + v0;

				xEnd = dxdyr * dy + x1;
				zEnd = dzdyr * dy + z1;
				uEnd = dudyr * dy + u1;
				vEnd = dvdyr * dy + v1;

				yStart = 0;
			}
			else
			{
				//注意平顶和平底这里的区别
				xStart = (float)x0;
				uStart = u0;
				vStart = v0;
				zStart = z0;

				xEnd = (float)x1;
				uEnd = u1;
				vEnd = v1;
				zEnd = z1;

				yStart = (float)y0;
			}
		}
		else
		{
			//平底三角形
			dy = 1.0f / (y1 - y0);

			dxdyl = (x1 - x0) * dy;
			dzdyl = (z1 - z0) * dy;
			dudyl = (u1 - u0) * dy;
			dvdyl = (v1 - v0) * dy;

			dxdyr = (x2 - x0) * dy;
			dzdyr = (z2 - z0) * dy;
			dudyr = (u2 - u0) * dy;
			dvdyr = (v2 - v0) * dy;

			if (y0 < 0)
			{
				dy = - (float)y0;

				xStart = dxdyl * dy + x0;
				zStart = dzdyl * dy + z0;
				uStart = dudyl * dy + u0;
				vStart = dvdyl * dy + v0;

				xEnd = dxdyr * dy + x0;
				zEnd = dzdyr * dy + z0;
				uEnd = dudyr * dy + u0;
				vEnd = dvdyr * dy + v0;

				yStart = 0.0f;
			}
			else
			{
				xStart = (float)x0;
				uStart = u0;
				vStart = v0;
				zStart = z0;

				xEnd = (float)x0;
				uEnd = u0;
				vEnd = v0;
				zEnd = z0;

				yStart = (float)y0;
			}
		}
		//y2>视窗高度时，大于rect.height部分就不用画出了
		cyStart = (int)yStart;
		cyEnd = (int)( y2 > nh ? nh : y2 );
		ypos = cyStart * resX;

		//x值需要裁剪的情况
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)xStart;
				cxEnd = xEnd > nw ? nw : (int)xEnd;

				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
				{
					if (cxStart == cxEnd)
					{
						pos = cxStart + ypos;

						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							textureBuffer[pos] = getPixelFromTexture( uStart, uEnd, texture );
							zBuffer[pos] = zStart;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;
						dudx = (uEnd - uStart) * dx;
						dvdx = (vEnd - vStart) * dx;

						currZ = zStart;
						currU = uStart;
						currV = vStart;

						//初始值需要裁剪
						if (cxStart < 0)
						{
							currZ -= cxStart * dzdx;
							currU -= cxStart * dudx;
							currV -= cxStart * dvdx;

							cxStart = 0;
						}
						//绘制扫描线
						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							/*MIN( currU, uEnd );
							MIN( currV, vEnd );*/

							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = color;
								textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currU += dudx;
							currV += dvdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							textureBuffer[pos] = getPixelFromTexture( uEnd, vEnd, texture );
							zBuffer[pos] = zEnd;
						}
					}
				}

				//y每增加1时,xl和xr分别加上他们的递增量
				xStart += dxdyl;
				zStart += dzdyl;
				uStart += dudyl;
				vStart += dvdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				uEnd += dudyr;
				vEnd += dvdyr;

				ypos += resX;
			}
		}
		else
		{
			//x不需要裁剪的情况
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if ( zStart > 0 || zEnd > 0 )
				{
					cxStart = (int)xStart;
					cxEnd = (int)xEnd;

					currZ = zStart;
					currU = uStart;
					currV = vStart;

					if (cxStart == cxEnd)
					{
						pos = cxStart + ypos;
						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
							zBuffer[pos] = currZ;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;
						dudx = (uEnd - uStart) * dx;
						dvdx = (vEnd - vStart) * dx;

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = color;
								textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currU += dudx;
							currV += dvdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							textureBuffer[pos] = getPixelFromTexture( uEnd, vEnd, texture );
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;
				uStart += dudyl;
				vStart += dvdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				uEnd += dudyr;
				vEnd += dvdyr;

				ypos += resX;
			}
		}
	}
	else
	{
		//普通三角形
		yEnd = (float)(y2 > nh ? nh : y2);

		if (y1 < 0)
		{
			//由于y0<y1,这时相当于平顶三角形
			//计算左右斜边的斜率的倒数
			dyl = 1.0f / (y2 - y1);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x2 - x1) * dyl;
			dzdyl = (z2 - z1) * dyl;
			dudyl = (u2 - u1) * dyl;
			dvdyl = (v2 - v1) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dudyr = (u2 - u0) * dyr;
			dvdyr = (v2 - v0) * dyr;

			//计算左右斜边初始值
			dyr = - (float)y0;
			dyl = - (float)y1;

			xStart = dxdyl * dyl + x1;
			zStart = dzdyl * dyl + z1;
			uStart = dudyl * dyl + u1;
			vStart = dvdyl * dyl + v1;

			xEnd = dxdyr * dyr + x0;
			zEnd = dzdyr * dyr + z0;
			uEnd = dudyr * dyr + u0;
			vEnd = dvdyr * dyr + v0;

			yStart = 0;

			if (dxdyr > dxdyl)
			{
				//交换斜边
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = u1;	u1 = u2;	u2 = temp2;
				temp2 = v1;	v1 = v2;	v2 = temp2;

				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
				temp2 = uStart;	uStart = uEnd;	uEnd = temp2;
				temp2 = vStart;	vStart = vEnd;	vEnd = temp2;

				side = 1;
			}
		}
		else if (y0 < 0)
		{
			dyl = 1.0f / (y1 - y0);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dxdyr = (x2 - x0) * dyr;

			dzdyl = (z1 - z0) * dyl;
			dudyl = (u1 - u0) * dyl;
			dvdyl = (v1 - v0) * dyl;

			dzdyr = (z2 - z0) * dyr;
			dudyr = (u2 - u0) * dyr;
			dvdyr = (v2 - v0) * dyr;

			dy = - (float)y0;

			xStart = dxdyl * dy + x0;
			zStart = dzdyl * dy + z0;
			uStart = dudyl * dy + u0;
			vStart = dvdyl * dy + v0;

			xEnd = dxdyr * dy + x0;
			zEnd = dzdyr * dy + z0;
			uEnd = dudyr * dy + u0;
			vEnd = dvdyr * dy + v0;

			yStart = 0;

			if (dxdyr < dxdyl)
			{
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = u1;	u1 = u2;	u2 = temp2;
				temp2 = v1;	v1 = v2;	v2 = temp2;

				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
				temp2 = uStart;	uStart = uEnd;	uEnd = temp2;
				temp2 = vStart;	vStart = vEnd;	vEnd = temp2;

				side = 1;
			}
		}
		else
		{
			//y值都大于0
			dyl = 1.0f / (y1 - y0);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dzdyl = (z1 - z0) * dyl;
			dudyl = (u1 - u0) * dyl;
			dvdyl = (v1 - v0) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dudyr = (u2 - u0) * dyr;
			dvdyr = (v2 - v0) * dyr;

			xStart = (float)x0;
			zStart = z0;
			uStart = u0;
			vStart = v0;

			xEnd = (float)x0;
			zEnd = z0;
			uEnd = u0;
			vEnd = v0;

			yStart = (float)y0;

			if (dxdyr < dxdyl)
			{
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = u1;	u1 = u2;	u2 = temp2;
				temp2 = v1;	v1 = v2;	v2 = temp2;

				side = 1;
			}
		}

		cyStart = (int)yStart;
		cyEnd = (int)yEnd;
		ypos = cyStart * resX;

		//x需要裁剪
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)xStart;
				cxEnd = xEnd > nw ? nw :(int)xEnd;

				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
				{
					if ( cxStart == cxEnd )
					{
						pos = cxStart + ypos;

						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							textureBuffer[pos] = getPixelFromTexture( uStart, vStart, texture );
							zBuffer[pos] = zStart;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;
						dudx = (uEnd - uStart) * dx;
						dvdx = (vEnd - vStart) * dx;

						currZ = zStart;
						currU = uStart;
						currV = vStart;

						//初始值需要裁剪
						if (cxStart < 0)
						{
							currZ -= cxStart * dzdx;
							currU -= cxStart * dudx;
							currV -= cxStart * dvdx;

							cxStart = 0;
						}

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = color;
								textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currU += dudx;
							currV += dvdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							textureBuffer[pos] = getPixelFromTexture( uEnd, vEnd, texture );
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;
				uStart += dudyl;
				vStart += dvdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				uEnd += dudyr;
				vEnd += dvdyr;

				ypos += resX;

				//转折点
				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dzdyl = (z2 - z1) * dyl;
						dudyl = (u2 - u1) * dyl;
						dvdyl = (v2 - v1) * dyl;

						xStart = (float)x1;
						zStart = z1;
						uStart = u1;
						vStart = v1;
					}
					else
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dzdyr = (z1 - z2) * dyr;
						dudyr = (u1 - u2) * dyr;
						dvdyr = (v1 - v2) * dyr;

						xEnd = (float)x2;
						zEnd = z2;
						uEnd = u2;
						vEnd = v2;
					}
				}
			}
		}
		else
		{
			//不需要裁剪
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if ( zStart > 0 || zEnd > 0 )
				{
					cxStart = (int)xStart;
					cxEnd = (int)xEnd;

					currZ = zStart;
					currU = uStart;
					currV = vStart;

					if (xStart == xEnd)
					{
						pos = cxStart + ypos;
						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
							zBuffer[pos] = currZ;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;
						dudx = (uEnd - uStart) * dx;
						dvdx = (vEnd - vStart) * dx;

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = color;
								textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currU += dudx;
							currV += dvdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = color;
							textureBuffer[pos] = getPixelFromTexture( uEnd, vEnd, texture );
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;
				uStart += dudyl;
				vStart += dvdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				uEnd += dudyr;
				vEnd += dvdyr;

				ypos += resX;

				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dzdyl = (z2 - z1) * dyl;
						dudyl = (u2 - u1) * dyl;
						dvdyl = (v2 - v1) * dyl;

						xStart = x1 + dxdyl;
						zStart = z1 + dzdyl;
						uStart = u1 + dudyl;
						vStart = v1 + dvdyl;
					}
					else
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dzdyr = (z1 - z2) * dyr;
						dudyr = (u1 - u2) * dyr;
						dvdyr = (v1 - v2) * dyr;

						xEnd = x2 + dxdyr;
						zEnd = z2 + dzdyr;
						uEnd = u2 + dudyr;
						vEnd = v2 + dvdyr;
					}
				}
			}
		}
	}
}


//void triangle_rasterize_lightOff_texture( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2, Texture * texture )
//{
//	RenderVertex	* tmpV;
//	DWORD			* colorBuffer = view->colorBuffer;
//	float			* zBuffer = view->zBuffer, a, r, g, b;
//
//	int				resX = (int)view->width, resY = (int)view->height;
//	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
//	float			u0, v0, u1, v1, u2, v2, z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, uStart, uEnd, vStart, vEnd, currZ,
//					currU, currV, dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dudyl, dudyr, dvdyl, dvdyr, dzdx, dudx, dvdx, temp2;
//
//
//	//背面剔除
//	if ( BACKFACE_CULLING_MODE == 2)
//	{
//		if ( ( ver2->x - ver0->x ) * ( ver1->y - ver2->y ) > ( ver2->y - ver0->y ) * ( ver1->x - ver2->x ) )
//			return;
//	}
//
//	if (((ver0->y < 0) && (ver1->y < 0) && (ver2->y < 0)) ||
//		((ver0->y > resY) && (ver1->y > resY) && (ver2->y > resY)) ||
//		((ver0->x < 0) && (ver1->x < 0) && (ver2->x < 0)) ||
//		((ver0->x > resX) && (ver1->x > resX) && (ver2->x > resX)))
//	{
//		return;
//	}
//
//	//判断是否是一条线，如果是，则返回。
//	if (((ver0->x == ver1->x) && (ver1->x == ver2->x)) || ((ver0->y == ver1->y) && (ver1->y == ver2->y)))
//	{
//		return;
//	}
//
//	//*************************************** start draw ************************************
//	nw = resX - 1;
//	nh = resY - 1;
//
//	//调整y0,y1,y2,让它们的y值从小到大
//	if (ver1->y < ver0->y)
//	{
//		tmpV = ver1;
//		ver1 = ver0;
//		ver0 = tmpV;
//	}
//	if (ver2->y < ver0->y)
//	{
//		tmpV = ver0;
//		ver0 = ver2;
//		ver2 = tmpV;
//	}
//	if (ver2->y < ver1->y)
//	{
//		tmpV = ver1;
//		ver1 = ver2;
//		ver2 = tmpV;
//	}
//
//	//判断三角形的类型
//	if (ver0->y == ver1->y)
//	{
//		tri_type = FLAT_TOP_TRIANGLE;
//
//		if (ver1->x < ver0->x)
//		{
//			tmpV = ver1;
//			ver1 = ver0;
//			ver0 = tmpV;
//		}
//	}
//	else if (ver1->y == ver2->y)
//	{
//		tri_type = FLAT_BOTTOM_TRIANGLE;
//
//		if (ver2->x < ver1->x)
//		{
//			tmpV = ver1;
//			ver1 = ver2;
//			ver2 = tmpV;
//		}
//	}
//	else
//	{
//		tri_type = GENERAL_TRIANGLE;
//	}
//
//	x0 = (int)ver0->x;
//	y0 = (int)ver0->y;
//	z0 = ver0->z;
//
//	x1 = (int)ver1->x;
//	y1 = (int)ver1->y;
//	z1 = ver1->z;
//
//	x2 = (int)ver2->x;
//	y2 = (int)ver2->y;
//	z2 = ver2->z;
//
//	u0 = ver0->u;
//	v0 = ver0->v;
//	u1 = ver1->u;
//	v1 = ver1->v;
//	u2 = ver2->u;
//	v2 = ver2->v;
//
//	a = ver0->a;
//	r = ver0->r;
//	g = ver0->g;
//	b = ver0->b;
//
//	side = 0;
//
//	//转折后的斜边在哪一侧
//	ys = y1;
//
//	//转折点y坐标
//	if (tri_type == FLAT_TOP_TRIANGLE || tri_type == FLAT_BOTTOM_TRIANGLE)
//	{
//		if (tri_type == FLAT_TOP_TRIANGLE)
//		{
//			dy = 1.0f / (y2 - y0);
//
//			dxdyl = (x2 - x0) * dy;	//左斜边倒数
//			dzdyl = (z2 - z0) * dy;
//			dudyl = (u2 - u0) * dy;
//			dvdyl = (v2 - v0) * dy;
//
//			dxdyr = (x2 - x1) * dy;	//右斜边倒数
//			dzdyr = (z2 - z1) * dy;
//			dudyr = (u2 - u1) * dy;
//			dvdyr = (v2 - v1) * dy;
//
//			//y0小于视窗顶部y坐标，调整左斜边和右斜边当前值,y值开始值
//			if (y0 < 0)
//			{
//				dy = - (float)y0;
//
//				xStart = dxdyl * dy + x0;
//				zStart = dzdyl * dy + z0;
//				uStart = dudyl * dy + u0;
//				vStart = dvdyl * dy + v0;
//
//				xEnd = dxdyr * dy + x1;
//				zEnd = dzdyr * dy + z1;
//				uEnd = dudyr * dy + u1;
//				vEnd = dvdyr * dy + v1;
//
//				yStart = 0;
//			}
//			else
//			{
//				//注意平顶和平底这里的区别
//				xStart = (float)x0;
//				uStart = u0;
//				vStart = v0;
//				zStart = z0;
//
//				xEnd = (float)x1;
//				uEnd = u1;
//				vEnd = v1;
//				zEnd = z1;
//
//				yStart = (float)y0;
//			}
//		}
//		else
//		{
//			//平底三角形
//			dy = 1.0f / (y1 - y0);
//
//			dxdyl = (x1 - x0) * dy;
//			dzdyl = (z1 - z0) * dy;
//			dudyl = (u1 - u0) * dy;
//			dvdyl = (v1 - v0) * dy;
//
//			dxdyr = (x2 - x0) * dy;
//			dzdyr = (z2 - z0) * dy;
//			dudyr = (u2 - u0) * dy;
//			dvdyr = (v2 - v0) * dy;
//
//			if (y0 < 0)
//			{
//				dy = - (float)y0;
//
//				xStart = dxdyl * dy + x0;
//				zStart = dzdyl * dy + z0;
//				uStart = dudyl * dy + u0;
//				vStart = dvdyl * dy + v0;
//
//				xEnd = dxdyr * dy + x0;
//				zEnd = dzdyr * dy + z0;
//				uEnd = dudyr * dy + u0;
//				vEnd = dvdyr * dy + v0;
//
//				yStart = 0.0f;
//			}
//			else
//			{
//				xStart = (float)x0;
//				uStart = u0;
//				vStart = v0;
//				zStart = z0;
//
//				xEnd = (float)x0;
//				uEnd = u0;
//				vEnd = v0;
//				zEnd = z0;
//
//				yStart = (float)y0;
//			}
//		}
//		//y2>视窗高度时，大于rect.height部分就不用画出了
//		cyStart = (int)yStart;
//		cyEnd = (int)( y2 > nh ? nh : y2 );
//		ypos = cyStart * resX;
//
//		//x值需要裁剪的情况
//		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
//		{
//			for ( yi = cyStart; yi <= cyEnd; yi ++ )
//			{
//				cxStart = (int)xStart;
//				cxEnd = xEnd > nw ? nw : (int)xEnd;
//
//				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
//				{
//					if (cxStart == cxEnd)
//					{
//						pos = cxStart + ypos;
//
//						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
//						{
//							colorBuffer[pos] = computePixelColor( r, g, b, a, uStart, vStart, texture );
//							zBuffer[pos] = zStart;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//						dudx = (uEnd - uStart) * dx;
//						dvdx = (vEnd - vStart) * dx;
//
//						currZ = zStart;
//						currU = uStart;
//						currV = vStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//							currU -= cxStart * dudx;
//							currV -= cxStart * dvdx;
//
//							cxStart = 0;
//						}
//						//绘制扫描线
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							/*MIN( currU, uEnd );
//							MIN( currV, vEnd );*/
//
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								colorBuffer[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							colorBuffer[pos] = computePixelColor( r, g, b, a, uEnd, vEnd, texture );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				//y每增加1时,xl和xr分别加上他们的递增量
//				xStart += dxdyl;
//				zStart += dzdyl;
//				uStart += dudyl;
//				vStart += dvdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				uEnd += dudyr;
//				vEnd += dvdyr;
//
//				ypos += resX;
//			}
//		}
//		else
//		{
//			//x不需要裁剪的情况
//			for ( yi = cyStart; yi <= cyEnd; yi ++ )
//			{
//				if ( zStart > 0 || zEnd > 0 )
//				{
//					cxStart = (int)xStart;
//					cxEnd = (int)xEnd;
//
//					currZ = zStart;
//					currU = uStart;
//					currV = vStart;
//
//					if (cxStart == cxEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							colorBuffer[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//						dudx = (uEnd - uStart) * dx;
//						dvdx = (vEnd - vStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								colorBuffer[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							colorBuffer[pos] = computePixelColor( r, g, b, a, uEnd, vEnd, texture );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//				uStart += dudyl;
//				vStart += dvdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				uEnd += dudyr;
//				vEnd += dvdyr;
//
//				ypos += resX;
//			}
//		}
//	}
//	else
//	{
//		//普通三角形
//		yEnd = (float)(y2 > nh ? nh : y2);
//
//		if (y1 < 0)
//		{
//			//由于y0<y1,这时相当于平顶三角形
//			//计算左右斜边的斜率的倒数
//			dyl = 1.0f / (y2 - y1);
//			dyr = 1.0f / (y2 - y0);
//
//			dxdyl = (x2 - x1) * dyl;
//			dzdyl = (z2 - z1) * dyl;
//			dudyl = (u2 - u1) * dyl;
//			dvdyl = (v2 - v1) * dyl;
//
//			dxdyr = (x2 - x0) * dyr;
//			dzdyr = (z2 - z0) * dyr;
//			dudyr = (u2 - u0) * dyr;
//			dvdyr = (v2 - v0) * dyr;
//
//			//计算左右斜边初始值
//			dyr = - (float)y0;
//			dyl = - (float)y1;
//
//			xStart = dxdyl * dyl + x1;
//			zStart = dzdyl * dyl + z1;
//			uStart = dudyl * dyl + u1;
//			vStart = dvdyl * dyl + v1;
//
//			xEnd = dxdyr * dyr + x0;
//			zEnd = dzdyr * dyr + z0;
//			uEnd = dudyr * dyr + u0;
//			vEnd = dvdyr * dyr + v0;
//
//			yStart = 0;
//
//			if (dxdyr > dxdyl)
//			{
//				//交换斜边
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
//				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = u1;	u1 = u2;	u2 = temp2;
//				temp2 = v1;	v1 = v2;	v2 = temp2;
//
//				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
//				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
//				temp2 = uStart;	uStart = uEnd;	uEnd = temp2;
//				temp2 = vStart;	vStart = vEnd;	vEnd = temp2;
//
//				side = 1;
//			}
//		}
//		else if (y0 < 0)
//		{
//			dyl = 1.0f / (y1 - y0);
//			dyr = 1.0f / (y2 - y0);
//
//			dxdyl = (x1 - x0) * dyl;
//			dxdyr = (x2 - x0) * dyr;
//
//			dzdyl = (z1 - z0) * dyl;
//			dudyl = (u1 - u0) * dyl;
//			dvdyl = (v1 - v0) * dyl;
//
//			dzdyr = (z2 - z0) * dyr;
//			dudyr = (u2 - u0) * dyr;
//			dvdyr = (v2 - v0) * dyr;
//
//			dy = - (float)y0;
//
//			xStart = dxdyl * dy + x0;
//			zStart = dzdyl * dy + z0;
//			uStart = dudyl * dy + u0;
//			vStart = dvdyl * dy + v0;
//
//			xEnd = dxdyr * dy + x0;
//			zEnd = dzdyr * dy + z0;
//			uEnd = dudyr * dy + u0;
//			vEnd = dvdyr * dy + v0;
//
//			yStart = 0;
//
//			if (dxdyr < dxdyl)
//			{
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
//				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = u1;	u1 = u2;	u2 = temp2;
//				temp2 = v1;	v1 = v2;	v2 = temp2;
//
//				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
//				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
//				temp2 = uStart;	uStart = uEnd;	uEnd = temp2;
//				temp2 = vStart;	vStart = vEnd;	vEnd = temp2;
//
//				side = 1;
//			}
//		}
//		else
//		{
//			//y值都大于0
//			dyl = 1.0f / (y1 - y0);
//			dyr = 1.0f / (y2 - y0);
//
//			dxdyl = (x1 - x0) * dyl;
//			dzdyl = (z1 - z0) * dyl;
//			dudyl = (u1 - u0) * dyl;
//			dvdyl = (v1 - v0) * dyl;
//
//			dxdyr = (x2 - x0) * dyr;
//			dzdyr = (z2 - z0) * dyr;
//			dudyr = (u2 - u0) * dyr;
//			dvdyr = (v2 - v0) * dyr;
//
//			xStart = (float)x0;
//			zStart = z0;
//			uStart = u0;
//			vStart = v0;
//
//			xEnd = (float)x0;
//			zEnd = z0;
//			uEnd = u0;
//			vEnd = v0;
//
//			yStart = (float)y0;
//
//			if (dxdyr < dxdyl)
//			{
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
//				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = u1;	u1 = u2;	u2 = temp2;
//				temp2 = v1;	v1 = v2;	v2 = temp2;
//
//				side = 1;
//			}
//		}
//
//		cyStart = (int)yStart;
//		cyEnd = (int)yEnd;
//		ypos = cyStart * resX;
//
//		//x需要裁剪
//		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
//		{
//			for ( yi = cyStart; yi <= cyEnd; yi ++ )
//			{
//				cxStart = (int)xStart;
//				cxEnd = xEnd > nw ? nw :(int)xEnd;
//
//				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
//				{
//					if ( cxStart == cxEnd )
//					{
//						pos = cxStart + ypos;
//
//						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
//						{
//							colorBuffer[pos] = computePixelColor( r, g, b, a, uStart, vStart, texture );
//							zBuffer[pos] = zStart;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//						dudx = (uEnd - uStart) * dx;
//						dvdx = (vEnd - vStart) * dx;
//
//						currZ = zStart;
//						currU = uStart;
//						currV = vStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//							currU -= cxStart * dudx;
//							currV -= cxStart * dvdx;
//
//							cxStart = 0;
//						}
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								colorBuffer[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							colorBuffer[pos] = computePixelColor( r, g, b, a, uEnd, vEnd, texture );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//				uStart += dudyl;
//				vStart += dvdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				uEnd += dudyr;
//				vEnd += dvdyr;
//
//				ypos += resX;
//
//				//转折点
//				if (yi == ys)
//				{
//					if (side == 0)
//					{
//						dyl = 1.0f / (y2 - y1);
//
//						dxdyl = (x2 - x1) * dyl;
//						dzdyl = (z2 - z1) * dyl;
//						dudyl = (u2 - u1) * dyl;
//						dvdyl = (v2 - v1) * dyl;
//
//						xStart = (float)x1;
//						zStart = z1;
//						uStart = u1;
//						vStart = v1;
//					}
//					else
//					{
//						dyr = 1.0f / (y1 - y2);
//
//						dxdyr = (x1 - x2) * dyr;
//						dzdyr = (z1 - z2) * dyr;
//						dudyr = (u1 - u2) * dyr;
//						dvdyr = (v1 - v2) * dyr;
//
//						xEnd = (float)x2;
//						zEnd = z2;
//						uEnd = u2;
//						vEnd = v2;
//					}
//				}
//			}
//		}
//		else
//		{
//			//不需要裁剪
//			for ( yi = cyStart; yi <= cyEnd; yi ++ )
//			{
//				if ( zStart > 0 || zEnd > 0 )
//				{
//					cxStart = (int)xStart;
//					cxEnd = (int)xEnd;
//
//					currZ = zStart;
//					currU = uStart;
//					currV = vStart;
//
//					if (xStart == xEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							colorBuffer[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//						dudx = (uEnd - uStart) * dx;
//						dvdx = (vEnd - vStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								colorBuffer[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							colorBuffer[pos] = computePixelColor( r, g, b, a, uEnd, vEnd, texture );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//				uStart += dudyl;
//				vStart += dvdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				uEnd += dudyr;
//				vEnd += dvdyr;
//
//				ypos += resX;
//
//				if (yi == ys)
//				{
//					if (side == 0)
//					{
//						dyl = 1.0f / (y2 - y1);
//
//						dxdyl = (x2 - x1) * dyl;
//						dzdyl = (z2 - z1) * dyl;
//						dudyl = (u2 - u1) * dyl;
//						dvdyl = (v2 - v1) * dyl;
//
//						xStart = x1 + dxdyl;
//						zStart = z1 + dzdyl;
//						uStart = u1 + dudyl;
//						vStart = v1 + dvdyl;
//					}
//					else
//					{
//						dyr = 1.0f / (y1 - y2);
//
//						dxdyr = (x1 - x2) * dyr;
//						dzdyr = (z1 - z2) * dyr;
//						dudyr = (u1 - u2) * dyr;
//						dvdyr = (v1 - v2) * dyr;
//
//						xEnd = x2 + dxdyr;
//						zEnd = z2 + dzdyr;
//						uEnd = u2 + dudyr;
//						vEnd = v2 + dvdyr;
//					}
//				}
//			}
//		}
//	}
//}
//

void triangle_rasterize_light( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2 )
{
	RenderVertex	* tmpV;
	DWORD			* colorBuffer = view->colorBuffer;
	float			* zBuffer = view->zBuffer;

	int				resX = (int)view->width, resY = (int)view->height;
	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
	float			z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, currZ,
					dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr,dzdx, temp2,
					dadyl, dadyr, dadx, currA, drdyl, drdyr, drdx, currR, dgdyl, dgdyr, dgdx, currG, dbdyl, dbdyr, dbdx, currB,
					a0, r0, g0, b0, a1, r1, g1, b1, a2, r2, g2, b2, aStart, aEnd, rStart, rEnd, gStart, gEnd, bStart, bEnd;


	//背面剔除
	if ( BACKFACE_CULLING_MODE == 2)
	{
		if ( ( ver2->x - ver0->x ) * ( ver1->y - ver2->y ) > ( ver2->y - ver0->y ) * ( ver1->x - ver2->x ) )
			return;
	}

	if (((ver0->y < 0) && (ver1->y < 0) && (ver2->y < 0)) ||
		((ver0->y > resY) && (ver1->y > resY) && (ver2->y > resY)) ||
		((ver0->x < 0) && (ver1->x < 0) && (ver2->x < 0)) ||
		((ver0->x > resX) && (ver1->x > resX) && (ver2->x > resX)))
	{
		return;
	}

	//判断是否是一条线，如果是，则返回。
	if (((ver0->x == ver1->x) && (ver1->x == ver2->x)) || ((ver0->y == ver1->y) && (ver1->y == ver2->y)))
	{
		return;
	}

	//*************************************** start draw ************************************
	nw = resX - 1;
	nh = resY - 1;

	//调整y0,y1,y2,让它们的y值从小到大
	if (ver1->y < ver0->y)
	{
		tmpV = ver1;
		ver1 = ver0;
		ver0 = tmpV;
	}
	if (ver2->y < ver0->y)
	{
		tmpV = ver0;
		ver0 = ver2;
		ver2 = tmpV;
	}
	if (ver2->y < ver1->y)
	{
		tmpV = ver1;
		ver1 = ver2;
		ver2 = tmpV;
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

	x0 = (int)ver0->x;
	y0 = (int)ver0->y;
	z0 = ver0->z;

	x1 = (int)ver1->x;
	y1 = (int)ver1->y;
	z1 = ver1->z;

	x2 = (int)ver2->x;
	y2 = (int)ver2->y;
	z2 = ver2->z;

	a0 = ver0->a;
	r0 = ver0->r;
	g0 = ver0->g;
	b0 = ver0->b;

	a1 = ver1->a;
	r1 = ver1->r;
	g1 = ver1->g;
	b1 = ver1->b;

	a2 = ver2->a;
	r2 = ver2->r;
	g2 = ver2->g;
	b2 = ver2->b;

	side = 0;

	//转折后的斜边在哪一侧
	ys = y1;

	//转折点y坐标
	if (tri_type == FLAT_TOP_TRIANGLE || tri_type == FLAT_BOTTOM_TRIANGLE)
	{
		if (tri_type == FLAT_TOP_TRIANGLE)
		{
			dy = 1.0f / (y2 - y0);

			dxdyl = (x2 - x0) * dy;	//左斜边倒数
			dzdyl = (z2 - z0) * dy;
			
			dadyl = (a2 - a0) * dy;
			drdyl = (r2 - r0) * dy;
			dgdyl = (g2 - g0) * dy;
			dbdyl = (b2 - b0) * dy;

			dxdyr = (x2 - x1) * dy;	//右斜边倒数
			dzdyr = (z2 - z1) * dy;

			dadyr = (a2 - a1) * dy;
			drdyr = (r2 - r1) * dy;
			dgdyr = (g2 - g1) * dy;
			dbdyr = (b2 - b1) * dy;

			//y0小于视窗顶部y坐标，调整左斜边和右斜边当前值,y值开始值
			if (y0 < 0)
			{
				dy = - (float)y0;

				xStart = dxdyl * dy + x0;
				zStart = dzdyl * dy + z0;

				aStart = dadyl * dy + a0;
				rStart = drdyl * dy + r0;
				gStart = dgdyl * dy + g0;
				bStart = dbdyl * dy + b0;

				xEnd = dxdyr * dy + x1;
				zEnd = dzdyr * dy + z1;

				aEnd = dadyr * dy + a1;
				rEnd = drdyr * dy + r1;
				gEnd = dgdyr * dy + g1;
				bEnd = dbdyr * dy + b1;

				yStart = 0;
			}
			else
			{
				//注意平顶和平底这里的区别
				xStart = (float)x0;
				zStart = z0;

				aStart = a0;
				rStart = r0;
				gStart = g0;
				bStart = b0;

				xEnd = (float)x1;
				zEnd = z1;

				aEnd = a1;
				rEnd = r1;
				gEnd = g1;
				bEnd = b1;

				yStart = (float)y0;
			}
		}
		else
		{
			//平底三角形
			dy = 1.0f / (y1 - y0);

			dxdyl = (x1 - x0) * dy;
			dzdyl = (z1 - z0) * dy;

			dadyl = (a1 - a0) * dy;
			drdyl = (r1 - r0) * dy;
			dgdyl = (g1 - g0) * dy;
			dbdyl = (b1 - b0) * dy;

			dxdyr = (x2 - x0) * dy;
			dzdyr = (z2 - z0) * dy;

			dadyr = (a2 - a0) * dy;
			drdyr = (r2 - r0) * dy;
			dgdyr = (g2 - g0) * dy;
			dbdyr = (b2 - b0) * dy;

			if (y0 < 0)
			{
				dy = - (float)y0;

				xStart = dxdyl * dy + x0;
				zStart = dzdyl * dy + z0;

				aStart = dadyl * dy + a0;
				rStart = drdyl * dy + r0;
				gStart = dgdyl * dy + g0;
				bStart = dbdyl * dy + b0;

				xEnd = dxdyr * dy + x0;
				zEnd = dzdyr * dy + z0;

				aEnd = dadyr * dy + a0;
				rEnd = drdyr * dy + r0;
				gEnd = dgdyr * dy + g0;
				bEnd = dbdyr * dy + b0;

				yStart = 0.0f;
			}
			else
			{
				xStart = (float)x0;
				zStart = z0;

				aStart = a0;
				rStart = r0;
				gStart = g0;
				bStart = b0;

				xEnd = (float)x0;
				zEnd = z0;

				aEnd = a0;
				rEnd = r0;
				gEnd = g0;
				bEnd = b0;

				yStart = (float)y0;
			}
		}
		//y2>视窗高度时，大于rect.height部分就不用画出了
		cyStart = (int)yStart;
		cyEnd = (int)( y2 > nh ? nh : y2 );
		ypos = cyStart * resX;

		//x值需要裁剪的情况
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)xStart;
				cxEnd = xEnd > nw ? nw : (int)xEnd;

				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
				{
					if (cxStart == cxEnd)
					{
						pos = cxStart + ypos;

						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aStart << 24) + ((int)rStart << 16) + ((int)gStart << 8) + (int)bStart;
							zBuffer[pos] = zStart;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;

						dadx = (aEnd - aStart) * dx;
						drdx = (rEnd - rStart) * dx;
						dgdx = (gEnd - gStart) * dx;
						dbdx = (bEnd - bStart) * dx;

						currZ = zStart;
						currA = aStart;
						currR = rStart;
						currG = gStart;
						currB = bStart;

						//初始值需要裁剪
						if (cxStart < 0)
						{
							currZ -= cxStart * dzdx;
							currA -= cxStart * dadx;
							currR -= cxStart * drdx;
							currG -= cxStart * dgdx;
							currB -= cxStart * dbdx;

							cxStart = 0;
						}
						//绘制扫描线
						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currA += dadx;
							currR += drdx;
							currG += dgdx;
							currB += dbdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aEnd << 24) + ((int)rEnd << 16) + ((int)gEnd << 8) + (int)bEnd;
							zBuffer[pos] = zEnd;
						}
					}
				}

				//y每增加1时,xl和xr分别加上他们的递增量
				xStart += dxdyl;
				zStart += dzdyl;
				
				aStart += dadyl;
				rStart += drdyl;
				gStart += dgdyl;
				bStart += dbdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				
				aEnd += dadyr;
				rEnd += drdyr;
				gEnd += dgdyr;
				bEnd += dbdyr;

				ypos += resX;
			}
		}
		else
		{
			//x不需要裁剪的情况
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if ( zStart > 0 || zEnd > 0 )
				{
					cxStart = (int)xStart;
					cxEnd = (int)xEnd;

					currZ = zStart;
					currA = aStart;
					currR = rStart;
					currG = gStart;
					currB = bStart;

					if (cxStart == cxEnd)
					{
						pos = cxStart + ypos;
						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
							zBuffer[pos] = currZ;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;

						dadx = (aEnd - aStart) * dx;
						drdx = (rEnd - rStart) * dx;
						dgdx = (gEnd - gStart) * dx;
						dbdx = (bEnd - bStart) * dx;

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currA += dadx;
							currR += drdx;
							currG += dgdx;
							currB += dbdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aEnd << 24) + ((int)rEnd << 16) + ((int)gEnd << 8) + (int)bEnd;
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;
				
				aStart += dadyl;
				rStart += drdyl;
				gStart += dgdyl;
				bStart += dbdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				
				aEnd += dadyr;
				rEnd += drdyr;
				gEnd += dgdyr;
				bEnd += dbdyr;

				ypos += resX;
			}
		}
	}
	else
	{
		//普通三角形
		yEnd = (float)(y2 > nh ? nh : y2);

		if (y1 < 0)
		{
			//由于y0<y1,这时相当于平顶三角形
			//计算左右斜边的斜率的倒数
			dyl = 1.0f / (y2 - y1);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x2 - x1) * dyl;
			dzdyl = (z2 - z1) * dyl;

			dadyl = (a2 - a1) * dyl;
			drdyl = (r2 - r1) * dyl;
			dgdyl = (g2 - g1) * dyl;
			dbdyl = (b2 - b1) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;

			dadyr = (a2 - a0) * dyr;
			drdyr = (r2 - r0) * dyr;
			dbdyr = (g2 - g0) * dyr;
			dgdyr = (b2 - b0) * dyr;

			//计算左右斜边初始值
			dyr = - (float)y0;
			dyl = - (float)y1;

			xStart = dxdyl * dyl + x1;
			zStart = dzdyl * dyl + z1;

			aStart = dadyl * dyl + a1;
			rStart = drdyl * dyl + r1;
			gStart = dgdyl * dyl + g1;
			bStart = dbdyl * dyl + b1;

			xEnd = dxdyr * dyr + x0;
			zEnd = dzdyr * dyr + z0;
			
			aEnd = dadyr * dyr + a0;
			rEnd = drdyr * dyr + r0;
			gEnd = dgdyr * dyr + g0;
			bEnd = dbdyr * dyr + b0;

			yStart = 0;

			if (dxdyr > dxdyl)
			{
				//交换斜边
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				
				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
				
				temp2 = aStart;	aStart = aEnd;	aEnd = temp2;
				temp2 = rStart;	rStart = rEnd;	rEnd = temp2;
				temp2 = gStart;	gStart = gEnd;	gEnd = temp2;
				temp2 = bStart;	bStart = bEnd;	bEnd = temp2;

				side = 1;
			}
		}
		else if (y0 < 0)
		{
			dyl = 1.0f / (y1 - y0);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dxdyr = (x2 - x0) * dyr;

			dzdyl = (z1 - z0) * dyl;
			
			dadyl = (a1 - a0) * dyl;
			drdyl = (r1 - r0) * dyl;
			dgdyl = (g1 - g0) * dyl;
			dbdyl = (b1 - b0) * dyl;

			dzdyr = (z2 - z0) * dyr;
			
			dadyr = (a2 - a0) * dyr;
			drdyr = (r2 - r0) * dyr;
			dgdyr = (g2 - g0) * dyr;
			dbdyr = (b2 - b0) * dyr;

			dy = - (float)y0;

			xStart = dxdyl * dy + x0;
			zStart = dzdyl * dy + z0;
			
			aStart = dadyl * dy + a0;
			rStart = drdyl * dy + r0;
			gStart = dgdyl * dy + g0;
			bStart = dbdyl * dy + b0;

			xEnd = dxdyr * dy + x0;
			zEnd = dzdyr * dy + z0;
			
			aEnd = dadyr * dy + a0;
			rEnd = drdyr * dy + r0;
			gEnd = dgdyr * dy + g0;
			bEnd = dbdyr * dy + b0;

			yStart = 0;

			if (dxdyr < dxdyl)
			{
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				
				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
				
				temp2 = aStart;	aStart = aEnd;	aEnd = temp2;
				temp2 = rStart;	rStart = rEnd;	rEnd = temp2;
				temp2 = gStart;	gStart = gEnd;	gEnd = temp2;
				temp2 = bStart;	bStart = bEnd;	bEnd = temp2;

				side = 1;
			}
		}
		else
		{
			//y值都大于0
			dyl = 1.0f / (y1 - y0);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dzdyl = (z1 - z0) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;

			dadyl = (a1 - a0) * dyl;
			drdyl = (r1 - r0) * dyl;
			dgdyl = (g1 - g0) * dyl;
			dbdyl = (b1 - b0) * dyl;

			dadyr = (a2 - a0) * dyr;
			drdyr = (r2 - r0) * dyr;
			dgdyr = (g2 - g0) * dyr;
			dbdyr = (b2 - b0) * dyr;

			xStart = (float)x0;
			zStart = z0;
			
			aStart = a0;
			rStart = r0;
			gStart = g0;
			bStart = b0;

			xEnd = (float)x0;
			zEnd = z0;
			
			aEnd = a0;
			rEnd = r0;
			gEnd = g0;
			bEnd = b0;

			yStart = (float)y0;

			if (dxdyr < dxdyl)
			{
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;

				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = a1;	a1 = a2;	a2 = temp2;
				temp2 = r1;	r1 = r2;	r2 = temp2;
				temp2 = g1;	g1 = g2;	g2 = temp2;
				temp2 = b1;	b1 = b2;	b2 = temp2;

				side = 1;
			}
		}

		cyStart = (int)yStart;
		cyEnd = (int)yEnd;
		ypos = cyStart * resX;

		//x需要裁剪
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)xStart;
				cxEnd = xEnd > nw ? nw :(int)xEnd;

				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
				{
					if ( cxStart == cxEnd )
					{
						pos = cxStart + ypos;

						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aStart << 24) + ((int)rStart << 16) + ((int)gStart << 8) + (int)bStart;
							zBuffer[pos] = zStart;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;
						
						dadx = (aEnd - aStart) * dx;
						drdx = (rEnd - rStart) * dx;
						dgdx = (gEnd - gStart) * dx;
						dbdx = (bEnd - bStart) * dx;

						currZ = zStart;
						currA = aStart;
						currR = rStart;
						currG = gStart;
						currB = bStart;

						//初始值需要裁剪
						if (cxStart < 0)
						{
							currZ -= cxStart * dzdx;
							currA -= cxStart * dadx;
							currR -= cxStart * drdx;
							currG -= cxStart * dgdx;
							currB -= cxStart * dbdx;

							cxStart = 0;
						}

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currA += dadx;
							currR += drdx;
							currG += dgdx;
							currB += dbdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aEnd << 24) + ((int)rEnd << 16) + ((int)gEnd << 8) + (int)bEnd;
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;

				aStart += dadyl;
				rStart += drdyl;
				gStart += dgdyl;
				bStart += dbdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;

				aEnd += dadyr;
				rEnd += drdyr;
				gEnd += dgdyr;
				bEnd += dbdyr;

				ypos += resX;

				//转折点
				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dzdyl = (z2 - z1) * dyl;
						
						dadyl = (a2 - a1) * dyl;
						drdyl = (r2 - r1) * dyl;
						dgdyl = (g2 - g1) * dyl;
						dbdyl = (b2 - b1) * dyl;

						xStart = (float)x1;
						zStart = z1;
						
						aStart = a1;
						rStart = r1;
						gStart = g1;
						bStart = b1;
					}
					else
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dzdyr = (z1 - z2) * dyr;
						
						dadyr = (a1 - a2) * dyr;
						drdyr = (r1 - r2) * dyr;
						dgdyr = (g1 - g2) * dyr;
						dbdyr = (b1 - b2) * dyr;

						xEnd = (float)x2;
						zEnd = z2;
						
						aEnd = a2;
						rEnd = r2;
						gEnd = g2;
						bEnd = b2;
					}
				}
			}
		}
		else
		{
			//不需要裁剪
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if ( zStart > 0 || zEnd > 0 )
				{
					cxStart = (int)xStart;
					cxEnd = (int)xEnd;

					currZ = zStart;
					currA = aStart;
					currR = rStart;
					currG = gStart;
					currB = bStart;

					if (xStart == xEnd)
					{
						pos = cxStart + ypos;
						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
							zBuffer[pos] = currZ;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;

						dadx = (aEnd - aStart) * dx;
						drdx = (rEnd - rStart) * dx;
						dgdx = (gEnd - gStart) * dx;
						dbdx = (bEnd - bStart) * dx;

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currA += dadx;
							currR += drdx;
							currG += dgdx;
							currB += dbdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aEnd << 24) + ((int)rEnd << 16) + ((int)gEnd << 8) + (int)bEnd;
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;
				
				aStart += dadyl;
				rStart += drdyl;
				gStart += dgdyl;
				bStart += dbdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				
				aEnd += dadyr;
				rEnd += drdyr;
				gEnd += dgdyr;
				bEnd += dbdyr;

				ypos += resX;

				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dzdyl = (z2 - z1) * dyl;
						
						dadyl = (a2 - a1) * dyl;
						drdyl = (r2 - r1) * dyl;
						dgdyl = (g2 - g1) * dyl;
						dbdyl = (b2 - b1) * dyl;

						xStart = x1 + dxdyl;
						zStart = z1 + dzdyl;
						
						aStart = a1 + dadyl;
						rStart = r1 + drdyl;
						gStart = g1 + dgdyl;
						bStart = b1 + dbdyl;
					}
					else
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dzdyr = (z1 - z2) * dyr;
						
						dadyr = (a1 - a2) * dyr;
						drdyr = (r1 - r2) * dyr;
						dgdyr = (g1 - g2) * dyr;
						dbdyr = (b1 - b2) * dyr;

						xEnd = x2 + dxdyr;
						zEnd = z2 + dzdyr;
						
						aEnd = a2 + dadyr;
						rEnd = r2 + drdyr;
						gEnd = g2 + dgdyr;
						bEnd = b2 + dbdyr;
					}
				}
			}
		}
	}
}

void triangle_rasterize_light_texture( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2, Texture * texture )
{
	RenderVertex	* tmpV;
	DWORD			* colorBuffer = view->colorBuffer;
	DWORD			* textureBuffer = view->textureBuffer;
	float			* zBuffer = view->zBuffer;

	int				resX = (int)view->width, resY = (int)view->height;
	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
	float			u0, v0, u1, v1, u2, v2, z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, uStart, uEnd, vStart, vEnd, currZ,
					currU, currV, dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dudyl, dudyr, dvdyl, dvdyr, dzdx, dudx, dvdx, temp2,
					dadyl, dadyr, dadx, currA, drdyl, drdyr, drdx, currR, dgdyl, dgdyr, dgdx, currG, dbdyl, dbdyr, dbdx, currB,
					a0, r0, g0, b0, a1, r1, g1, b1, a2, r2, g2, b2, aStart, aEnd, rStart, rEnd, gStart, gEnd, bStart, bEnd;


	//背面剔除
	if ( BACKFACE_CULLING_MODE == 2)
	{
		if ( ( ver2->x - ver0->x ) * ( ver1->y - ver2->y ) > ( ver2->y - ver0->y ) * ( ver1->x - ver2->x ) )
			return;
	}

	if (((ver0->y < 0) && (ver1->y < 0) && (ver2->y < 0)) ||
		((ver0->y > resY) && (ver1->y > resY) && (ver2->y > resY)) ||
		((ver0->x < 0) && (ver1->x < 0) && (ver2->x < 0)) ||
		((ver0->x > resX) && (ver1->x > resX) && (ver2->x > resX)))
	{
		return;
	}

	//判断是否是一条线，如果是，则返回。
	if (((ver0->x == ver1->x) && (ver1->x == ver2->x)) || ((ver0->y == ver1->y) && (ver1->y == ver2->y)))
	{
		return;
	}

	//*************************************** start draw ************************************
	nw = resX - 1;
	nh = resY - 1;

	//调整y0,y1,y2,让它们的y值从小到大
	if (ver1->y < ver0->y)
	{
		tmpV = ver1;
		ver1 = ver0;
		ver0 = tmpV;
	}
	if (ver2->y < ver0->y)
	{
		tmpV = ver0;
		ver0 = ver2;
		ver2 = tmpV;
	}
	if (ver2->y < ver1->y)
	{
		tmpV = ver1;
		ver1 = ver2;
		ver2 = tmpV;
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

	x0 = (int)ver0->x;
	y0 = (int)ver0->y;
	z0 = ver0->z;

	x1 = (int)ver1->x;
	y1 = (int)ver1->y;
	z1 = ver1->z;

	x2 = (int)ver2->x;
	y2 = (int)ver2->y;
	z2 = ver2->z;

	u0 = ver0->u;
	v0 = ver0->v;
	u1 = ver1->u;
	v1 = ver1->v;
	u2 = ver2->u;
	v2 = ver2->v;

	a0 = ver0->a;
	r0 = ver0->r;
	g0 = ver0->g;
	b0 = ver0->b;

	a1 = ver1->a;
	r1 = ver1->r;
	g1 = ver1->g;
	b1 = ver1->b;

	a2 = ver2->a;
	r2 = ver2->r;
	g2 = ver2->g;
	b2 = ver2->b;

	/*AS3_Trace(AS3_String("Output0 XYZ..............."));
	AS3_Trace(AS3_String("X0Y0...."));
	AS3_Trace(AS3_Int(x0));
	AS3_Trace(AS3_Int(y0));
	AS3_Trace(AS3_Number(u0));
	AS3_Trace(AS3_Number(v0));
	AS3_Trace(AS3_String("X1Y1...."));
	AS3_Trace(AS3_Int(x1));
	AS3_Trace(AS3_Int(y1));
	AS3_Trace(AS3_Number(u1));
	AS3_Trace(AS3_Number(v1));
	AS3_Trace(AS3_String("X2Y2...."));
	AS3_Trace(AS3_Int(x2));
	AS3_Trace(AS3_Int(y2));
	AS3_Trace(AS3_Number(u2));
	AS3_Trace(AS3_Number(v2));*/

	side = 0;

	//转折后的斜边在哪一侧
	ys = y1;

	//转折点y坐标
	if (tri_type == FLAT_TOP_TRIANGLE || tri_type == FLAT_BOTTOM_TRIANGLE)
	{
		if (tri_type == FLAT_TOP_TRIANGLE)
		{
			dy = 1.0f / (y2 - y0);

			dxdyl = (x2 - x0) * dy;	//左斜边倒数
			dzdyl = (z2 - z0) * dy;
			dudyl = (u2 - u0) * dy;
			dvdyl = (v2 - v0) * dy;
			
			dadyl = (a2 - a0) * dy;
			drdyl = (r2 - r0) * dy;
			dgdyl = (g2 - g0) * dy;
			dbdyl = (b2 - b0) * dy;

			dxdyr = (x2 - x1) * dy;	//右斜边倒数
			dzdyr = (z2 - z1) * dy;
			dudyr = (u2 - u1) * dy;
			dvdyr = (v2 - v1) * dy;

			dadyr = (a2 - a1) * dy;
			drdyr = (r2 - r1) * dy;
			dgdyr = (g2 - g1) * dy;
			dbdyr = (b2 - b1) * dy;

			//y0小于视窗顶部y坐标，调整左斜边和右斜边当前值,y值开始值
			if (y0 < 0)
			{
				dy = - (float)y0;

				xStart = dxdyl * dy + x0;
				zStart = dzdyl * dy + z0;
				uStart = dudyl * dy + u0;
				vStart = dvdyl * dy + v0;

				aStart = dadyl * dy + a0;
				rStart = drdyl * dy + r0;
				gStart = dgdyl * dy + g0;
				bStart = dbdyl * dy + b0;

				xEnd = dxdyr * dy + x1;
				zEnd = dzdyr * dy + z1;
				uEnd = dudyr * dy + u1;
				vEnd = dvdyr * dy + v1;

				aEnd = dadyr * dy + a1;
				rEnd = drdyr * dy + r1;
				gEnd = dgdyr * dy + g1;
				bEnd = dbdyr * dy + b1;

				yStart = 0;
			}
			else
			{
				//注意平顶和平底这里的区别
				xStart = (float)x0;
				uStart = u0;
				vStart = v0;
				zStart = z0;

				aStart = a0;
				rStart = r0;
				gStart = g0;
				bStart = b0;

				xEnd = (float)x1;
				uEnd = u1;
				vEnd = v1;
				zEnd = z1;

				aEnd = a1;
				rEnd = r1;
				gEnd = g1;
				bEnd = b1;

				yStart = (float)y0;
			}
		}
		else
		{
			//平底三角形
			dy = 1.0f / (y1 - y0);

			dxdyl = (x1 - x0) * dy;
			dzdyl = (z1 - z0) * dy;
			dudyl = (u1 - u0) * dy;
			dvdyl = (v1 - v0) * dy;

			dadyl = (a1 - a0) * dy;
			drdyl = (r1 - r0) * dy;
			dgdyl = (g1 - g0) * dy;
			dbdyl = (b1 - b0) * dy;

			dxdyr = (x2 - x0) * dy;
			dzdyr = (z2 - z0) * dy;
			dudyr = (u2 - u0) * dy;
			dvdyr = (v2 - v0) * dy;

			dadyr = (a2 - a0) * dy;
			drdyr = (r2 - r0) * dy;
			dgdyr = (g2 - g0) * dy;
			dbdyr = (b2 - b0) * dy;

			if (y0 < 0)
			{
				dy = - (float)y0;

				xStart = dxdyl * dy + x0;
				zStart = dzdyl * dy + z0;
				uStart = dudyl * dy + u0;
				vStart = dvdyl * dy + v0;

				aStart = dadyl * dy + a0;
				rStart = drdyl * dy + r0;
				gStart = dgdyl * dy + g0;
				bStart = dbdyl * dy + b0;

				xEnd = dxdyr * dy + x0;
				zEnd = dzdyr * dy + z0;
				uEnd = dudyr * dy + u0;
				vEnd = dvdyr * dy + v0;

				aEnd = dadyr * dy + a0;
				rEnd = drdyr * dy + r0;
				gEnd = dgdyr * dy + g0;
				bEnd = dbdyr * dy + b0;

				yStart = 0.0f;
			}
			else
			{
				xStart = (float)x0;
				uStart = u0;
				vStart = v0;
				zStart = z0;

				aStart = a0;
				rStart = r0;
				gStart = g0;
				bStart = b0;

				xEnd = (float)x0;
				uEnd = u0;
				vEnd = v0;
				zEnd = z0;

				aEnd = a0;
				rEnd = r0;
				gEnd = g0;
				bEnd = b0;

				yStart = (float)y0;
			}
		}
		//y2>视窗高度时，大于rect.height部分就不用画出了
		cyStart = (int)yStart;
		cyEnd = (int)( y2 > nh ? nh : y2 );
		ypos = cyStart * resX;

		//x值需要裁剪的情况
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)xStart;
				cxEnd = xEnd > nw ? nw : (int)xEnd;

				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
				{
					if (cxStart == cxEnd)
					{
						pos = cxStart + ypos;

						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aStart << 24) + ((int)rStart << 16) + ((int)gStart << 8) + (int)bStart;
							textureBuffer[pos] = getPixelFromTexture( uStart, vStart, texture );
							zBuffer[pos] = zStart;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;
						dudx = (uEnd - uStart) * dx;
						dvdx = (vEnd - vStart) * dx;

						dadx = (aEnd - aStart) * dx;
						drdx = (rEnd - rStart) * dx;
						dgdx = (gEnd - gStart) * dx;
						dbdx = (bEnd - bStart) * dx;

						currZ = zStart;
						currU = uStart;
						currV = vStart;
						currA = aStart;
						currR = rStart;
						currG = gStart;
						currB = bStart;

						//初始值需要裁剪
						if (cxStart < 0)
						{
							currZ -= cxStart * dzdx;
							currU -= cxStart * dudx;
							currV -= cxStart * dvdx;
							currA -= cxStart * dadx;
							currR -= cxStart * drdx;
							currG -= cxStart * dgdx;
							currB -= cxStart * dbdx;

							cxStart = 0;
						}
						//绘制扫描线
						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
								textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currU += dudx;
							currV += dvdx;
							currA += dadx;
							currR += drdx;
							currG += dgdx;
							currB += dbdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aEnd << 24) + ((int)rEnd << 16) + ((int)gEnd << 8) + (int)bEnd;
							textureBuffer[pos] = getPixelFromTexture( uEnd, vEnd, texture );
							zBuffer[pos] = zEnd;
						}
					}
				}

				//y每增加1时,xl和xr分别加上他们的递增量
				xStart += dxdyl;
				zStart += dzdyl;
				uStart += dudyl;
				vStart += dvdyl;
				
				aStart += dadyl;
				rStart += drdyl;
				gStart += dgdyl;
				bStart += dbdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				uEnd += dudyr;
				vEnd += dvdyr;
				
				aEnd += dadyr;
				rEnd += drdyr;
				gEnd += dgdyr;
				bEnd += dbdyr;

				ypos += resX;
			}
		}
		else
		{
			//x不需要裁剪的情况
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if ( zStart > 0 || zEnd > 0 )
				{
					cxStart = (int)xStart;
					cxEnd = (int)xEnd;

					currZ = zStart;
					currU = uStart;
					currV = vStart;
					currA = aStart;
					currR = rStart;
					currG = gStart;
					currB = bStart;

					if (cxStart == cxEnd)
					{
						pos = cxStart + ypos;
						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
							textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
							zBuffer[pos] = currZ;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;
						dudx = (uEnd - uStart) * dx;
						dvdx = (vEnd - vStart) * dx;

						dadx = (aEnd - aStart) * dx;
						drdx = (rEnd - rStart) * dx;
						dgdx = (gEnd - gStart) * dx;
						dbdx = (bEnd - bStart) * dx;

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
								textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currU += dudx;
							currV += dvdx;
							currA += dadx;
							currR += drdx;
							currG += dgdx;
							currB += dbdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aEnd << 24) + ((int)rEnd << 16) + ((int)gEnd << 8) + (int)bEnd;
							textureBuffer[pos] = getPixelFromTexture( uEnd, vEnd, texture );
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;
				uStart += dudyl;
				vStart += dvdyl;
				
				aStart += dadyl;
				rStart += drdyl;
				gStart += dgdyl;
				bStart += dbdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				uEnd += dudyr;
				vEnd += dvdyr;
				
				aEnd += dadyr;
				rEnd += drdyr;
				gEnd += dgdyr;
				bEnd += dbdyr;

				ypos += resX;
			}
		}
	}
	else
	{
		//普通三角形
		yEnd = (float)(y2 > nh ? nh : y2);

		if (y1 < 0)
		{
			//由于y0<y1,这时相当于平顶三角形
			//计算左右斜边的斜率的倒数
			dyl = 1.0f / (y2 - y1);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x2 - x1) * dyl;
			dzdyl = (z2 - z1) * dyl;
			dudyl = (u2 - u1) * dyl;
			dvdyl = (v2 - v1) * dyl;

			dadyl = (a2 - a1) * dyl;
			drdyl = (r2 - r1) * dyl;
			dgdyl = (g2 - g1) * dyl;
			dbdyl = (b2 - b1) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dudyr = (u2 - u0) * dyr;
			dvdyr = (v2 - v0) * dyr;

			dadyr = (a2 - a0) * dyr;
			drdyr = (r2 - r0) * dyr;
			dbdyr = (g2 - g0) * dyr;
			dgdyr = (b2 - b0) * dyr;

			//计算左右斜边初始值
			dyr = - (float)y0;
			dyl = - (float)y1;

			xStart = dxdyl * dyl + x1;
			zStart = dzdyl * dyl + z1;
			uStart = dudyl * dyl + u1;
			vStart = dvdyl * dyl + v1;

			aStart = dadyl * dyl + a1;
			rStart = drdyl * dyl + r1;
			gStart = dgdyl * dyl + g1;
			bStart = dbdyl * dyl + b1;

			xEnd = dxdyr * dyr + x0;
			zEnd = dzdyr * dyr + z0;
			uEnd = dudyr * dyr + u0;
			vEnd = dvdyr * dyr + v0;
			
			aEnd = dadyr * dyr + a0;
			rEnd = drdyr * dyr + r0;
			gEnd = dgdyr * dyr + g0;
			bEnd = dbdyr * dyr + b0;

			yStart = 0;

			if (dxdyr > dxdyl)
			{
				//交换斜边
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;
				
				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = u1;	u1 = u2;	u2 = temp2;
				temp2 = v1;	v1 = v2;	v2 = temp2;

				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
				temp2 = uStart;	uStart = uEnd;	uEnd = temp2;
				temp2 = vStart;	vStart = vEnd;	vEnd = temp2;
				
				temp2 = aStart;	aStart = aEnd;	aEnd = temp2;
				temp2 = rStart;	rStart = rEnd;	rEnd = temp2;
				temp2 = gStart;	gStart = gEnd;	gEnd = temp2;
				temp2 = bStart;	bStart = bEnd;	bEnd = temp2;

				side = 1;
			}
		}
		else if (y0 < 0)
		{
			dyl = 1.0f / (y1 - y0);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dxdyr = (x2 - x0) * dyr;

			dzdyl = (z1 - z0) * dyl;
			dudyl = (u1 - u0) * dyl;
			dvdyl = (v1 - v0) * dyl;
			
			dadyl = (a1 - a0) * dyl;
			drdyl = (r1 - r0) * dyl;
			dgdyl = (g1 - g0) * dyl;
			dbdyl = (b1 - b0) * dyl;

			dzdyr = (z2 - z0) * dyr;
			dudyr = (u2 - u0) * dyr;
			dvdyr = (v2 - v0) * dyr;
			
			dadyr = (a2 - a0) * dyr;
			drdyr = (r2 - r0) * dyr;
			dgdyr = (g2 - g0) * dyr;
			dbdyr = (b2 - b0) * dyr;

			dy = - (float)y0;

			xStart = dxdyl * dy + x0;
			zStart = dzdyl * dy + z0;
			uStart = dudyl * dy + u0;
			vStart = dvdyl * dy + v0;
			
			aStart = dadyl * dy + a0;
			rStart = drdyl * dy + r0;
			gStart = dgdyl * dy + g0;
			bStart = dbdyl * dy + b0;

			xEnd = dxdyr * dy + x0;
			zEnd = dzdyr * dy + z0;
			uEnd = dudyr * dy + u0;
			vEnd = dvdyr * dy + v0;
			
			aEnd = dadyr * dy + a0;
			rEnd = drdyr * dy + r0;
			gEnd = dgdyr * dy + g0;
			bEnd = dbdyr * dy + b0;

			yStart = 0;

			if (dxdyr < dxdyl)
			{
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;
				
				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = u1;	u1 = u2;	u2 = temp2;
				temp2 = v1;	v1 = v2;	v2 = temp2;

				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
				temp2 = uStart;	uStart = uEnd;	uEnd = temp2;
				temp2 = vStart;	vStart = vEnd;	vEnd = temp2;
				
				temp2 = aStart;	aStart = aEnd;	aEnd = temp2;
				temp2 = rStart;	rStart = rEnd;	rEnd = temp2;
				temp2 = gStart;	gStart = gEnd;	gEnd = temp2;
				temp2 = bStart;	bStart = bEnd;	bEnd = temp2;

				side = 1;
			}
		}
		else
		{
			//y值都大于0
			dyl = 1.0f / (y1 - y0);
			dyr = 1.0f / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dzdyl = (z1 - z0) * dyl;
			dudyl = (u1 - u0) * dyl;
			dvdyl = (v1 - v0) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dudyr = (u2 - u0) * dyr;
			dvdyr = (v2 - v0) * dyr;

			dadyl = (a1 - a0) * dyl;
			drdyl = (r1 - r0) * dyl;
			dgdyl = (g1 - g0) * dyl;
			dbdyl = (b1 - b0) * dyl;

			dadyr = (a2 - a0) * dyr;
			drdyr = (r2 - r0) * dyr;
			dgdyr = (g2 - g0) * dyr;
			dbdyr = (b2 - b0) * dyr;

			xStart = (float)x0;
			zStart = z0;
			uStart = u0;
			vStart = v0;
			
			aStart = a0;
			rStart = r0;
			gStart = g0;
			bStart = b0;

			xEnd = (float)x0;
			zEnd = z0;
			uEnd = u0;
			vEnd = v0;
			
			aEnd = a0;
			rEnd = r0;
			gEnd = g0;
			bEnd = b0;

			yStart = (float)y0;

			if (dxdyr < dxdyl)
			{
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;

				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp2 = z1;	z1 = z2;	z2 = temp2;

				temp2 = u1;	u1 = u2;	u2 = temp2;
				temp2 = v1;	v1 = v2;	v2 = temp2;

				temp2 = a1;	a1 = a2;	a2 = temp2;
				temp2 = r1;	r1 = r2;	r2 = temp2;
				temp2 = g1;	g1 = g2;	g2 = temp2;
				temp2 = b1;	b1 = b2;	b2 = temp2;

				side = 1;
			}
		}

		cyStart = (int)yStart;
		cyEnd = (int)yEnd;
		ypos = cyStart * resX;

		//x需要裁剪
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)xStart;
				cxEnd = xEnd > nw ? nw :(int)xEnd;

				if ( cxEnd >= 0 && cxStart <= nw && ( ( zStart > 0 && zStart < 1 )  || ( zEnd > 0 && zEnd < 1 ) ) )
				{
					if ( cxStart == cxEnd )
					{
						pos = cxStart + ypos;

						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aStart << 24) + ((int)rStart << 16) + ((int)gStart << 8) + (int)bStart;
							textureBuffer[pos] = getPixelFromTexture( uStart, vStart, texture );
							zBuffer[pos] = zStart;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);

						dzdx = (zEnd - zStart) * dx;
						dudx = (uEnd - uStart) * dx;
						dvdx = (vEnd - vStart) * dx;
						
						dadx = (aEnd - aStart) * dx;
						drdx = (rEnd - rStart) * dx;
						dgdx = (gEnd - gStart) * dx;
						dbdx = (bEnd - bStart) * dx;

						currZ = zStart;
						currU = uStart;
						currV = vStart;
						currA = aStart;
						currR = rStart;
						currG = gStart;
						currB = bStart;

						//初始值需要裁剪
						if (cxStart < 0)
						{
							currZ -= cxStart * dzdx;
							currU -= cxStart * dudx;
							currV -= cxStart * dvdx;
							currA -= cxStart * dadx;
							currR -= cxStart * drdx;
							currG -= cxStart * dgdx;
							currB -= cxStart * dbdx;

							cxStart = 0;
						}

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
								textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currU += dudx;
							currV += dvdx;
							currA += dadx;
							currR += drdx;
							currG += dgdx;
							currB += dbdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aEnd << 24) + ((int)rEnd << 16) + ((int)gEnd << 8) + (int)bEnd;
							textureBuffer[pos] = getPixelFromTexture( uEnd, vEnd, texture );
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;
				uStart += dudyl;
				vStart += dvdyl;

				aStart += dadyl;
				rStart += drdyl;
				gStart += dgdyl;
				bStart += dbdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				uEnd += dudyr;
				vEnd += dvdyr;

				aEnd += dadyr;
				rEnd += drdyr;
				gEnd += dgdyr;
				bEnd += dbdyr;

				ypos += resX;

				//转折点
				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dzdyl = (z2 - z1) * dyl;
						dudyl = (u2 - u1) * dyl;
						dvdyl = (v2 - v1) * dyl;
						
						dadyl = (a2 - a1) * dyl;
						drdyl = (r2 - r1) * dyl;
						dgdyl = (g2 - g1) * dyl;
						dbdyl = (b2 - b1) * dyl;

						xStart = (float)x1;
						zStart = z1;
						uStart = u1;
						vStart = v1;
						
						aStart = a1;
						rStart = r1;
						gStart = g1;
						bStart = b1;
					}
					else
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dzdyr = (z1 - z2) * dyr;
						dudyr = (u1 - u2) * dyr;
						dvdyr = (v1 - v2) * dyr;
						
						dadyr = (a1 - a2) * dyr;
						drdyr = (r1 - r2) * dyr;
						dgdyr = (g1 - g2) * dyr;
						dbdyr = (b1 - b2) * dyr;

						xEnd = (float)x2;
						zEnd = z2;
						uEnd = u2;
						vEnd = v2;
						
						aEnd = a2;
						rEnd = r2;
						gEnd = g2;
						bEnd = b2;
					}
				}
			}
		}
		else
		{
			//不需要裁剪
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if ( zStart > 0 || zEnd > 0 )
				{
					cxStart = (int)xStart;
					cxEnd = (int)xEnd;

					currZ = zStart;
					currU = uStart;
					currV = vStart;
					currA = aStart;
					currR = rStart;
					currG = gStart;
					currB = bStart;

					if (xStart == xEnd)
					{
						pos = cxStart + ypos;
						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
							textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
							zBuffer[pos] = currZ;
						}
					}
					else
					{
						dx = 1.0f / (xEnd - xStart);
						dzdx = (zEnd - zStart) * dx;
						dudx = (uEnd - uStart) * dx;
						dvdx = (vEnd - vStart) * dx;

						dadx = (aEnd - aStart) * dx;
						drdx = (rEnd - rStart) * dx;
						dgdx = (gEnd - gStart) * dx;
						dbdx = (bEnd - bStart) * dx;

						for ( xi = cxStart; xi < cxEnd; xi ++ )
						{
							pos = xi + ypos;
							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
							{
								colorBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
								textureBuffer[pos] = getPixelFromTexture( currU, currV, texture );
								zBuffer[pos] = currZ;
							}

							currZ += dzdx;
							currU += dudx;
							currV += dvdx;
							currA += dadx;
							currR += drdx;
							currG += dgdx;
							currB += dbdx;
						}

						pos = cxEnd + ypos;
						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
						{
							colorBuffer[pos] = ((int)aEnd << 24) + ((int)rEnd << 16) + ((int)gEnd << 8) + (int)bEnd;
							textureBuffer[pos] = getPixelFromTexture( uEnd, vEnd, texture );
							zBuffer[pos] = zEnd;
						}
					}
				}

				xStart += dxdyl;
				zStart += dzdyl;
				uStart += dudyl;
				vStart += dvdyl;
				
				aStart += dadyl;
				rStart += drdyl;
				gStart += dgdyl;
				bStart += dbdyl;

				xEnd += dxdyr;
				zEnd += dzdyr;
				uEnd += dudyr;
				vEnd += dvdyr;
				
				aEnd += dadyr;
				rEnd += drdyr;
				gEnd += dgdyr;
				bEnd += dbdyr;

				ypos += resX;

				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dzdyl = (z2 - z1) * dyl;
						dudyl = (u2 - u1) * dyl;
						dvdyl = (v2 - v1) * dyl;
						
						dadyl = (a2 - a1) * dyl;
						drdyl = (r2 - r1) * dyl;
						dgdyl = (g2 - g1) * dyl;
						dbdyl = (b2 - b1) * dyl;

						xStart = x1 + dxdyl;
						zStart = z1 + dzdyl;
						uStart = u1 + dudyl;
						vStart = v1 + dvdyl;
						
						aStart = a1 + dadyl;
						rStart = r1 + drdyl;
						gStart = g1 + dgdyl;
						bStart = b1 + dbdyl;
					}
					else
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dzdyr = (z1 - z2) * dyr;
						dudyr = (u1 - u2) * dyr;
						dvdyr = (v1 - v2) * dyr;
						
						dadyr = (a1 - a2) * dyr;
						drdyr = (r1 - r2) * dyr;
						dgdyr = (g1 - g2) * dyr;
						dbdyr = (b1 - b2) * dyr;

						xEnd = x2 + dxdyr;
						zEnd = z2 + dzdyr;
						uEnd = u2 + dudyr;
						vEnd = v2 + dvdyr;
						
						aEnd = a2 + dadyr;
						rEnd = r2 + drdyr;
						gEnd = g2 + dgdyr;
						bEnd = b2 + dbdyr;
					}
				}
			}
		}
	}
}

# endif