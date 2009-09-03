//INLINE DWORD computePixelColor( float r, float g, float b, float a, float u, float v, Texture * texture )
//{
//	LPWORD ARGBBuffer = texture->ARGBBuffer;
//	int pos = ((int)(u * (texture->width - 1) + 0.5) + ( (int)(v * (texture->height - 1) + 0.5) * texture->width )) << 2;
//
//	a *= ARGBBuffer[pos];
//	r *= ARGBBuffer[++pos];
//	g *= ARGBBuffer[++pos];
//	b *= ARGBBuffer[++pos];
//
//	return ((int)a << 24) + ((int)r << 16) + ((int)g << 8) + (int)b;
//}

//INLINE DWORD computePixelColor2( float u, float v, Texture * texture )
//{
//	LPWORD ARGBBuffer = texture->ARGBBuffer;
//	int pos;
//
//	pos = ((int)(u * texture->width) + (int)(v * texture->height * texture->width )) << 2;
//
//	return ((int)ARGBBuffer[pos] << 24) + ((int)ARGBBuffer[++pos] << 16) + ((int)ARGBBuffer[++pos] << 8) + (int)ARGBBuffer[++pos];
//	return 0;
//}

INLINE void getMixedColor( WORD a, WORD r, WORD g, WORD b, int u, int v, int pos, Texture * texture, LPDWORD mixedChannel )
{
	int pos2 = (u + v) << 2;
	LPBYTE ARGBBuffer = texture->ARGBBuffer;

	a = ARGBBuffer[pos2];
	r += ARGBBuffer[++pos2];
	g += ARGBBuffer[++pos2];
	b += ARGBBuffer[++pos2];

	/*a >>= 8;
	r >>= 8;
	g >>= 8;
	b >>= 8;*/

	r = MIN( r, 255 );
	g = MIN( g, 255 );
	b = MIN( b, 255 );

	mixedChannel[pos] = (a << 24) + (r << 16) + (g << 8) + b;
}

INLINE void getPixelColor( WORD a, WORD r, WORD g, WORD b, int pos, LPDWORD mixedChannel)
{
	mixedChannel[pos] = (a << 24) + (r << 16) + (g << 8) + b;
}

INLINE void getPixelFromTexture( int u, int v, Texture * texture, int pos, LPDWORD mixedChannel)
{
	int pos2 = (u + v) << 2;
	LPBYTE ARGBBuffer = texture->ARGBBuffer;
	WORD a, r, g, b;

	a = ARGBBuffer[pos2];
	r = ARGBBuffer[++pos2];
	g = ARGBBuffer[++pos2];
	b = ARGBBuffer[++pos2];

	mixedChannel[pos] = (a << 24) + (r << 16) + (g << 8) + b;
}

//INLINE void mixedTexture( LPDWORD mixedChannel, LPWORD colorChannel, LPWORD textureChannel, int pWH )
//{
//	int i = 0, j = 0;
//	WORD a1 = 0, r1 = 0, g1 = 0, b1 = 0, a2 = 0, r2 = 0, g2 = 0, b2 = 0;
//
//	for ( ; i < pWH; i ++, j += 4 )
//	{
//		a1 = colorChannel[j];
//		r1 = colorChannel[j+1];
//		g1 = colorChannel[j+2];
//		b1 = colorChannel[j+3];
//
//		a2 = textureChannel[j];
//		r2 = textureChannel[j+1];
//		g2 = textureChannel[j+2];
//		b2 = textureChannel[j+3];
//
//		//if ( a1 == 0 && r1 == 0 && ) continue;
//
//		a1 += a2;
//		r1 += r2;
//		g1 += g2;
//		b1 += b2;
//
//		a1 = MIN( a1, 255 );
//		r1 = MIN( r1, 255 );
//		g1 = MIN( g1, 255 );
//		b1 = MIN( b1, 255 );
//
//		mixedChannel[i] = (a1 << 24) + (r1 << 16) + (g1 << 8) + b1;
//	}
//}
//
//void wireframe_rasterize( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2 )
//{
//	Vertex	* tmpV;
//	float			* zBuffer = view->zBuffer;
//
//	int				resX = (int)view->width, resY = (int)view->height;
//	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
//	float			z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, currZ, dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dzdx, temp2;
//	WORD			a, r, g, b;
//
//	if (((ver0->s_pos->y < 0) && (ver1->s_pos->y < 0) && (ver2->s_pos->y < 0)) ||
//		((ver0->s_pos->y > resY) && (ver1->s_pos->y > resY) && (ver2->s_pos->y > resY)) ||
//		((ver0->s_pos->x < 0) && (ver1->s_pos->x < 0) && (ver2->s_pos->x < 0)) ||
//		((ver0->s_pos->x > resX) && (ver1->s_pos->x > resX) && (ver2->s_pos->x > resX)) ||
//		((ver0->s_pos->z > 1) && (ver1->s_pos->z > 1) && (ver2->s_pos->z > 1)) ||
//		((ver0->s_pos->z < 0 ) && (ver1->s_pos->z < 0) && (ver2->s_pos->z < 0)))
//	{
//		return;
//	}
//
//	//判断是否是一条线，如果是，则返回。
//	if (((ver0->s_pos->x == ver1->s_pos->x) && (ver1->s_pos->x == ver2->s_pos->x)) || ((ver0->s_pos->y == ver1->s_pos->y) && (ver1->s_pos->y == ver2->s_pos->y)))
//	{
//		return;
//	}
//
//	//*************************************** start draw ************************************
//	nw = resX - 1;
//	nh = resY - 1;
//
//	//调整y0,y1,y2,让它们的y值从小到大
//	if (ver1->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver0;
//		ver0 = tmpV;
//	}
//	if (ver2->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver0;
//		ver0 = ver2;
//		ver2 = tmpV;
//	}
//	if (ver2->s_pos->y < ver1->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver2;
//		ver2 = tmpV;
//	}
//
//	//判断三角形的类型
//	if (ver0->s_pos->y == ver1->s_pos->y)
//	{
//		tri_type = TRI_TYPE_FLAT_TOP;
//
//		if (ver1->s_pos->x < ver0->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver0;
//			ver0 = tmpV;
//		}
//	}
//	else if (ver1->s_pos->y == ver2->s_pos->y)
//	{
//		tri_type = TRI_TYPE_FLAT_BOTTOM;
//
//		if (ver2->s_pos->x < ver1->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver2;
//			ver2 = tmpV;
//		}
//	}
//	else
//	{
//		tri_type = TRI_TYPE_GENERAL;
//	}
//
//	x0 = (int)ver0->s_pos->x;
//	y0 = (int)ver0->s_pos->y;
//	z0 = ver0->s_pos->z;
//
//	x1 = (int)ver1->s_pos->x;
//	y1 = (int)ver1->s_pos->y;
//	z1 = ver1->s_pos->z;
//
//	x2 = (int)ver2->s_pos->x;
//	y2 = (int)ver2->s_pos->y;
//	z2 = ver2->s_pos->z;
//
//	a = ver0->color->alpha;
//	r = ver0->color->red;
//	g = ver0->color->green;
//	b = ver0->color->blue;
//
//	//转折后的斜边在哪一侧
//	side = 0;
//
//	//转折点y坐标
//	ys = y1;
//
//	if (tri_type == TRI_TYPE_FLAT_TOP || tri_type == TRI_TYPE_FLAT_BOTTOM)
//	{
//		if (tri_type == TRI_TYPE_FLAT_TOP)
//		{
//			dy = 1.0f / (y2 - y0);
//
//			dxdyl = (x2 - x0) * dy;	//左斜边倒数
//			dzdyl = (z2 - z0) * dy;
//
//			dxdyr = (x2 - x1) * dy;	//右斜边倒数
//			dzdyr = (z2 - z1) * dy;
//
//			//y0小于视窗顶部y坐标，调整左斜边和右斜边当前值,y值开始值
//			if (y0 < 0)
//			{
//				xStart = x0 - dxdyl * y0;
//				zStart = z0 - dzdyl * y0;
//
//				xEnd = x1 - dxdyr * y0;
//				zEnd = z1 - dzdyr * y0;
//
//				yStart = 0;
//			}
//			else
//			{
//				//注意平顶和平底这里的区别
//				xStart = (float)x0;
//				zStart = z0;
//
//				xEnd = (float)x1;
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
//
//			dxdyr = (x2 - x0) * dy;
//			dzdyr = (z2 - z0) * dy;
//
//			if (y0 < 0)
//			{
//				dy = - (float)y0;
//
//				xStart = dxdyl * dy + x0;
//				zStart = dzdyl * dy + z0;
//
//				xEnd = dxdyr * dy + x0;
//				zEnd = dzdyr * dy + z0;
//
//				yStart = 0.0f;
//			}
//			else
//			{
//				xStart = (float)x0;
//				zStart = z0;
//
//				xEnd = (float)x0;
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
//				if ( cxEnd >= 0 && cxStart <= nw )
//				{
//					if (cxStart == cxEnd)
//					{
//						pos = cxStart + ypos;
//
//						if( zStart < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zStart;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						currZ = zStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//
//							cxStart = 0;
//						}
//						//绘制扫描线
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( ( xi == cxStart || xi == cxEnd || yi == cyStart || yi == cyEnd ) && currZ < zBuffer[pos] )
//							{
//								getPixelColor( a, r, g, b, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				//y每增加1时,xl和xr分别加上他们的递增量
//				xStart += dxdyl;
//				zStart += dzdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
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
//
//					if (cxStart == cxEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( ( xi == cxStart || xi == cxEnd || yi == cyStart || yi == cyEnd ) && currZ < zBuffer[pos] )
//							{
//								getPixelColor( a, r, g, b, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
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
//
//			dxdyr = (x2 - x0) * dyr;
//			dzdyr = (z2 - z0) * dyr;
//
//			//计算左右斜边初始值
//			dyr = - (float)y0;
//			dyl = - (float)y1;
//
//			xStart = dxdyl * dyl + x1;
//			zStart = dzdyl * dyl + z1;
//
//			xEnd = dxdyr * dyr + x0;
//			zEnd = dzdyr * dyr + z0;
//
//			yStart = 0;
//
//			if (dxdyr > dxdyl)
//			{
//				//交换斜边
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
//				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
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
//
//			dzdyr = (z2 - z0) * dyr;
//
//			dy = - (float)y0;
//
//			xStart = dxdyl * dy + x0;
//			zStart = dzdyl * dy + z0;
//
//			xEnd = dxdyr * dy + x0;
//			zEnd = dzdyr * dy + z0;
//
//			yStart = 0;
//
//			if (dxdyr < dxdyl)
//			{
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
//				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
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
//
//			dxdyr = (x2 - x0) * dyr;
//			dzdyr = (z2 - z0) * dyr;
//
//			xStart = (float)x0;
//			zStart = z0;
//
//			xEnd = (float)x0;
//			zEnd = z0;
//
//			yStart = (float)y0;
//
//			if (dxdyr < dxdyl)
//			{
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
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
//						if( zStart < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zStart;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						currZ = zStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//
//							cxStart = 0;
//						}
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( ( xi == cxStart || xi == cxEnd || yi == cyStart || yi == cyEnd ) && currZ < zBuffer[pos] )
//							{
//								getPixelColor( a, r, g, b, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
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
//
//						xStart = (float)x1;
//						zStart = z1;
//					}
//					else
//					{
//						dyr = 1.0f / (y1 - y2);
//
//						dxdyr = (x1 - x2) * dyr;
//						dzdyr = (z1 - z2) * dyr;
//
//						xEnd = (float)x2;
//						zEnd = z2;
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
//
//					if (xStart == xEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( ( xi == cxStart || xi == cxEnd || yi == cyStart || yi == cyEnd ) && currZ < zBuffer[pos] )
//							{
//								getPixelColor( a, r, g, b, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
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
//
//						xStart = x1 + dxdyl;
//						zStart = z1 + dzdyl;
//					}
//					else
//					{
//						dyr = 1.0f / (y1 - y2);
//
//						dxdyr = (x1 - x2) * dyr;
//						dzdyr = (z1 - z2) * dyr;
//
//						xEnd = x2 + dxdyr;
//						zEnd = z2 + dzdyr;
//					}
//				}
//			}
//		}
//	}
//}
//
//
//void triangle_rasterize( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2 )
//{
//	Vertex	* tmpV;
//	float			* zBuffer = view->zBuffer;
//
//	int				resX = (int)view->width, resY = (int)view->height;
//	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
//	float			z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, currZ, dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dzdx, temp2;
//	WORD			a, r, g, b;
//
//	if (((ver0->s_pos->y < 0) && (ver1->s_pos->y < 0) && (ver2->s_pos->y < 0)) ||
//		((ver0->s_pos->y > resY) && (ver1->s_pos->y > resY) && (ver2->s_pos->y > resY)) ||
//		((ver0->s_pos->x < 0) && (ver1->s_pos->x < 0) && (ver2->s_pos->x < 0)) ||
//		((ver0->s_pos->x > resX) && (ver1->s_pos->x > resX) && (ver2->s_pos->x > resX)) ||
//		((ver0->s_pos->z > 1) && (ver1->s_pos->z > 1) && (ver2->s_pos->z > 1)) ||
//		((ver0->s_pos->z < 0 ) && (ver1->s_pos->z < 0) && (ver2->s_pos->z < 0)))
//	{
//		return;
//	}
//
//	//判断是否是一条线，如果是，则返回。
//	if (((ver0->s_pos->x == ver1->s_pos->x) && (ver1->s_pos->x == ver2->s_pos->x)) || ((ver0->s_pos->y == ver1->s_pos->y) && (ver1->s_pos->y == ver2->s_pos->y)))
//	{
//		return;
//	}
//
//	//*************************************** start draw ************************************
//	nw = resX - 1;
//	nh = resY - 1;
//
//	//调整y0,y1,y2,让它们的y值从小到大
//	if (ver1->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver0;
//		ver0 = tmpV;
//	}
//	if (ver2->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver0;
//		ver0 = ver2;
//		ver2 = tmpV;
//	}
//	if (ver2->s_pos->y < ver1->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver2;
//		ver2 = tmpV;
//	}
//
//	//判断三角形的类型
//	if (ver0->s_pos->y == ver1->s_pos->y)
//	{
//		tri_type = TRI_TYPE_FLAT_TOP;
//
//		if (ver1->s_pos->x < ver0->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver0;
//			ver0 = tmpV;
//		}
//	}
//	else if (ver1->s_pos->y == ver2->s_pos->y)
//	{
//		tri_type = TRI_TYPE_FLAT_BOTTOM;
//
//		if (ver2->s_pos->x < ver1->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver2;
//			ver2 = tmpV;
//		}
//	}
//	else
//	{
//		tri_type = TRI_TYPE_GENERAL;
//	}
//
//	x0 = (int)ver0->s_pos->x;
//	y0 = (int)ver0->s_pos->y;
//	z0 = ver0->s_pos->z;
//
//	x1 = (int)ver1->s_pos->x;
//	y1 = (int)ver1->s_pos->y;
//	z1 = ver1->s_pos->z;
//
//	x2 = (int)ver2->s_pos->x;
//	y2 = (int)ver2->s_pos->y;
//	z2 = ver2->s_pos->z;
//
//	a = ver0->color->alpha;
//	r = ver0->color->red;
//	g = ver0->color->green;
//	b = ver0->color->blue;
//
//	//转折后的斜边在哪一侧
//	side = 0;
//
//	//转折点y坐标
//	ys = y1;
//
//	if (tri_type == TRI_TYPE_FLAT_TOP || tri_type == TRI_TYPE_FLAT_BOTTOM)
//	{
//		if (tri_type == TRI_TYPE_FLAT_TOP)
//		{
//			dy = 1.0f / (y2 - y0);
//
//			dxdyl = (x2 - x0) * dy;	//左斜边倒数
//			dzdyl = (z2 - z0) * dy;
//
//			dxdyr = (x2 - x1) * dy;	//右斜边倒数
//			dzdyr = (z2 - z1) * dy;
//
//			//y0小于视窗顶部y坐标，调整左斜边和右斜边当前值,y值开始值
//			if (y0 < 0)
//			{
//				xStart = x0 - dxdyl * y0;
//				zStart = z0 - dzdyl * y0;
//
//				xEnd = x1 - dxdyr * y0;
//				zEnd = z1 - dzdyr * y0;
//
//				yStart = 0;
//			}
//			else
//			{
//				//注意平顶和平底这里的区别
//				xStart = (float)x0;
//				zStart = z0;
//
//				xEnd = (float)x1;
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
//
//			dxdyr = (x2 - x0) * dy;
//			dzdyr = (z2 - z0) * dy;
//
//			if (y0 < 0)
//			{
//				dy = - (float)y0;
//
//				xStart = dxdyl * dy + x0;
//				zStart = dzdyl * dy + z0;
//
//				xEnd = dxdyr * dy + x0;
//				zEnd = dzdyr * dy + z0;
//
//				yStart = 0.0f;
//			}
//			else
//			{
//				xStart = (float)x0;
//				zStart = z0;
//
//				xEnd = (float)x0;
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
//				if ( cxEnd >= 0 && cxStart <= nw )
//				{
//					if (cxStart == cxEnd)
//					{
//						pos = cxStart + ypos;
//
//						if( zStart > 0 && zStart < 1 && zStart < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zStart;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						currZ = zStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//
//							cxStart = 0;
//						}
//						//绘制扫描线
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getPixelColor( a, r, g, b, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				//y每增加1时,xl和xr分别加上他们的递增量
//				xStart += dxdyl;
//				zStart += dzdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
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
//
//					if (cxStart == cxEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getPixelColor( a, r, g, b, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
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
//
//			dxdyr = (x2 - x0) * dyr;
//			dzdyr = (z2 - z0) * dyr;
//
//			//计算左右斜边初始值
//			dyr = - (float)y0;
//			dyl = - (float)y1;
//
//			xStart = dxdyl * dyl + x1;
//			zStart = dzdyl * dyl + z1;
//
//			xEnd = dxdyr * dyr + x0;
//			zEnd = dzdyr * dyr + z0;
//
//			yStart = 0;
//
//			if (dxdyr > dxdyl)
//			{
//				//交换斜边
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
//				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
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
//
//			dzdyr = (z2 - z0) * dyr;
//
//			dy = - (float)y0;
//
//			xStart = dxdyl * dy + x0;
//			zStart = dzdyl * dy + z0;
//
//			xEnd = dxdyr * dy + x0;
//			zEnd = dzdyr * dy + z0;
//
//			yStart = 0;
//
//			if (dxdyr < dxdyl)
//			{
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
//				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
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
//
//			dxdyr = (x2 - x0) * dyr;
//			dzdyr = (z2 - z0) * dyr;
//
//			xStart = (float)x0;
//			zStart = z0;
//
//			xEnd = (float)x0;
//			zEnd = z0;
//
//			yStart = (float)y0;
//
//			if (dxdyr < dxdyl)
//			{
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
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
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zStart;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						currZ = zStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//
//							cxStart = 0;
//						}
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getPixelColor( a, r, g, b, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
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
//
//						xStart = (float)x1;
//						zStart = z1;
//					}
//					else
//					{
//						dyr = 1.0f / (y1 - y2);
//
//						dxdyr = (x1 - x2) * dyr;
//						dzdyr = (z1 - z2) * dyr;
//
//						xEnd = (float)x2;
//						zEnd = z2;
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
//
//					if (xStart == xEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getPixelColor( a, r, g, b, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getPixelColor( a, r, g, b, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
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
//
//						xStart = x1 + dxdyl;
//						zStart = z1 + dzdyl;
//					}
//					else
//					{
//						dyr = 1.0f / (y1 - y2);
//
//						dxdyr = (x1 - x2) * dyr;
//						dzdyr = (z1 - z2) * dyr;
//
//						xEnd = x2 + dxdyr;
//						zEnd = z2 + dzdyr;
//					}
//				}
//			}
//		}
//	}
//}
//
//void triangle_rasterize_texture( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2, Texture * texture )
//{
//	Vertex	* tmpV;
//	float	* zBuffer = view->zBuffer;
//
//	int		resX = (int)view->width, resY = (int)view->height;
//	int		nw, nh, side, ys, xi, yi, xStart, xEnd, yStart, yEnd, tri_type, pos, temp, ypos;
//	int		x0, y0, x1, y1, x2, y2, u0, v0, u1, v1, u2, v2, ui, vi;
//	int		z0, z1, z2, xLeft, xRight, zLeft, zRight, uLeft, uRight, vLeft, vRight, zi,
//			dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dudyl, dudyr, dvdyl, dvdyr, dzdx, dudx, dvdx, temp2;
//	WORD	a, r, g, b;
//
//	if (((ver0->s_pos->y < 0) && (ver1->s_pos->y < 0) && (ver2->s_pos->y < 0)) ||
//		((ver0->s_pos->y > resY) && (ver1->s_pos->y > resY) && (ver2->s_pos->y > resY)) ||
//		((ver0->s_pos->x < 0) && (ver1->s_pos->x < 0) && (ver2->s_pos->x < 0)) ||
//		((ver0->s_pos->x > resX) && (ver1->s_pos->x > resX) && (ver2->s_pos->x > resX)) ||
//		((ver0->s_pos->z > 1) && (ver1->s_pos->z > 1) && (ver2->s_pos->z > 1)) ||
//		((ver0->s_pos->z < 0 ) && (ver1->s_pos->z < 0) && (ver2->s_pos->z < 0)))
//	{
//		return;
//	}
//
//	//判断是否是一条线，如果是，则返回。
//	if ( ( FCMP( ver0->s_pos->x, ver1->s_pos->x ) &&
//		FCMP( ver1->s_pos->x, ver2->s_pos->x) ) ||
//		( FCMP( ver0->s_pos->y, ver1->s_pos->y ) &&
//		FCMP( ver1->s_pos->y, ver2->s_pos->y ) ) )
//	{
//		return;
//	}
//
//	//*************************************** start draw ************************************
//	nw = resX - 1;
//	nh = resY - 1;
//
//	//调整y0,y1,y2,让它们的y值从小到大
//	if (ver1->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver0;
//		ver0 = tmpV;
//	}
//	if (ver2->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver0;
//		ver0 = ver2;
//		ver2 = tmpV;
//	}
//	if (ver2->s_pos->y < ver1->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver2;
//		ver2 = tmpV;
//	}
//
//	//判断三角形的类型
//	if ( FCMP( ver0->s_pos->y, ver1->s_pos->y ) )
//	{
//		tri_type = TRI_TYPE_FLAT_TOP;
//
//		if (ver1->s_pos->x < ver0->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver0;
//			ver0 = tmpV;
//		}
//	}
//	else if ( FCMP( ver1->s_pos->y, ver2->s_pos->y ) )
//	{
//		tri_type = TRI_TYPE_FLAT_BOTTOM;
//
//		if (ver2->s_pos->x < ver1->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver2;
//			ver2 = tmpV;
//		}
//	}
//	else
//	{
//		tri_type = TRI_TYPE_GENERAL;
//	}
//
//	x0 = (int)( ver0->s_pos->x + 0.5 );
//	y0 = (int)( ver0->s_pos->y + 0.5 );
//	z0 = ver0->s_pos->z;
//
//	x1 = (int)( ver1->s_pos->x + 0.5 );
//	y1 = (int)( ver1->s_pos->y + 0.5 );
//	z1 = ver1->s_pos->z;
//
//	x2 = (int)( ver2->s_pos->x + 0.5 );
//	y2 = (int)( ver2->s_pos->y + 0.5 );
//	z2 = ver2->s_pos->z;
//
//	u0 = (int)( ver0->uv->x * (texture->width - 1) );
//	v0 = (int)( ver0->uv->y * (texture->height - 1) );
//	u1 = (int)( ver1->uv->x * (texture->width - 1) );
//	v1 = (int)( ver1->uv->y * (texture->height - 1) );
//	u2 = (int)( ver2->uv->x * (texture->width - 1) );
//	v2 = (int)( ver2->uv->y * (texture->height - 1) );
//
//	a = (WORD)ver0->color->alpha;
//	r = (WORD)ver0->color->red;
//	g = (WORD)ver0->color->green;
//	b = (WORD)ver0->color->blue;
//
//	side = 0;
//
//	//转折后的斜边在哪一侧
//	ys = y1;
//
//	//转折点y坐标
//	if (tri_type == TRI_TYPE_FLAT_TOP || tri_type == TRI_TYPE_FLAT_BOTTOM)
//	{
//		//平顶三角形
//		if (tri_type == TRI_TYPE_FLAT_TOP)
//		{
//			dy = 1.0f / (y2 - y0);
//
//			dxdyl = ( (x2 - x0) << FIXP16_SHIFT ) * dy;	//左斜边倒数
//			dxdyr = ( (x2 - x1) << FIXP16_SHIFT ) * dy;	//右斜边倒数
//
//			dudyl = ( (u2 - u0) << FIXP16_SHIFT ) * dy;
//			dudyr = ( (u2 - u1) << FIXP16_SHIFT ) * dy;
//
//			dvdyl = ( (v2 - v0) << FIXP16_SHIFT ) * dy;
//			dzdyr = ( (z2 - z1) << FIXP16_SHIFT ) * dy;
//
//			dzdyl = (z2 - z0) * dy;
//			dvdyr = (v2 - v1) * dy;
//
//			//y0小于视窗顶部y坐标，调整左斜边和右斜边当前值,y值开始值
//			if ( y0 < 0 )
//			{
//				dy = - y0;
//
//				xLeft = dxdyl * dy + ( x0 << FIXP16_SHIFT );
//				xRight = dxdyr * dy + ( x1 << FIXP16_SHIFT );
//
//				uLeft = dudyl * dy + ( u0 << FIXP16_SHIFT );
//				uRight = dudyr * dy + ( u1 << FIXP16_SHIFT );
//
//				vLeft = dvdyl * dy + ( v0 << FIXP16_SHIFT );
//				vRight = dvdyr * dy + ( v1 << FIXP16_SHIFT );
//
//				zLeft = dzdyl * dy + z0;
//				zRight = dzdyr * dy + z1;
//
//				yStart = 0;
//			}
//			else
//			{
//				//注意平顶和平底这里的区别
//				xLeft = ( x0 << FIXP16_SHIFT );
//				xRight = ( x1 << FIXP16_SHIFT );
//
//				uLeft = ( u0 << FIXP16_SHIFT );
//				uRight = ( u1 << FIXP16_SHIFT );
//
//				vLeft = ( v0 << FIXP16_SHIFT );
//				vRight = ( v1 << FIXP16_SHIFT );
//
//				zLeft = z0;
//				zRight = z1;
//
//				yStart = y0;
//			}
//		}
//		//平底三角形
//		else
//		{
//			dy = 1.0f / (y1 - y0);
//
//			dxdyl = ( (x1 - x0) << FIXP16_SHIFT ) * dy;
//			dxdyr = ( (x2 - x0) << FIXP16_SHIFT ) * dy;
//
//			dudyl = ( (u1 - u0) << FIXP16_SHIFT ) * dy;
//			dudyr = ( (u2 - u0) << FIXP16_SHIFT ) * dy;
//
//			dvdyl = ( (v1 - v0) << FIXP16_SHIFT ) * dy;
//			dvdyr = ( (v2 - v0) << FIXP16_SHIFT ) * dy;
//
//			dzdyl = (z1 - z0) * dy;
//			dzdyr = (z2 - z0) * dy;
//
//			if (y0 < 0)
//			{
//				dy = - y0;
//
//				xLeft = dxdyl * dy + ( x0 << FIXP16_SHIFT );
//				xRight = dxdyr * dy + ( x0 << FIXP16_SHIFT );
//
//				uLeft = dudyl * dy + ( u0 << FIXP16_SHIFT );
//				uRight = dudyr * dy + ( u0 << FIXP16_SHIFT );;
//
//				vLeft = dvdyl * dy + ( v0 << FIXP16_SHIFT );
//				vRight = dvdyr * dy + ( v0 << FIXP16_SHIFT );
//
//				zLeft = dzdyl * dy + z0;
//				zRight = dzdyr * dy + z0;
//
//				yStart = 0;
//			}
//			else
//			{
//				//修正开始位置
//				xLeft = x0 << FIXP16_SHIFT;
//				xRight = x0 << FIXP16_SHIFT;
//
//				uLeft = u0 << FIXP16_SHIFT;
//				uRight = u0 << FIXP16_SHIFT;
//
//				vLeft = v0 << FIXP16_SHIFT;
//				vRight = v0 << FIXP16_SHIFT;
//
//				zLeft = z0;
//				zRight = z0;
//
//				yStart = y0;
//			}
//		}
//
//		//y2>视窗高度时，大于nh部分就不用画出了
//		yEnd = y2 > nh ? nh : y2;
//		ypos = yStart * resX;
//
//		//x值需要裁剪的情况
//		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
//		{
//			for ( yi = yStart; yi <= yEnd; yi ++, ypos += resX )
//			{
//				xStart = ((xLeft + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
//				xEnd = ((xRight + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
//
//				ui = uLeft + FIXP16_ROUND_UP;
//				vi = vLeft + FIXP16_ROUND_UP;
//
//				zi = zLeft;
//
//				//判断x开始和结束点是否在屏幕范围内
//				if ( xEnd >= 0 && xStart <= nw )
//				{
//					if ( xEnd == xStart )
//					{
//						pos = xStart + ypos;
//
//						if( zLeft < zBuffer[pos] )
//						{
//							getMixedColor( a, r, g, b, (int)(ui + 0.5), (int)(vi + 0.5) * texture->width, pos, texture, view->mixedChannel );
//							zBuffer[pos] = zLeft;
//						}
//					}
//					else
//					{
//						dx = 1 / (xEnd - xStart);
//
//						if ( dx > 0 )
//						{
//							dudx = (uRight - uLeft) * dx;
//							dvdx = (vRight - vLeft) * dx;
//						}
//						else
//						{
//							dudx = (ur - ul);
//							dvdx = (vr - vl);
//						}
//
//						dzdx = (zRight - zLeft) * dx;
//						dudx = (uRight - uLeft) * dx;
//						dvdx = (vRight - vLeft) * dx;
//
//						currZ = zLeft + FIXP16_ROUND_UP;
//						currU = uLeft + FIXP16_ROUND_UP;
//						currV = vLeft + FIXP16_ROUND_UP;
//
//						//初始值需要裁剪
//						if ( xStart < 0 )
//						{
//							currZ -= xStart * dzdx;
//							currU -= xStart * dudx;
//							currV -= xStart * dvdx;
//
//							xStart = 0;
//						}
//
//						//绘制扫描线
//						for ( xi = xStart; xi <= xEnd; xi ++ )
//						{
//							pos = xi + ypos;
//
//							if( currZ < zBuffer[pos] )
//							{
//								getMixedColor( a, r, g, b, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//						}
//
//						pos = cxRight + ypos;
//						if( zRight > 0 && zRight < 1 && zRight < zBuffer[pos] )
//						{
//							getMixedColor( a, r, g, b, (int)(uRight + 0.5), (int)(vRight + 0.5) * texture->width, pos, texture, view->mixedChannel );
//							zBuffer[pos] = zRight;
//						}
//					}
//				}
//
//				//y每增加1时,xl和xr分别加上他们的递增量
//				xLeft += dxdyl;
//				zLeft += dzdyl;
//				uLeft += dudyl;
//				vLeft += dvdyl;
//
//				xRight += dxdyr;
//				zRight += dzdyr;
//				uRight += dudyr;
//				vRight += dvdyr;
//			}
//		}
//		else
//		{
//			//x不需要裁剪的情况
//			for ( yi = cyStart; yi <= cyEnd; yi ++, ypos += resX )
//			{
//				if ( zLeft > 0 || zRight > 0 )
//				{
//					cxLeft = (int)xLeft;
//					cxRight = (int)xRight;
//
//					currZ = zLeft;
//					currU = uLeft;
//					currV = vLeft;
//
//					if (cxLeft == cxRight)
//					{
//						pos = cxLeft + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							getMixedColor( a, r, g, b, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xRight - xLeft);
//
//						dzdx = (zRight - zLeft) * dx;
//						dudx = (uRight - uLeft) * dx;
//						dvdx = (vRight - vLeft) * dx;
//
//						for ( xi = cxLeft; xi < cxRight; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getMixedColor( a, r, g, b, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//						}
//
//						pos = cxRight + ypos;
//						if( zRight > 0 && zRight < 1 && zRight < zBuffer[pos] )
//						{
//							getMixedColor( a, r, g, b, (int)(uRight + 0.5), (int)(vRight + 0.5) * texture->width, pos, texture, view->mixedChannel );
//							zBuffer[pos] = zRight;
//						}
//					}
//				}
//
//				xLeft += dxdyl;
//				zLeft += dzdyl;
//				uLeft += dudyl;
//				vLeft += dvdyl;
//
//				xRight += dxdyr;
//				zRight += dzdyr;
//				uRight += dudyr;
//				vRight += dvdyr;
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
//			xLeft = dxdyl * dyl + x1;
//			zLeft = dzdyl * dyl + z1;
//			uLeft = dudyl * dyl + u1;
//			vLeft = dvdyl * dyl + v1;
//
//			xRight = dxdyr * dyr + x0;
//			zRight = dzdyr * dyr + z0;
//			uRight = dudyr * dyr + u0;
//			vRight = dvdyr * dyr + v0;
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
//				temp2 = xLeft;	xLeft = xRight;	xRight = temp2;
//				temp2 = zLeft;	zLeft = zRight;	zRight = temp2;
//				temp2 = uLeft;	uLeft = uRight;	uRight = temp2;
//				temp2 = vLeft;	vLeft = vRight;	vRight = temp2;
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
//			xLeft = dxdyl * dy + x0;
//			zLeft = dzdyl * dy + z0;
//			uLeft = dudyl * dy + u0;
//			vLeft = dvdyl * dy + v0;
//
//			xRight = dxdyr * dy + x0;
//			zRight = dzdyr * dy + z0;
//			uRight = dudyr * dy + u0;
//			vRight = dvdyr * dy + v0;
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
//				temp2 = xLeft;	xLeft = xRight;	xRight = temp2;
//				temp2 = zLeft;	zLeft = zRight;	zRight = temp2;
//				temp2 = uLeft;	uLeft = uRight;	uRight = temp2;
//				temp2 = vLeft;	vLeft = vRight;	vRight = temp2;
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
//			xLeft = (float)x0;
//			zLeft = z0;
//			uLeft = u0;
//			vLeft = v0;
//
//			xRight = (float)x0;
//			zRight = z0;
//			uRight = u0;
//			vRight = v0;
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
//				cxLeft = (int)xLeft;
//				cxRight = xRight > nw ? nw :(int)xRight;
//
//				if ( cxRight >= 0 && cxLeft <= nw && ( ( zLeft > 0 && zLeft < 1 )  || ( zRight > 0 && zRight < 1 ) ) )
//				{
//					if ( cxLeft == cxRight )
//					{
//						pos = cxLeft + ypos;
//
//						if( zLeft > 0 && zLeft < 1 && zLeft < zBuffer[pos] )
//						{
//							getMixedColor( a, r, g, b, (int)(uLeft + 0.5), (int)(vLeft + 0.5) * texture->width, pos, texture, view->mixedChannel );
//							zBuffer[pos] = zLeft;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xRight - xLeft);
//
//						dzdx = (zRight - zLeft) * dx;
//						dudx = (uRight - uLeft) * dx;
//						dvdx = (vRight - vLeft) * dx;
//
//						currZ = zLeft;
//						currU = uLeft;
//						currV = vLeft;
//
//						//初始值需要裁剪
//						if (cxLeft < 0)
//						{
//							currZ -= cxLeft * dzdx;
//							currU -= cxLeft * dudx;
//							currV -= cxLeft * dvdx;
//
//							cxLeft = 0;
//						}
//
//						for ( xi = cxLeft; xi < cxRight; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getMixedColor( a, r, g, b, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//						}
//
//						pos = cxRight + ypos;
//						if( zRight > 0 && zRight < 1 && zRight < zBuffer[pos] )
//						{
//							getMixedColor( a, r, g, b, (int)(uRight + 0.5), (int)(vRight + 0.5) * texture->width, pos, texture, view->mixedChannel );
//							zBuffer[pos] = zRight;
//						}
//					}
//				}
//
//				xLeft += dxdyl;
//				zLeft += dzdyl;
//				uLeft += dudyl;
//				vLeft += dvdyl;
//
//				xRight += dxdyr;
//				zRight += dzdyr;
//				uRight += dudyr;
//				vRight += dvdyr;
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
//						xLeft = (float)x1;
//						zLeft = z1;
//						uLeft = u1;
//						vLeft = v1;
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
//						xRight = (float)x2;
//						zRight = z2;
//						uRight = u2;
//						vRight = v2;
//					}
//				}
//			}
//		}
//		else
//		{
//			//不需要裁剪
//			for ( yi = cyStart; yi <= cyEnd; yi ++ )
//			{
//				if ( zLeft > 0 || zRight > 0 )
//				{
//					cxLeft = (int)xLeft;
//					cxRight = (int)xRight;
//
//					currZ = zLeft;
//					currU = uLeft;
//					currV = vLeft;
//
//					if (xLeft == xRight)
//					{
//						pos = cxLeft + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							getMixedColor( a, r, g, b, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xRight - xLeft);
//
//						dzdx = (zRight - zLeft) * dx;
//						dudx = (uRight - uLeft) * dx;
//						dvdx = (vRight - vLeft) * dx;
//
//						for ( xi = cxLeft; xi < cxRight; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getMixedColor( a, r, g, b, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//						}
//
//						pos = cxRight + ypos;
//						if( zRight > 0 && zRight < 1 && zRight < zBuffer[pos] )
//						{
//							getMixedColor( a, r, g, b, (int)(uRight + 0.5), (int)(vRight + 0.5) * texture->width, pos, texture, view->mixedChannel );
//							zBuffer[pos] = zRight;
//						}
//					}
//				}
//
//				xLeft += dxdyl;
//				zLeft += dzdyl;
//				uLeft += dudyl;
//				vLeft += dvdyl;
//
//				xRight += dxdyr;
//				zRight += dzdyr;
//				uRight += dudyr;
//				vRight += dvdyr;
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
//						xLeft = x1 + dxdyl;
//						zLeft = z1 + dzdyl;
//						uLeft = u1 + dudyl;
//						vLeft = v1 + dvdyl;
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
//						xRight = x2 + dxdyr;
//						zRight = z2 + dzdyr;
//						uRight = u2 + dudyr;
//						vRight = v2 + dvdyr;
//					}
//				}
//			}
//		}
//	}
//}
//
//
//void triangle_rasterize_lightOff_texture( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2, Texture * texture )
//{
//	Vertex	* tmpV;
//	LPDWORD			colorChannel = view->colorChannel;
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
//		if ( ( ver2->s_pos->x - ver0->s_pos->x ) * ( ver1->s_pos->y - ver2->s_pos->y ) > ( ver2->s_pos->y - ver0->s_pos->y ) * ( ver1->s_pos->x - ver2->s_pos->x ) )
//			return;
//	}
//
//	if (((ver0->s_pos->y < 0) && (ver1->s_pos->y < 0) && (ver2->s_pos->y < 0)) ||
//		((ver0->s_pos->y > resY) && (ver1->s_pos->y > resY) && (ver2->s_pos->y > resY)) ||
//		((ver0->s_pos->x < 0) && (ver1->s_pos->x < 0) && (ver2->s_pos->x < 0)) ||
//		((ver0->s_pos->x > resX) && (ver1->s_pos->x > resX) && (ver2->s_pos->x > resX)))
//	{
//		return;
//	}
//
//	//判断是否是一条线，如果是，则返回。
//	if (((ver0->s_pos->x == ver1->s_pos->x) && (ver1->s_pos->x == ver2->s_pos->x)) || ((ver0->s_pos->y == ver1->s_pos->y) && (ver1->s_pos->y == ver2->s_pos->y)))
//	{
//		return;
//	}
//
//	//*************************************** start draw ************************************
//	nw = resX - 1;
//	nh = resY - 1;
//
//	//调整y0,y1,y2,让它们的y值从小到大
//	if (ver1->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver0;
//		ver0 = tmpV;
//	}
//	if (ver2->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver0;
//		ver0 = ver2;
//		ver2 = tmpV;
//	}
//	if (ver2->s_pos->y < ver1->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver2;
//		ver2 = tmpV;
//	}
//
//	//判断三角形的类型
//	if (ver0->s_pos->y == ver1->s_pos->y)
//	{
//		tri_type = TRI_TYPE_FLAT_TOP;
//
//		if (ver1->s_pos->x < ver0->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver0;
//			ver0 = tmpV;
//		}
//	}
//	else if (ver1->s_pos->y == ver2->s_pos->y)
//	{
//		tri_type = TRI_TYPE_FLAT_BOTTOM;
//
//		if (ver2->s_pos->x < ver1->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver2;
//			ver2 = tmpV;
//		}
//	}
//	else
//	{
//		tri_type = TRI_TYPE_GENERAL;
//	}
//
//	x0 = (int)ver0->s_pos->x;
//	y0 = (int)ver0->s_pos->y;
//	z0 = ver0->s_pos->z;
//
//	x1 = (int)ver1->s_pos->x;
//	y1 = (int)ver1->s_pos->y;
//	z1 = ver1->s_pos->z;
//
//	x2 = (int)ver2->s_pos->x;
//	y2 = (int)ver2->s_pos->y;
//	z2 = ver2->s_pos->z;
//
//	u0 = ver0->uv->x;
//	v0 = ver0->uv->y;
//	u1 = ver1->uv->x;
//	v1 = ver1->uv->y;
//	u2 = ver2->uv->x;
//	v2 = ver2->uv->y;
//
//	a = ver0->color->alpha;
//	r = ver0->color->red;
//	g = ver0->color->green;
//	b = ver0->color->blue;
//
//	side = 0;
//
//	//转折后的斜边在哪一侧
//	ys = y1;
//
//	//转折点y坐标
//	if (tri_type == TRI_TYPE_FLAT_TOP || tri_type == TRI_TYPE_FLAT_BOTTOM)
//	{
//		if (tri_type == TRI_TYPE_FLAT_TOP)
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
//							colorChannel[pos] = computePixelColor( r, g, b, a, uStart, vStart, texture );
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
//								colorChannel[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
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
//							colorChannel[pos] = computePixelColor( r, g, b, a, uEnd, vEnd, texture );
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
//							colorChannel[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
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
//								colorChannel[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
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
//							colorChannel[pos] = computePixelColor( r, g, b, a, uEnd, vEnd, texture );
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
//							colorChannel[pos] = computePixelColor( r, g, b, a, uStart, vStart, texture );
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
//								colorChannel[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
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
//							colorChannel[pos] = computePixelColor( r, g, b, a, uEnd, vEnd, texture );
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
//							colorChannel[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
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
//								colorChannel[pos] = computePixelColor( r, g, b, a, currU, currV, texture );
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
//							colorChannel[pos] = computePixelColor( r, g, b, a, uEnd, vEnd, texture );
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
//void triangle_rasterize_light( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2 )
//{
//	Vertex	* tmpV;
//	float			* zBuffer = view->zBuffer;
//
//	int				resX = (int)view->width, resY = (int)view->height;
//	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
//	float			z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, currZ,
//					dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr,dzdx, temp2,
//					dadyl, dadyr, dadx, currA, drdyl, drdyr, drdx, currR, dgdyl, dgdyr, dgdx, currG, dbdyl, dbdyr, dbdx, currB,
//					a0, r0, g0, b0, a1, r1, g1, b1, a2, r2, g2, b2, aStart, aEnd, rStart, rEnd, gStart, gEnd, bStart, bEnd;
//
//	if (((ver0->s_pos->y < 0) && (ver1->s_pos->y < 0) && (ver2->s_pos->y < 0)) ||
//		((ver0->s_pos->y > resY) && (ver1->s_pos->y > resY) && (ver2->s_pos->y > resY)) ||
//		((ver0->s_pos->x < 0) && (ver1->s_pos->x < 0) && (ver2->s_pos->x < 0)) ||
//		((ver0->s_pos->x > resX) && (ver1->s_pos->x > resX) && (ver2->s_pos->x > resX)) ||
//		((ver0->s_pos->z > 1) && (ver1->s_pos->z > 1) && (ver2->s_pos->z > 1)) ||
//		((ver0->s_pos->z < 0 ) && (ver1->s_pos->z < 0) && (ver2->s_pos->z < 0)))
//	{
//		return;
//	}
//
//	//判断是否是一条线，如果是，则返回。
//	if (((ver0->s_pos->x == ver1->s_pos->x) && (ver1->s_pos->x == ver2->s_pos->x)) || ((ver0->s_pos->y == ver1->s_pos->y) && (ver1->s_pos->y == ver2->s_pos->y)))
//	{
//		return;
//	}
//
//	//*************************************** start draw ************************************
//	nw = resX - 1;
//	nh = resY - 1;
//
//	//调整y0,y1,y2,让它们的y值从小到大
//	if (ver1->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver0;
//		ver0 = tmpV;
//	}
//	if (ver2->s_pos->y < ver0->s_pos->y)
//	{
//		tmpV = ver0;
//		ver0 = ver2;
//		ver2 = tmpV;
//	}
//	if (ver2->s_pos->y < ver1->s_pos->y)
//	{
//		tmpV = ver1;
//		ver1 = ver2;
//		ver2 = tmpV;
//	}
//
//	//判断三角形的类型
//	if (ver0->s_pos->y == ver1->s_pos->y)
//	{
//		tri_type = TRI_TYPE_FLAT_TOP;
//
//		if (ver1->s_pos->x < ver0->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver0;
//			ver0 = tmpV;
//		}
//	}
//	else if (ver1->s_pos->y == ver2->s_pos->y)
//	{
//		tri_type = TRI_TYPE_FLAT_BOTTOM;
//
//		if (ver2->s_pos->x < ver1->s_pos->x)
//		{
//			tmpV = ver1;
//			ver1 = ver2;
//			ver2 = tmpV;
//		}
//	}
//	else
//	{
//		tri_type = TRI_TYPE_GENERAL;
//	}
//
//	x0 = (int)ver0->s_pos->x;
//	y0 = (int)ver0->s_pos->y;
//	z0 = ver0->s_pos->z;
//
//	x1 = (int)ver1->s_pos->x;
//	y1 = (int)ver1->s_pos->y;
//	z1 = ver1->s_pos->z;
//
//	x2 = (int)ver2->s_pos->x;
//	y2 = (int)ver2->s_pos->y;
//	z2 = ver2->s_pos->z;
//
//	a0 = ver0->color->alpha;
//	r0 = ver0->color->red;
//	g0 = ver0->color->green;
//	b0 = ver0->color->blue;
//
//	a1 = ver1->color->alpha;
//	r1 = ver1->color->red;
//	g1 = ver1->color->green;
//	b1 = ver1->color->blue;
//
//	a2 = ver2->color->alpha;
//	r2 = ver2->color->red;
//	g2 = ver2->color->green;
//	b2 = ver2->color->blue;
//
//	side = 0;
//
//	//转折后的斜边在哪一侧
//	ys = y1;
//
//	//转折点y坐标
//	if (tri_type == TRI_TYPE_FLAT_TOP || tri_type == TRI_TYPE_FLAT_BOTTOM)
//	{
//		if (tri_type == TRI_TYPE_FLAT_TOP)
//		{
//			dy = 1.0f / (y2 - y0);
//
//			dxdyl = (x2 - x0) * dy;	//左斜边倒数
//			dzdyl = (z2 - z0) * dy;
//			
//			dadyl = (a2 - a0) * dy;
//			drdyl = (r2 - r0) * dy;
//			dgdyl = (g2 - g0) * dy;
//			dbdyl = (b2 - b0) * dy;
//
//			dxdyr = (x2 - x1) * dy;	//右斜边倒数
//			dzdyr = (z2 - z1) * dy;
//
//			dadyr = (a2 - a1) * dy;
//			drdyr = (r2 - r1) * dy;
//			dgdyr = (g2 - g1) * dy;
//			dbdyr = (b2 - b1) * dy;
//
//			//y0小于视窗顶部y坐标，调整左斜边和右斜边当前值,y值开始值
//			if (y0 < 0)
//			{
//				dy = - (float)y0;
//
//				xStart = dxdyl * dy + x0;
//				zStart = dzdyl * dy + z0;
//
//				aStart = dadyl * dy + a0;
//				rStart = drdyl * dy + r0;
//				gStart = dgdyl * dy + g0;
//				bStart = dbdyl * dy + b0;
//
//				xEnd = dxdyr * dy + x1;
//				zEnd = dzdyr * dy + z1;
//
//				aEnd = dadyr * dy + a1;
//				rEnd = drdyr * dy + r1;
//				gEnd = dgdyr * dy + g1;
//				bEnd = dbdyr * dy + b1;
//
//				yStart = 0;
//			}
//			else
//			{
//				//注意平顶和平底这里的区别
//				xStart = (float)x0;
//				zStart = z0;
//
//				aStart = a0;
//				rStart = r0;
//				gStart = g0;
//				bStart = b0;
//
//				xEnd = (float)x1;
//				zEnd = z1;
//
//				aEnd = a1;
//				rEnd = r1;
//				gEnd = g1;
//				bEnd = b1;
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
//
//			dadyl = (a1 - a0) * dy;
//			drdyl = (r1 - r0) * dy;
//			dgdyl = (g1 - g0) * dy;
//			dbdyl = (b1 - b0) * dy;
//
//			dxdyr = (x2 - x0) * dy;
//			dzdyr = (z2 - z0) * dy;
//
//			dadyr = (a2 - a0) * dy;
//			drdyr = (r2 - r0) * dy;
//			dgdyr = (g2 - g0) * dy;
//			dbdyr = (b2 - b0) * dy;
//
//			if (y0 < 0)
//			{
//				dy = - (float)y0;
//
//				xStart = dxdyl * dy + x0;
//				zStart = dzdyl * dy + z0;
//
//				aStart = dadyl * dy + a0;
//				rStart = drdyl * dy + r0;
//				gStart = dgdyl * dy + g0;
//				bStart = dbdyl * dy + b0;
//
//				xEnd = dxdyr * dy + x0;
//				zEnd = dzdyr * dy + z0;
//
//				aEnd = dadyr * dy + a0;
//				rEnd = drdyr * dy + r0;
//				gEnd = dgdyr * dy + g0;
//				bEnd = dbdyr * dy + b0;
//
//				yStart = 0.0f;
//			}
//			else
//			{
//				xStart = (float)x0;
//				zStart = z0;
//
//				aStart = a0;
//				rStart = r0;
//				gStart = g0;
//				bStart = b0;
//
//				xEnd = (float)x0;
//				zEnd = z0;
//
//				aEnd = a0;
//				rEnd = r0;
//				gEnd = g0;
//				bEnd = b0;
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
//							getPixelColor( (WORD)aStart, (WORD)rStart, (WORD)gStart, (WORD)bStart, pos, view->mixedChannel );
//							zBuffer[pos] = zStart;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						dadx = (aEnd - aStart) * dx;
//						drdx = (rEnd - rStart) * dx;
//						dgdx = (gEnd - gStart) * dx;
//						dbdx = (bEnd - bStart) * dx;
//
//						currZ = zStart;
//						currA = aStart;
//						currR = rStart;
//						currG = gStart;
//						currB = bStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//							currA -= cxStart * dadx;
//							currR -= cxStart * drdx;
//							currG -= cxStart * dgdx;
//							currB -= cxStart * dbdx;
//
//							cxStart = 0;
//						}
//						//绘制扫描线
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getPixelColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currA += dadx;
//							currR += drdx;
//							currG += dgdx;
//							currB += dbdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getPixelColor( (WORD)aEnd, (WORD)rEnd, (WORD)gEnd, (WORD)bEnd, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				//y每增加1时,xl和xr分别加上他们的递增量
//				xStart += dxdyl;
//				zStart += dzdyl;
//				
//				aStart += dadyl;
//				rStart += drdyl;
//				gStart += dgdyl;
//				bStart += dbdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				
//				aEnd += dadyr;
//				rEnd += drdyr;
//				gEnd += dgdyr;
//				bEnd += dbdyr;
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
//					currA = aStart;
//					currR = rStart;
//					currG = gStart;
//					currB = bStart;
//
//					if (cxStart == cxEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							getPixelColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, pos, view->mixedChannel );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						dadx = (aEnd - aStart) * dx;
//						drdx = (rEnd - rStart) * dx;
//						dgdx = (gEnd - gStart) * dx;
//						dbdx = (bEnd - bStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getPixelColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currA += dadx;
//							currR += drdx;
//							currG += dgdx;
//							currB += dbdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getPixelColor( (WORD)aEnd, (WORD)rEnd, (WORD)gEnd, (WORD)bEnd, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//				
//				aStart += dadyl;
//				rStart += drdyl;
//				gStart += dgdyl;
//				bStart += dbdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				
//				aEnd += dadyr;
//				rEnd += drdyr;
//				gEnd += dgdyr;
//				bEnd += dbdyr;
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
//
//			dadyl = (a2 - a1) * dyl;
//			drdyl = (r2 - r1) * dyl;
//			dgdyl = (g2 - g1) * dyl;
//			dbdyl = (b2 - b1) * dyl;
//
//			dxdyr = (x2 - x0) * dyr;
//			dzdyr = (z2 - z0) * dyr;
//
//			dadyr = (a2 - a0) * dyr;
//			drdyr = (r2 - r0) * dyr;
//			dbdyr = (g2 - g0) * dyr;
//			dgdyr = (b2 - b0) * dyr;
//
//			//计算左右斜边初始值
//			dyr = - (float)y0;
//			dyl = - (float)y1;
//
//			xStart = dxdyl * dyl + x1;
//			zStart = dzdyl * dyl + z1;
//
//			aStart = dadyl * dyl + a1;
//			rStart = drdyl * dyl + r1;
//			gStart = dgdyl * dyl + g1;
//			bStart = dbdyl * dyl + b1;
//
//			xEnd = dxdyr * dyr + x0;
//			zEnd = dzdyr * dyr + z0;
//			
//			aEnd = dadyr * dyr + a0;
//			rEnd = drdyr * dyr + r0;
//			gEnd = dgdyr * dyr + g0;
//			bEnd = dbdyr * dyr + b0;
//
//			yStart = 0;
//
//			if (dxdyr > dxdyl)
//			{
//				//交换斜边
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//				
//				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
//				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
//				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
//				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
//				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
//				
//				temp2 = aStart;	aStart = aEnd;	aEnd = temp2;
//				temp2 = rStart;	rStart = rEnd;	rEnd = temp2;
//				temp2 = gStart;	gStart = gEnd;	gEnd = temp2;
//				temp2 = bStart;	bStart = bEnd;	bEnd = temp2;
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
//			
//			dadyl = (a1 - a0) * dyl;
//			drdyl = (r1 - r0) * dyl;
//			dgdyl = (g1 - g0) * dyl;
//			dbdyl = (b1 - b0) * dyl;
//
//			dzdyr = (z2 - z0) * dyr;
//			
//			dadyr = (a2 - a0) * dyr;
//			drdyr = (r2 - r0) * dyr;
//			dgdyr = (g2 - g0) * dyr;
//			dbdyr = (b2 - b0) * dyr;
//
//			dy = - (float)y0;
//
//			xStart = dxdyl * dy + x0;
//			zStart = dzdyl * dy + z0;
//			
//			aStart = dadyl * dy + a0;
//			rStart = drdyl * dy + r0;
//			gStart = dgdyl * dy + g0;
//			bStart = dbdyl * dy + b0;
//
//			xEnd = dxdyr * dy + x0;
//			zEnd = dzdyr * dy + z0;
//			
//			aEnd = dadyr * dy + a0;
//			rEnd = drdyr * dy + r0;
//			gEnd = dgdyr * dy + g0;
//			bEnd = dbdyr * dy + b0;
//
//			yStart = 0;
//
//			if (dxdyr < dxdyl)
//			{
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//				
//				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
//				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
//				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
//				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;
//				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
//				
//				temp2 = aStart;	aStart = aEnd;	aEnd = temp2;
//				temp2 = rStart;	rStart = rEnd;	rEnd = temp2;
//				temp2 = gStart;	gStart = gEnd;	gEnd = temp2;
//				temp2 = bStart;	bStart = bEnd;	bEnd = temp2;
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
//
//			dxdyr = (x2 - x0) * dyr;
//			dzdyr = (z2 - z0) * dyr;
//
//			dadyl = (a1 - a0) * dyl;
//			drdyl = (r1 - r0) * dyl;
//			dgdyl = (g1 - g0) * dyl;
//			dbdyl = (b1 - b0) * dyl;
//
//			dadyr = (a2 - a0) * dyr;
//			drdyr = (r2 - r0) * dyr;
//			dgdyr = (g2 - g0) * dyr;
//			dbdyr = (b2 - b0) * dyr;
//
//			xStart = (float)x0;
//			zStart = z0;
//			
//			aStart = a0;
//			rStart = r0;
//			gStart = g0;
//			bStart = b0;
//
//			xEnd = (float)x0;
//			zEnd = z0;
//			
//			aEnd = a0;
//			rEnd = r0;
//			gEnd = g0;
//			bEnd = b0;
//
//			yStart = (float)y0;
//
//			if (dxdyr < dxdyl)
//			{
//				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
//				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
//
//				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
//				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
//				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
//				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = a1;	a1 = a2;	a2 = temp2;
//				temp2 = r1;	r1 = r2;	r2 = temp2;
//				temp2 = g1;	g1 = g2;	g2 = temp2;
//				temp2 = b1;	b1 = b2;	b2 = temp2;
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
//							getPixelColor( (WORD)aStart, (WORD)rStart, (WORD)gStart, (WORD)bStart, pos, view->mixedChannel );
//							zBuffer[pos] = zStart;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//						
//						dadx = (aEnd - aStart) * dx;
//						drdx = (rEnd - rStart) * dx;
//						dgdx = (gEnd - gStart) * dx;
//						dbdx = (bEnd - bStart) * dx;
//
//						currZ = zStart;
//						currA = aStart;
//						currR = rStart;
//						currG = gStart;
//						currB = bStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//							currA -= cxStart * dadx;
//							currR -= cxStart * drdx;
//							currG -= cxStart * dgdx;
//							currB -= cxStart * dbdx;
//
//							cxStart = 0;
//						}
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getPixelColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currA += dadx;
//							currR += drdx;
//							currG += dgdx;
//							currB += dbdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getPixelColor( (WORD)aEnd, (WORD)rEnd, (WORD)gEnd, (WORD)bEnd, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//
//				aStart += dadyl;
//				rStart += drdyl;
//				gStart += dgdyl;
//				bStart += dbdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//
//				aEnd += dadyr;
//				rEnd += drdyr;
//				gEnd += dgdyr;
//				bEnd += dbdyr;
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
//						
//						dadyl = (a2 - a1) * dyl;
//						drdyl = (r2 - r1) * dyl;
//						dgdyl = (g2 - g1) * dyl;
//						dbdyl = (b2 - b1) * dyl;
//
//						xStart = (float)x1;
//						zStart = z1;
//						
//						aStart = a1;
//						rStart = r1;
//						gStart = g1;
//						bStart = b1;
//					}
//					else
//					{
//						dyr = 1.0f / (y1 - y2);
//
//						dxdyr = (x1 - x2) * dyr;
//						dzdyr = (z1 - z2) * dyr;
//						
//						dadyr = (a1 - a2) * dyr;
//						drdyr = (r1 - r2) * dyr;
//						dgdyr = (g1 - g2) * dyr;
//						dbdyr = (b1 - b2) * dyr;
//
//						xEnd = (float)x2;
//						zEnd = z2;
//						
//						aEnd = a2;
//						rEnd = r2;
//						gEnd = g2;
//						bEnd = b2;
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
//					currA = aStart;
//					currR = rStart;
//					currG = gStart;
//					currB = bStart;
//
//					if (xStart == xEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							getPixelColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, pos, view->mixedChannel );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//
//						dzdx = (zEnd - zStart) * dx;
//
//						dadx = (aEnd - aStart) * dx;
//						drdx = (rEnd - rStart) * dx;
//						dgdx = (gEnd - gStart) * dx;
//						dbdx = (bEnd - bStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getPixelColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, pos, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currA += dadx;
//							currR += drdx;
//							currG += dgdx;
//							currB += dbdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getPixelColor( (WORD)aEnd, (WORD)rEnd, (WORD)gEnd, (WORD)bEnd, pos, view->mixedChannel );
//							zBuffer[pos] = zEnd;
//						}
//					}
//				}
//
//				xStart += dxdyl;
//				zStart += dzdyl;
//				
//				aStart += dadyl;
//				rStart += drdyl;
//				gStart += dgdyl;
//				bStart += dbdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				
//				aEnd += dadyr;
//				rEnd += drdyr;
//				gEnd += dgdyr;
//				bEnd += dbdyr;
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
//						
//						dadyl = (a2 - a1) * dyl;
//						drdyl = (r2 - r1) * dyl;
//						dgdyl = (g2 - g1) * dyl;
//						dbdyl = (b2 - b1) * dyl;
//
//						xStart = x1 + dxdyl;
//						zStart = z1 + dzdyl;
//						
//						aStart = a1 + dadyl;
//						rStart = r1 + drdyl;
//						gStart = g1 + dgdyl;
//						bStart = b1 + dbdyl;
//					}
//					else
//					{
//						dyr = 1.0f / (y1 - y2);
//
//						dxdyr = (x1 - x2) * dyr;
//						dzdyr = (z1 - z2) * dyr;
//						
//						dadyr = (a1 - a2) * dyr;
//						drdyr = (r1 - r2) * dyr;
//						dgdyr = (g1 - g2) * dyr;
//						dbdyr = (b1 - b2) * dyr;
//
//						xEnd = x2 + dxdyr;
//						zEnd = z2 + dzdyr;
//						
//						aEnd = a2 + dadyr;
//						rEnd = r2 + drdyr;
//						gEnd = g2 + dgdyr;
//						bEnd = b2 + dbdyr;
//					}
//				}
//			}
//		}
//	}
//}
//
//void triangle_rasterize_light_texture( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2, Texture * texture )
//{
//	Vertex	* tmpV;
//	float			* zBuffer = view->zBuffer;
//
//	int				resX = (int)view->width, resY = (int)view->height;
//	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
//	float			u0, v0, u1, v1, u2, v2, z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, uStart, uEnd, vStart, vEnd, currZ,
//					currU, currV, dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dudyl, dudyr, dvdyl, dvdyr, dzdx, dudx, dvdx, temp2,
//					dadyl, dadyr, dadx, currA, drdyl, drdyr, drdx, currR, dgdyl, dgdyr, dgdx, currG, dbdyl, dbdyr, dbdx, currB,
//					a0, r0, g0, b0, a1, r1, g1, b1, a2, r2, g2, b2, aStart, aEnd, rStart, rEnd, gStart, gEnd, bStart, bEnd;
//
//	if (((ver0->s_pos->y < 0) && (ver1->s_pos->y < 0) && (ver2->s_pos->y < 0)) ||
//		((ver0->s_pos->y > resY) && (ver1->s_pos->y > resY) && (ver2->s_pos->y > resY)) ||
//		((ver0->s_pos->x < 0) && (ver1->s_pos->x < 0) && (ver2->s_pos->x < 0)) ||
//		((ver0->s_pos->x > resX) && (ver1->s_pos->x > resX) && (ver2->s_pos->x > resX)) ||
//		((ver0->s_pos->z > 1) && (ver1->s_pos->z > 1) && (ver2->s_pos->z > 1)) ||
//		((ver0->s_pos->z < 0 ) && (ver1->s_pos->z < 0) && (ver2->s_pos->z < 0)))
//	{
//		return;
//	}
//
//	//判断是否是一条线，如果是，则返回。
//	if (((ver0->s_pos->x == ver1->s_pos->x) && (ver1->s_pos->x == ver2->s_pos->x)) || ((ver0->s_pos->y == ver1->s_pos->y) && (ver1->s_pos->y == ver2->s_pos->y)))
//	{
//		return;
//	}
//
//	//*************************************** start draw ************************************
//	nw = resX - 1;
//	nh = resY - 1;
//
//	//调整y0,y1,y2,让它们的y值从小到大
//	if ( ver1->s_pos->y < ver0->s_pos->y )
//	{
//		tmpV = ver1;
//		ver1 = ver0;
//		ver0 = tmpV;
//	}
//	if ( ver2->s_pos->y < ver0->s_pos->y )
//	{
//		tmpV = ver0;
//		ver0 = ver2;
//		ver2 = tmpV;
//	}
//	if ( ver2->s_pos->y < ver1->s_pos->y )
//	{
//		tmpV = ver1;
//		ver1 = ver2;
//		ver2 = tmpV;
//	}
//
//	//判断三角形的类型
//	if ( ver0->s_pos->y == ver1->s_pos->y )
//	{
//		tri_type = TRI_TYPE_FLAT_TOP;
//
//		if ( ver1->s_pos->x < ver0->s_pos->x )
//		{
//			tmpV = ver1;
//			ver1 = ver0;
//			ver0 = tmpV;
//		}
//	}
//	else if ( ver1->s_pos->y == ver2->s_pos->y )
//	{
//		tri_type = TRI_TYPE_FLAT_BOTTOM;
//
//		if ( ver2->s_pos->x < ver1->s_pos->x )
//		{
//			tmpV = ver1;
//			ver1 = ver2;
//			ver2 = tmpV;
//		}
//	}
//	else
//	{
//		tri_type = TRI_TYPE_GENERAL;
//	}
//
//	x0 = (int)ver0->s_pos->x;
//	y0 = (int)ver0->s_pos->y;
//	z0 = ver0->s_pos->z;
//
//	x1 = (int)ver1->s_pos->x;
//	y1 = (int)ver1->s_pos->y;
//	z1 = ver1->s_pos->z;
//
//	x2 = (int)ver2->s_pos->x;
//	y2 = (int)ver2->s_pos->y;
//	z2 = ver2->s_pos->z;
//
//	u0 = ver0->uv->x * (texture->width - 1);
//	v0 = ver0->uv->y * (texture->height - 1);
//	u1 = ver1->uv->x * (texture->width - 1);
//	v1 = ver1->uv->y * (texture->height - 1);
//	u2 = ver2->uv->x * (texture->width - 1);
//	v2 = ver2->uv->y * (texture->height - 1);
//
//	a0 = ver0->color->alpha;
//	r0 = ver0->color->red;
//	g0 = ver0->color->green;
//	b0 = ver0->color->blue;
//
//	a1 = ver1->color->alpha;
//	r1 = ver1->color->red;
//	g1 = ver1->color->green;
//	b1 = ver1->color->blue;
//
//	a2 = ver2->color->alpha;
//	r2 = ver2->color->red;
//	g2 = ver2->color->green;
//	b2 = ver2->color->blue;
//
//	/*AS3_Trace(AS3_String("Output0 XYZ..............."));
//	AS3_Trace(AS3_String("X0Y0...."));
//	AS3_Trace(AS3_Int(x0));
//	AS3_Trace(AS3_Int(y0));
//	AS3_Trace(AS3_Number(u0));
//	AS3_Trace(AS3_Number(v0));
//	AS3_Trace(AS3_String("X1Y1...."));
//	AS3_Trace(AS3_Int(x1));
//	AS3_Trace(AS3_Int(y1));
//	AS3_Trace(AS3_Number(u1));
//	AS3_Trace(AS3_Number(v1));
//	AS3_Trace(AS3_String("X2Y2...."));
//	AS3_Trace(AS3_Int(x2));
//	AS3_Trace(AS3_Int(y2));
//	AS3_Trace(AS3_Number(u2));
//	AS3_Trace(AS3_Number(v2));*/
//
//	side = 0;
//
//	//转折后的斜边在哪一侧
//	ys = y1;
//
//	//转折点y坐标
//	if (tri_type == TRI_TYPE_FLAT_TOP || tri_type == TRI_TYPE_FLAT_BOTTOM)
//	{
//		if (tri_type == TRI_TYPE_FLAT_TOP)
//		{
//			dy = 1.0f / (y2 - y0);
//
//			dxdyl = (x2 - x0) * dy;	//左斜边倒数
//			dzdyl = (z2 - z0) * dy;
//			dudyl = (u2 - u0) * dy;
//			dvdyl = (v2 - v0) * dy;
//			
//			dadyl = (a2 - a0) * dy;
//			drdyl = (r2 - r0) * dy;
//			dgdyl = (g2 - g0) * dy;
//			dbdyl = (b2 - b0) * dy;
//
//			dxdyr = (x2 - x1) * dy;	//右斜边倒数
//			dzdyr = (z2 - z1) * dy;
//			dudyr = (u2 - u1) * dy;
//			dvdyr = (v2 - v1) * dy;
//
//			dadyr = (a2 - a1) * dy;
//			drdyr = (r2 - r1) * dy;
//			dgdyr = (g2 - g1) * dy;
//			dbdyr = (b2 - b1) * dy;
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
//				aStart = dadyl * dy + a0;
//				rStart = drdyl * dy + r0;
//				gStart = dgdyl * dy + g0;
//				bStart = dbdyl * dy + b0;
//
//				xEnd = dxdyr * dy + x1;
//				zEnd = dzdyr * dy + z1;
//				uEnd = dudyr * dy + u1;
//				vEnd = dvdyr * dy + v1;
//
//				aEnd = dadyr * dy + a1;
//				rEnd = drdyr * dy + r1;
//				gEnd = dgdyr * dy + g1;
//				bEnd = dbdyr * dy + b1;
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
//				aStart = a0;
//				rStart = r0;
//				gStart = g0;
//				bStart = b0;
//
//				xEnd = (float)x1;
//				uEnd = u1;
//				vEnd = v1;
//				zEnd = z1;
//
//				aEnd = a1;
//				rEnd = r1;
//				gEnd = g1;
//				bEnd = b1;
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
//			dadyl = (a1 - a0) * dy;
//			drdyl = (r1 - r0) * dy;
//			dgdyl = (g1 - g0) * dy;
//			dbdyl = (b1 - b0) * dy;
//
//			dxdyr = (x2 - x0) * dy;
//			dzdyr = (z2 - z0) * dy;
//			dudyr = (u2 - u0) * dy;
//			dvdyr = (v2 - v0) * dy;
//
//			dadyr = (a2 - a0) * dy;
//			drdyr = (r2 - r0) * dy;
//			dgdyr = (g2 - g0) * dy;
//			dbdyr = (b2 - b0) * dy;
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
//				aStart = dadyl * dy + a0;
//				rStart = drdyl * dy + r0;
//				gStart = dgdyl * dy + g0;
//				bStart = dbdyl * dy + b0;
//
//				xEnd = dxdyr * dy + x0;
//				zEnd = dzdyr * dy + z0;
//				uEnd = dudyr * dy + u0;
//				vEnd = dvdyr * dy + v0;
//
//				aEnd = dadyr * dy + a0;
//				rEnd = drdyr * dy + r0;
//				gEnd = dgdyr * dy + g0;
//				bEnd = dbdyr * dy + b0;
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
//				aStart = a0;
//				rStart = r0;
//				gStart = g0;
//				bStart = b0;
//
//				xEnd = (float)x0;
//				uEnd = u0;
//				vEnd = v0;
//				zEnd = z0;
//
//				aEnd = a0;
//				rEnd = r0;
//				gEnd = g0;
//				bEnd = b0;
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
//							getMixedColor( (WORD)aStart, (WORD)rStart, (WORD)gStart, (WORD)bStart, (int)(uStart + 0.5), (int)(vStart + 0.5) * texture->width, pos, texture, view->mixedChannel );
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
//						dadx = (aEnd - aStart) * dx;
//						drdx = (rEnd - rStart) * dx;
//						dgdx = (gEnd - gStart) * dx;
//						dbdx = (bEnd - bStart) * dx;
//
//						currZ = zStart;
//						currU = uStart;
//						currV = vStart;
//						currA = aStart;
//						currR = rStart;
//						currG = gStart;
//						currB = bStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//							currU -= cxStart * dudx;
//							currV -= cxStart * dvdx;
//							currA -= cxStart * dadx;
//							currR -= cxStart * drdx;
//							currG -= cxStart * dgdx;
//							currB -= cxStart * dbdx;
//
//							cxStart = 0;
//						}
//						//绘制扫描线
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getMixedColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//							currA += dadx;
//							currR += drdx;
//							currG += dgdx;
//							currB += dbdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getMixedColor( (WORD)aEnd, (WORD)rEnd, (WORD)gEnd, (WORD)bEnd, (int)(uEnd + 0.5), (int)(vEnd + 0.5) * texture->width, pos, texture, view->mixedChannel );
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
//				aStart += dadyl;
//				rStart += drdyl;
//				gStart += dgdyl;
//				bStart += dbdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				uEnd += dudyr;
//				vEnd += dvdyr;
//				
//				aEnd += dadyr;
//				rEnd += drdyr;
//				gEnd += dgdyr;
//				bEnd += dbdyr;
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
//					currA = aStart;
//					currR = rStart;
//					currG = gStart;
//					currB = bStart;
//
//					if (cxStart == cxEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							getMixedColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
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
//						dadx = (aEnd - aStart) * dx;
//						drdx = (rEnd - rStart) * dx;
//						dgdx = (gEnd - gStart) * dx;
//						dbdx = (bEnd - bStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getMixedColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//							currA += dadx;
//							currR += drdx;
//							currG += dgdx;
//							currB += dbdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getMixedColor( (WORD)aEnd, (WORD)rEnd, (WORD)gEnd, (WORD)bEnd, (int)(uEnd + 0.5), (int)(vEnd + 0.5) * texture->width, pos, texture, view->mixedChannel );
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
//				aStart += dadyl;
//				rStart += drdyl;
//				gStart += dgdyl;
//				bStart += dbdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				uEnd += dudyr;
//				vEnd += dvdyr;
//				
//				aEnd += dadyr;
//				rEnd += drdyr;
//				gEnd += dgdyr;
//				bEnd += dbdyr;
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
//			dadyl = (a2 - a1) * dyl;
//			drdyl = (r2 - r1) * dyl;
//			dgdyl = (g2 - g1) * dyl;
//			dbdyl = (b2 - b1) * dyl;
//
//			dxdyr = (x2 - x0) * dyr;
//			dzdyr = (z2 - z0) * dyr;
//			dudyr = (u2 - u0) * dyr;
//			dvdyr = (v2 - v0) * dyr;
//
//			dadyr = (a2 - a0) * dyr;
//			drdyr = (r2 - r0) * dyr;
//			dbdyr = (g2 - g0) * dyr;
//			dgdyr = (b2 - b0) * dyr;
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
//			aStart = dadyl * dyl + a1;
//			rStart = drdyl * dyl + r1;
//			gStart = dgdyl * dyl + g1;
//			bStart = dbdyl * dyl + b1;
//
//			xEnd = dxdyr * dyr + x0;
//			zEnd = dzdyr * dyr + z0;
//			uEnd = dudyr * dyr + u0;
//			vEnd = dvdyr * dyr + v0;
//			
//			aEnd = dadyr * dyr + a0;
//			rEnd = drdyr * dyr + r0;
//			gEnd = dgdyr * dyr + g0;
//			bEnd = dbdyr * dyr + b0;
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
//				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
//				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
//				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
//				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;
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
//				temp2 = aStart;	aStart = aEnd;	aEnd = temp2;
//				temp2 = rStart;	rStart = rEnd;	rEnd = temp2;
//				temp2 = gStart;	gStart = gEnd;	gEnd = temp2;
//				temp2 = bStart;	bStart = bEnd;	bEnd = temp2;
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
//			dadyl = (a1 - a0) * dyl;
//			drdyl = (r1 - r0) * dyl;
//			dgdyl = (g1 - g0) * dyl;
//			dbdyl = (b1 - b0) * dyl;
//
//			dzdyr = (z2 - z0) * dyr;
//			dudyr = (u2 - u0) * dyr;
//			dvdyr = (v2 - v0) * dyr;
//			
//			dadyr = (a2 - a0) * dyr;
//			drdyr = (r2 - r0) * dyr;
//			dgdyr = (g2 - g0) * dyr;
//			dbdyr = (b2 - b0) * dyr;
//
//			dy = - (float)y0;
//
//			xStart = dxdyl * dy + x0;
//			zStart = dzdyl * dy + z0;
//			uStart = dudyl * dy + u0;
//			vStart = dvdyl * dy + v0;
//			
//			aStart = dadyl * dy + a0;
//			rStart = drdyl * dy + r0;
//			gStart = dgdyl * dy + g0;
//			bStart = dbdyl * dy + b0;
//
//			xEnd = dxdyr * dy + x0;
//			zEnd = dzdyr * dy + z0;
//			uEnd = dudyr * dy + u0;
//			vEnd = dvdyr * dy + v0;
//			
//			aEnd = dadyr * dy + a0;
//			rEnd = drdyr * dy + r0;
//			gEnd = dgdyr * dy + g0;
//			bEnd = dbdyr * dy + b0;
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
//				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
//				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
//				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
//				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;
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
//				temp2 = aStart;	aStart = aEnd;	aEnd = temp2;
//				temp2 = rStart;	rStart = rEnd;	rEnd = temp2;
//				temp2 = gStart;	gStart = gEnd;	gEnd = temp2;
//				temp2 = bStart;	bStart = bEnd;	bEnd = temp2;
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
//			dadyl = (a1 - a0) * dyl;
//			drdyl = (r1 - r0) * dyl;
//			dgdyl = (g1 - g0) * dyl;
//			dbdyl = (b1 - b0) * dyl;
//
//			dadyr = (a2 - a0) * dyr;
//			drdyr = (r2 - r0) * dyr;
//			dgdyr = (g2 - g0) * dyr;
//			dbdyr = (b2 - b0) * dyr;
//
//			xStart = (float)x0;
//			zStart = z0;
//			uStart = u0;
//			vStart = v0;
//			
//			aStart = a0;
//			rStart = r0;
//			gStart = g0;
//			bStart = b0;
//
//			xEnd = (float)x0;
//			zEnd = z0;
//			uEnd = u0;
//			vEnd = v0;
//			
//			aEnd = a0;
//			rEnd = r0;
//			gEnd = g0;
//			bEnd = b0;
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
//				temp2 = dadyl;	dadyl = dadyr;	dadyr = temp2;
//				temp2 = drdyl;	drdyl = drdyr;	drdyr = temp2;
//				temp2 = dgdyl;	dgdyl = dgdyr;	dgdyr = temp2;
//				temp2 = dbdyl;	dbdyl = dbdyr;	dbdyr = temp2;
//
//				temp = x1;	x1 = x2;	x2 = temp;
//				temp = y1;	y1 = y2;	y2 = temp;
//				temp2 = z1;	z1 = z2;	z2 = temp2;
//
//				temp2 = u1;	u1 = u2;	u2 = temp2;
//				temp2 = v1;	v1 = v2;	v2 = temp2;
//
//				temp2 = a1;	a1 = a2;	a2 = temp2;
//				temp2 = r1;	r1 = r2;	r2 = temp2;
//				temp2 = g1;	g1 = g2;	g2 = temp2;
//				temp2 = b1;	b1 = b2;	b2 = temp2;
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
//							getMixedColor( (WORD)aStart, (WORD)rStart, (WORD)gStart, (WORD)bStart, (int)(uStart + 0.5), (int)(vStart + 0.5) * texture->width, pos, texture, view->mixedChannel );
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
//						dadx = (aEnd - aStart) * dx;
//						drdx = (rEnd - rStart) * dx;
//						dgdx = (gEnd - gStart) * dx;
//						dbdx = (bEnd - bStart) * dx;
//
//						currZ = zStart;
//						currU = uStart;
//						currV = vStart;
//						currA = aStart;
//						currR = rStart;
//						currG = gStart;
//						currB = bStart;
//
//						//初始值需要裁剪
//						if (cxStart < 0)
//						{
//							currZ -= cxStart * dzdx;
//							currU -= cxStart * dudx;
//							currV -= cxStart * dvdx;
//							currA -= cxStart * dadx;
//							currR -= cxStart * drdx;
//							currG -= cxStart * dgdx;
//							currB -= cxStart * dbdx;
//
//							cxStart = 0;
//						}
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getMixedColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//							currA += dadx;
//							currR += drdx;
//							currG += dgdx;
//							currB += dbdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getMixedColor( (WORD)aEnd, (WORD)rEnd, (WORD)gEnd, (WORD)bEnd, (int)(uEnd + 0.5), (int)(vEnd + 0.5) * texture->width, pos, texture, view->mixedChannel );
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
//				aStart += dadyl;
//				rStart += drdyl;
//				gStart += dgdyl;
//				bStart += dbdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				uEnd += dudyr;
//				vEnd += dvdyr;
//
//				aEnd += dadyr;
//				rEnd += drdyr;
//				gEnd += dgdyr;
//				bEnd += dbdyr;
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
//						dadyl = (a2 - a1) * dyl;
//						drdyl = (r2 - r1) * dyl;
//						dgdyl = (g2 - g1) * dyl;
//						dbdyl = (b2 - b1) * dyl;
//
//						xStart = (float)x1;
//						zStart = z1;
//						uStart = u1;
//						vStart = v1;
//						
//						aStart = a1;
//						rStart = r1;
//						gStart = g1;
//						bStart = b1;
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
//						dadyr = (a1 - a2) * dyr;
//						drdyr = (r1 - r2) * dyr;
//						dgdyr = (g1 - g2) * dyr;
//						dbdyr = (b1 - b2) * dyr;
//
//						xEnd = (float)x2;
//						zEnd = z2;
//						uEnd = u2;
//						vEnd = v2;
//						
//						aEnd = a2;
//						rEnd = r2;
//						gEnd = g2;
//						bEnd = b2;
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
//					currA = aStart;
//					currR = rStart;
//					currG = gStart;
//					currB = bStart;
//
//					if (xStart == xEnd)
//					{
//						pos = cxStart + ypos;
//						if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//						{
//							getMixedColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//							zBuffer[pos] = currZ;
//						}
//					}
//					else
//					{
//						dx = 1.0f / (xEnd - xStart);
//						dzdx = (zEnd - zStart) * dx;
//						dudx = (uEnd - uStart) * dx;
//						dvdx = (vEnd - vStart) * dx;
//
//						dadx = (aEnd - aStart) * dx;
//						drdx = (rEnd - rStart) * dx;
//						dgdx = (gEnd - gStart) * dx;
//						dbdx = (bEnd - bStart) * dx;
//
//						for ( xi = cxStart; xi < cxEnd; xi ++ )
//						{
//							pos = xi + ypos;
//							if( currZ > 0 && currZ < 1 && currZ < zBuffer[pos] )
//							{
//								getMixedColor( (WORD)currA, (WORD)currR, (WORD)currG, (WORD)currB, (int)(currU + 0.5), (int)(currV + 0.5) * texture->width, pos, texture, view->mixedChannel );
//								zBuffer[pos] = currZ;
//							}
//
//							currZ += dzdx;
//							currU += dudx;
//							currV += dvdx;
//							currA += dadx;
//							currR += drdx;
//							currG += dgdx;
//							currB += dbdx;
//						}
//
//						pos = cxEnd + ypos;
//						if( zEnd > 0 && zEnd < 1 && zEnd < zBuffer[pos] )
//						{
//							getMixedColor( (WORD)aEnd, (WORD)rEnd, (WORD)gEnd, (WORD)bEnd, (int)(uEnd + 0.5), (int)(vEnd + 0.5) * texture->width, pos, texture, view->mixedChannel );
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
//				aStart += dadyl;
//				rStart += drdyl;
//				gStart += dgdyl;
//				bStart += dbdyl;
//
//				xEnd += dxdyr;
//				zEnd += dzdyr;
//				uEnd += dudyr;
//				vEnd += dvdyr;
//				
//				aEnd += dadyr;
//				rEnd += drdyr;
//				gEnd += dgdyr;
//				bEnd += dbdyr;
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
//						dadyl = (a2 - a1) * dyl;
//						drdyl = (r2 - r1) * dyl;
//						dgdyl = (g2 - g1) * dyl;
//						dbdyl = (b2 - b1) * dyl;
//
//						xStart = x1 + dxdyl;
//						zStart = z1 + dzdyl;
//						uStart = u1 + dudyl;
//						vStart = v1 + dvdyl;
//						
//						aStart = a1 + dadyl;
//						rStart = r1 + drdyl;
//						gStart = g1 + dgdyl;
//						bStart = b1 + dbdyl;
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
//						dadyr = (a1 - a2) * dyr;
//						drdyr = (r1 - r2) * dyr;
//						dgdyr = (g1 - g2) * dyr;
//						dbdyr = (b1 - b2) * dyr;
//
//						xEnd = x2 + dxdyr;
//						zEnd = z2 + dzdyr;
//						uEnd = u2 + dudyr;
//						vEnd = v2 + dvdyr;
//						
//						aEnd = a2 + dadyr;
//						rEnd = r2 + drdyr;
//						gEnd = g2 + dgdyr;
//						bEnd = b2 + dbdyr;
//					}
//				}
//			}
//		}
//	}
//}
//