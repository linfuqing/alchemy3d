#ifndef __RENDER_H
#define __RENDER_H

#include "Viewport.h"
#include "Scene.h"
#include "Camera.h"
#include "Entity.h"
#include "Polygon.h"
#include "Vertex.h"
#include "Vector3D.h"
#include "Color.h"

//��դ��
void triangle_rasterize( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2 )
{
	RenderVertex	* tmpV;
	uint32			* gfxBuffer = view->gfxBuffer;
	float			* zBuffer = view->zBuffer;

	int				resX = (int)view->width, resY = (int)view->height;
	int				x0, y0, x1, y1, x2, y2, nw, nh, side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
	float			u0, v0, u1, v1, u2, v2, z0, z1, z2, xStart, xEnd, yStart, yEnd, zStart, zEnd, uStart, uEnd, vStart, vEnd, currZ,
					currU, currV, dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dudyl, dudyr, dvdyl, dvdyr, dzdx, dudx, dvdx, temp2,
					dadyl, dadyr, dadx, currA, drdyl, drdyr, drdx, currR, dgdyl, dgdyr, dgdx, currG, dbdyl, dbdyr, dbdx, currB,
					a0, r0, g0, b0, a1, r1, g1, b1, a2, r2, g2, b2, aStart, aEnd, rStart, rEnd, gStart, gEnd, bStart, bEnd;


	//�����޳�
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

	//�ж��Ƿ���һ���ߣ�����ǣ��򷵻ء�
	if (((ver0->x == ver1->x) && (ver1->x == ver2->x)) || ((ver0->y == ver1->y) && (ver1->y == ver2->y)))
	{
		return;
	}

	//*************************************** start draw ************************************
	nw = resX - 1;
	nh = resY - 1;

	//����y0,y1,y2,�����ǵ�yֵ��С����
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

	//�ж������ε�����
	if (ver0->y == ver1->y)
	{
		tri_type = 0;

		if (ver1->x < ver0->x)
		{
			tmpV = ver1;
			ver1 = ver0;
			ver0 = tmpV;
		}
	}
	else if (ver1->y == ver2->y)
	{
		tri_type = 1;

		if (ver2->x < ver1->x)
		{
			tmpV = ver1;
			ver1 = ver2;
			ver2 = tmpV;
		}
	}
	else
	{
		tri_type = 2;
	}

	x0 = (int)(ver0->x + 0.5);
	y0 = (int)(ver0->y + 0.5);
	z0 = ver0->z;

	x1 = (int)(ver1->x + 0.5);
	y1 = (int)(ver1->y + 0.5);
	z1 = ver1->z;

	x2 = (int)(ver2->x + 0.5);
	y2 = (int)(ver2->y + 0.5);
	z2 = ver2->z;

	u0 = ver0->u;
	v0 = ver0->v;
	u1 = ver1->u;
	v1 = ver1->v;
	u2 = ver2->u;
	v2 = ver2->v;

	a0 = ver0->color->alpha;
	r0 = ver0->color->red;
	g0 = ver0->color->green;
	b0 = ver0->color->blue;

	a1 = ver1->color->alpha;
	r1 = ver1->color->red;
	g1 = ver1->color->green;
	b1 = ver1->color->blue;

	a2 = ver2->color->alpha;
	r2 = ver2->color->red;
	g2 = ver2->color->green;
	b2 = ver2->color->blue;

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

	//ת�ۺ��б������һ��
	ys = y1;

	//ת�۵�y����
	if (tri_type == 0 || tri_type == 1)
	{
		if (tri_type == 0)
		{
			dy = 1.0f / (y2 - y0);

			dxdyl = (x2 - x0) * dy;	//��б�ߵ���
			dzdyl = (z2 - z0) * dy;
			dudyl = (u2 - u0) * dy;
			dvdyl = (v2 - v0) * dy;
			
			dadyl = (a2 - a0) * dy;
			drdyl = (r2 - r0) * dy;
			dgdyl = (g2 - g0) * dy;
			dbdyl = (b2 - b0) * dy;

			dxdyr = (x2 - x1) * dy;	//��б�ߵ���
			dzdyr = (z2 - z1) * dy;
			dudyr = (u2 - u1) * dy;
			dvdyr = (v2 - v1) * dy;

			dadyr = (a2 - a1) * dy;
			drdyr = (r2 - r1) * dy;
			dgdyr = (g2 - g1) * dy;
			dbdyr = (b2 - b1) * dy;

			//y0С���Ӵ�����y���꣬������б�ߺ���б�ߵ�ǰֵ,yֵ��ʼֵ
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
				//ע��ƽ����ƽ�����������
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
			//ƽ��������
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
		//y2>�Ӵ��߶�ʱ������rect.height���־Ͳ��û�����
		cyStart = (int)(yStart + 0.5);
		cyEnd = (int)( ( y2 > nh ? nh : y2 ) + 0.5 );
		ypos = cyStart * resX;

		//xֵ��Ҫ�ü������
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if (xEnd < 0 || xStart > nw)
				{
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
					uEnd += dudyl;
					vEnd += dvdyr;

					aEnd += dadyr;
					rEnd += drdyr;
					gEnd += dgdyr;
					bEnd += dbdyr;

					continue;
				}

				dx = 1.0f / (xEnd - xStart);

				dzdx = (zEnd - zStart) * dx;
				dudx = (uEnd - uStart) * dx;
				dvdx = (vEnd - vStart) * dx;

				dadx = (aEnd - aStart) * dx;
				drdx = (rEnd - rStart) * dx;
				dgdx = (gEnd - gStart) * dx;
				dbdx = (bEnd - bStart) * dx;

				cxStart = (int)(xStart + 0.5);
				cxEnd = xEnd > nw ? nw : (int)(xEnd + 0.5);

				currZ = zStart;
				currU = uStart;
				currV = vStart;

				currA = aStart;
				currR = rStart;
				currG = gStart;
				currB = bStart;

				//��ʼֵ��Ҫ�ü�
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

				if (cxStart == cxEnd)
				{
					pos = cxStart + ypos;
					if(currZ < zBuffer[pos])
					{
						gfxBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
						zBuffer[pos] = currZ;
					}
				}
				else
				{
					//����ɨ����
					for ( xi = cxStart; xi <= cxEnd; xi ++ )
					{
						currU = currU > 1 ? 1 : currU;
						currV = currV > 1 ? 1 : currV;

						pos = xi + ypos;
						if(currZ < zBuffer[pos])
						{
							gfxBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
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
				}
				//yÿ����1ʱ,xl��xr�ֱ�������ǵĵ�����
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
			//����Ҫ�ü������
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)(xStart + 0.5);
				cxEnd = (int)(xEnd + 0.5);

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
					if(currZ < zBuffer[pos])
					{
						gfxBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
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

					for ( xi = cxStart; xi <= cxEnd; xi ++ )
					{
						currU = currU > 1 ? 1 : currU;
						currV = currV > 1 ? 1 : currV;

						pos = xi + ypos;
						if(currZ < zBuffer[pos])
						{
							gfxBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
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
		//��ͨ������
		yEnd = (float)(y2 > nh ? nh : y2);

		if (y1 < 0)
		{
			//����y0<y1,��ʱ�൱��ƽ��������
			//��������б�ߵ�б�ʵĵ���
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

			//��������б�߳�ʼֵ
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
				//����б��
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
			dzdyl = (z1 - z0) * dyl;
			dudyl = (u1 - u0) * dyl;
			dvdyl = (v1 - v0) * dyl;
			
			dadyl = (a1 - a0) * dyl;
			drdyl = (r1 - r0) * dyl;
			dgdyl = (g1 - g0) * dyl;
			dbdyl = (b1 - b0) * dyl;

			dxdyr = (x2 - x0) * dyr;
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
				/*x1 = x2 + (x2 = x1) * 0;
				y1 = y2 + (y2 = y1) * 0;
				z1 = z2 + (z2 = z1) * 0;*/

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
			//yֵ������0
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

		cyStart = (int)(yStart + 0.5);
		cyEnd = (int)(yEnd + 0.5);
		ypos = cyStart * resX;

		//x��Ҫ�ü�
		if ((x0 < 0) || (x0 > nw) || (x1 < 0) || (x1 > nw) || (x2 < 0) || (x2 > nw))
		{
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				if (xEnd < 0 || xStart > nw)
				{
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

					continue;
				}

				dx = 1.0f / (xEnd - xStart);
				dzdx = (zEnd - zStart) * dx;
				dudx = (uEnd - uStart) * dx;
				dvdx = (vEnd - vStart) * dx;
				
				dadx = (aEnd - aStart) * dx;
				drdx = (rEnd - rStart) * dx;
				dgdx = (gEnd - gStart) * dx;
				dbdx = (bEnd - bStart) * dx;

				cxStart = (int)(xStart + 0.5);
				cxEnd = xEnd > nw ? nw :(int)(xEnd + 0.5);;

				currZ = zStart;
				currU = uStart;
				currV = vStart;
				currA = aStart;
				currR = rStart;
				currG = gStart;
				currB = bStart;

				//��ʼֵ��Ҫ�ü�
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
				if (cxStart == cxEnd)
				{
					pos = cxStart + ypos;
					if(currZ < zBuffer[pos])
					{
						gfxBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
						zBuffer[pos] = currZ;
					}
				}
				else
				{
					for ( xi = cxStart; xi <= cxEnd; xi ++ )
					{
						currU = currU > 1 ? 1 : currU;
						currV = currV > 1 ? 1 : currV;

						pos = xi + ypos;
						if(currZ < zBuffer[pos])
						{
							gfxBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
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

				//ת�۵�
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
		else
		{
			//����Ҫ�ü�
			for ( yi = cyStart; yi <= cyEnd; yi ++ )
			{
				cxStart = (int)(xStart + 0.5);
				cxEnd = (int)(xEnd + 0.5);

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
					if(currZ < zBuffer[pos])
					{
						gfxBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
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

					for ( xi = cxStart; xi <= cxEnd; xi ++ )
					{
						currU = currU > 1 ? 1 : currU;
						currV = currV > 1 ? 1 : currV;

						pos = xi + ypos;
						if(currZ < zBuffer[pos])
						{
							gfxBuffer[pos] = ((int)currA << 24) + ((int)currR << 16) + ((int)currG << 8) + (int)currB;
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

void rasterize(Viewport * view)
{
	Scene * scene;
	SceneNode * sceneNode;
	Camera * camera;
	FaceNode * faceNode;
	Vertex * (* vertex)[3];

	resetBuffer( view );

	scene = view->scene;
	camera = view->camera;

	sceneNode = scene->nodes;

	//����ʵ��
	while( sceneNode != NULL )
	{
		//���ʵ������
		if ( TRUE == sceneNode->entity->offScreen )
		{
			sceneNode = sceneNode->next;
			continue;
		}

		faceNode = sceneNode->entity->mesh->faces->nodes;

		//������
		while( faceNode != NULL )
		{
			//���û�в��ʣ����ù�դ��
			if ( sceneNode->entity->material == NULL ) continue;

			//����汳������������ù�դ��
			if ( TRUE == faceNode->face->isBackFace )
			{
				polygon_resetBackFace( faceNode->face );

				faceNode = faceNode->next;

				continue;
			}

			vertex = & faceNode->face->vertex;
			
			triangle_rasterize( view, 
								& view->screenVertices[( * vertex)[0]->index],
								& view->screenVertices[( * vertex)[1]->index],
								& view->screenVertices[( * vertex)[2]->index] );

			polygon_resetBackFace( faceNode->face );

			faceNode = faceNode->next;
		}

		sceneNode = sceneNode->next;
	}
}

# endif