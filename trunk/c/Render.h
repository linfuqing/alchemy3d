#ifndef RENDER_H
#define RENDER_H

#include "Viewport.h"

typedef struct Screen
{
	Viewport * viewport;

	struct Screen * next;
}Screen;

typedef struct
{
	Screen * screen;

	Number * zBuffer;

	Number * gfxBuffer;
}RenderEngine;

//��դ��
void triangle_rasterize( Polygon * tri, Viewport * view )
{
	int resX, resY;

	unsigned int * gfxBuffer;
	Number * zBuffer;

	int x0, y0, z0, x1, y1, z1, x2, y2, z2, nw, nh;
	Number u0, v0, u1, v1, u2, v2;

	int side, ys, xi, yi, cxStart, cxEnd, cyStart, cyEnd, tri_type, pos, temp, ypos;
	Number xStart, xEnd, yStart, yEnd, zStart, zEnd, uStart, uEnd, vStart, vEnd, currZ, currU, currV, dx, dy, dyr, dyl, dxdyl, dxdyr, dzdyl, dzdyr, dudyl, dudyr, dvdyl, dvdyr, dzdx, dudx, dvdx, temp2;

	Polygon * triPtr;
	Vector * uvCoordinates;
	Vector3D * screenPosition;

	Number coordinates[9], uvDatas[6];

	int i, j;

	resX = (int)view->width;
	resY = (int)view->height;
	gfxBuffer = view->gfxBuffer;
	zBuffer = view->zBuffer;

	i = -1;
	j = -1;

	triPtr = tri;

	while(triPtr != NULL)
	{
		screenPosition = triPtr->vertex->screenPosition;
		uvCoordinates = triPtr->uv;

		coordinates[++i] = screenPosition->x;
		coordinates[++i] = screenPosition->y;
		coordinates[++i] = screenPosition->z;

		uvDatas[++j] = uvCoordinates->x;
		uvDatas[++j] = uvCoordinates->y;

		triPtr = triPtr->next;
	}

	x0 = (int)(coordinates[0] + 0.5);
	y0 = (int)(coordinates[1] + 0.5);
	z0 = (int)(coordinates[2] + 0.5);

	x1 = (int)(coordinates[3] + 0.5);
	y1 = (int)(coordinates[4] + 0.5);
	z1 = (int)(coordinates[5] + 0.5);

	x2 = (int)(coordinates[6] + 0.5);
	y2 = (int)(coordinates[7] + 0.5);
	z2 = (int)(coordinates[8] + 0.5);

	//�����޳�
	//if (!doubleSide)
	//{
	//	double caX = x2 - x0;
	//	double caY = y2 - y0;

	//	double bcX = x1 - x2;
	//	double bcY = y1 - y2;

	//	if (caX * bcY > caY * bcX)
	//	{
	//		continue;
	//	}
	//}

	if (((y0 < 0) && (y1 < 0) && (y2 < 0)) ||
		((y0 > resY) && (y1 > resY) && (y2 > resY)) ||
		((x0 < 0) && (x1 < 0) && (x2 < 0)) ||
		((x0 > resX) && (x1 > resX) && (x2 > resX)))
	{
		return;
	}

	//�ж��Ƿ���һ���ߣ�����ǣ��򷵻ء�
	if (((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
	{
		return;
	}

	u0 = uvDatas[0];
	v0 = uvDatas[1];
	u1 = uvDatas[2];
	v1 = uvDatas[3];
	u2 = uvDatas[4];
	v2 = uvDatas[5];

	//*************************************** start draw ************************************
	nw = resX - 1;
	nh = resY - 1;

	//����y0,y1,y2,�����ǵ�yֵ��С����
	if (y1 < y0)
	{
		temp = x0, x0 = x1, x1 = temp;
		temp = y0, y0 = y1, y1 = temp;
		temp = z0, z0 = z1, z1 = temp;
		temp2 = u0, u0 = u1, u1 = temp2;
		temp2 = v0, v0 = v1, v1 = temp2;
	}
	if (y2 < y0)
	{
		temp = x0, x0 = x2, x2 = temp;
		temp = y0, y0 = y2, y2 = temp;
		temp = z0, z0 = z2, z2 = temp;
		temp2 = u0, u0 = u2, u2 = temp2;
		temp2 = v0, v0 = v2, v2 = temp2;
	}
	if (y2 < y1)
	{
		temp = x1, x1 = x2, x2 = temp;
		temp = y1, y1 = y2, y2 = temp;
		temp = z1, z1 = z2, z2 = temp;
		temp2 = u1, u1 = u2, u2 = temp2;
		temp2 = v1, v1 = v2, v2 = temp2;
	}

	//�ж������ε�����
	if (y0 == y1)
	{
		tri_type = 0;

		if (x1 < x0)
		{
			temp = x0, x0 = x1, x1 = temp;
			temp = y0, y0 = y1, y1 = temp;
			temp = z0, z0 = z1, z1 = temp;
			temp2 = u0, u0 = u1, u1 = temp2;
			temp2 = v0, v0 = v1, v1 = temp2;
		}
	}
	else if (y1 == y2)
	{
		tri_type = 1;

		if (x2 < x1)
		{
			temp = x1, x1 = x2, x2 = temp;
			temp = y1, y1 = y2, y2 = temp;
			temp = z1, z1 = z2, z2 = temp;
			temp2 = u1, u1 = u2, u2 = temp2;
			temp2 = v1, v1 = v2, v2 = temp2;
		}
	}
	else
	{
		tri_type = 2;
	}

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
			dy = 1. / (y2 - y0);

			dxdyl = (x2 - x0) * dy;	//��б�ߵ���
			dxdyr = (x2 - x1) * dy;	//��б�ߵ���

			dzdyl = (z2 - z0) * dy;
			dzdyr = (z2 - z1) * dy;
			dudyl = (u2 - u0) * dy;

			dudyr = (u2 - u1) * dy;
			dvdyl = (v2 - v0) * dy;
			dvdyr = (v2 - v1) * dy;

			//y0С���Ӵ�����y���꣬������б�ߺ���б�ߵ�ǰֵ,yֵ��ʼֵ
			if (y0 < 0)
			{
				dy = - y0;

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
				//ע��ƽ����ƽ�����������
				xStart = x0;
				uStart = u0;
				vStart = v0;
				zStart = z0;

				xEnd = x1;
				uEnd = u1;
				vEnd = v1;
				zEnd = z1;

				yStart = y0;
			}
		}
		else
		{
			//ƽ��������
			dy = 1. / (y1 - y0);

			dxdyl = (x1 - x0) * dy;
			dxdyr = (x2 - x0) * dy;
			dzdyl = (z1 - z0) * dy;
			dzdyr = (z2 - z0) * dy;

			dudyl = (u1 - u0) * dy;
			dudyr = (u2 - u0) * dy;
			dvdyl = (v1 - v0) * dy;
			dvdyr = (v2 - v0) * dy;

			if (y0 < 0)
			{
				dy = - y0;
				xStart = dxdyl * dy + x0;
				zStart = dzdyl * dy + z0;
				uStart = dudyl * dy + u0;
				vStart = dvdyl * dy + v0;

				xEnd = dxdyr * dy + x0;
				zEnd = dzdyr * dy + z0;
				uEnd = dudyr * dy + u0;
				vEnd = dvdyr * dy + v0;

				yStart = 0;
			}
			else
			{
				xStart = x0;
				uStart = u0;
				vStart = v0;
				zStart = z0;

				xEnd = x0;
				uEnd = u0;
				vEnd = v0;
				zEnd = z0;

				yStart = y0;
			}
		}
		//y2>�Ӵ��߶�ʱ������rect.height���־Ͳ��û�����
		yEnd = y2 > nh ? nh : y2;
		cyStart = (int)(yStart + 0.5);
		cyEnd = (int)(yEnd + 0.5);
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

					xEnd += dxdyr;
					zEnd += dzdyr;
					uEnd += dudyl;
					vEnd += dvdyr;

					continue;
				}

				dx = 1. / (xEnd - xStart);
				dzdx = (zEnd - zStart) * dx;
				dudx = (uEnd - uStart) * dx;
				dvdx = (vEnd - vStart) * dx;

				cxStart = (int)(xStart + 0.5);
				cxEnd = xEnd > nw ? nw : (int)(xEnd + 0.5);

				currZ = zStart;
				currU = uStart;
				currV = vStart;

				//��ʼֵ��Ҫ�ü�
				if (cxStart < 0)
				{
					currZ -= cxStart * dzdx;
					currU -= cxStart * dudx;
					currV -= cxStart * dvdx;

					cxStart = 0;
				}

				if (cxStart == cxEnd)
				{
					pos = cxStart + ypos;
					if(currZ < zBuffer[pos])
					{
						gfxBuffer[pos] = 0xffffffff;
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
							gfxBuffer[pos] = 0xffffffff;
							zBuffer[pos] = currZ;
						}
						currZ += dzdx;
						currU += dudx;
						currV += dvdx;
					}
				}
				//yÿ����1ʱ,xl��xr�ֱ�������ǵĵ�����
				xStart += dxdyl;
				xEnd += dxdyr;
				zStart += dzdyl;
				zEnd += dzdyr;
				uStart += dudyl;
				uEnd += dudyr;
				vStart += dvdyl;
				vEnd += dvdyr;
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

				if (cxStart == cxEnd)
				{
					pos = cxStart + ypos;
					if(currZ < zBuffer[pos])
					{
						gfxBuffer[pos] = 0xffffffff;
						zBuffer[pos] = currZ;
					}
				}
				else
				{
					dx = 1. / (xEnd - xStart);
					dzdx = (zEnd - zStart) * dx;
					dudx = (uEnd - uStart) * dx;
					dvdx = (vEnd - vStart) * dx;

					for ( xi = cxStart; xi <= cxEnd; xi ++ )
					{
						currU = currU > 1 ? 1 : currU;
						currV = currV > 1 ? 1 : currV;

						pos = xi + ypos;
						if(currZ < zBuffer[pos])
						{
							gfxBuffer[pos] = 0xffffffff;
							zBuffer[pos] = currZ;

							//AS3_Trace(AS3_Int(pos));
							//AS3_Trace(AS3_Number(currZ));
						}

						currZ += dzdx;
						currU += dudx;
						currV += dvdx;
					}
				}
				xStart += dxdyl;
				xEnd += dxdyr;
				zStart += dzdyl;
				zEnd += dzdyr;
				uStart += dudyl;
				uEnd += dudyr;
				vStart += dvdyl;
				vEnd += dvdyr;
				ypos += resX;
			}
		}
	}
	else
	{
		//��ͨ������
		yEnd = y2 > nh ? nh : y2;

		if (y1 < 0)
		{
			//����y0<y1,��ʱ�൱��ƽ��������
			//��������б�ߵ�б�ʵĵ���
			dyl = 1. / (y2 - y1);
			dyr = 1. / (y2 - y0);

			dxdyl = (x2 - x1) * dyl;
			dzdyl = (z2 - z1) * dyl;
			dudyl = (u2 - u1) * dyl;
			dvdyl = (v2 - v1) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dudyr = (u2 - u0) * dyr;
			dvdyr = (v2 - v0) * dyr;

			//��������б�߳�ʼֵ
			dyr = - y0;
			dyl = - y1;

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
				//����б��
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;
				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp = z1;	z1 = z2;	z2 = temp;

				temp2 = u1;	u1 = u2;	u2 = temp2;
				temp2 = v1;	v1 = v2;	v2 = temp2;

				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
				temp2 = uStart;	uStart = uEnd;	uEnd = temp2;
				temp2 = vStart;	vStart = vEnd;	vEnd = temp2;

				side = 1;
			}
		}
		else if (y0 < 0)
		{
			dyl = 1. / (y1 - y0);
			dyr = 1. / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dzdyl = (z1 - z0) * dyl;
			dudyl = (u1 - u0) * dyl;
			dvdyl = (v1 - v0) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dudyr = (u2 - u0) * dyr;
			dvdyr = (v2 - v0) * dyr;

			dy = - y0;

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
				/*x1 = x2 + (x2 = x1) * 0;
				y1 = y2 + (y2 = y1) * 0;
				z1 = z2 + (z2 = z1) * 0;*/

				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;
				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp = z1;	z1 = z2;	z2 = temp;

				temp2 = u1;	u1 = u2;	u2 = temp2;
				temp2 = v1;	v1 = v2;	v2 = temp2;

				temp2 = zStart;	zStart = zEnd;	zEnd = temp2;
				temp2 = uStart;	uStart = uEnd;	uEnd = temp2;
				temp2 = vStart;	vStart = vEnd;	vEnd = temp2;

				side = 1;
			}
		}
		else
		{
			//yֵ������0
			dyl = 1. / (y1 - y0);
			dyr = 1. / (y2 - y0);

			dxdyl = (x1 - x0) * dyl;
			dzdyl = (z1 - z0) * dyl;
			dudyl = (u1 - u0) * dyl;
			dvdyl = (v1 - v0) * dyl;

			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dudyr = (u2 - u0) * dyr;
			dvdyr = (v2 - v0) * dyr;

			xStart = x0;
			zStart = z0;
			uStart = u0;
			vStart = v0;

			xEnd = x0;
			zEnd = z0;
			uEnd = u0;
			vEnd = v0;

			yStart = y0;

			if (dxdyr < dxdyl)
			{
				temp2 = dxdyl;	dxdyl = dxdyr;	dxdyr = temp2;
				temp2 = dzdyl;	dzdyl = dzdyr;	dzdyr = temp2;
				temp2 = dudyl;	dudyl = dudyr;	dudyr = temp2;
				temp2 = dvdyl;	dvdyl = dvdyr;	dvdyr = temp2;
				temp2 = xStart;	xStart = xEnd;	xEnd = temp2;

				temp = x1;	x1 = x2;	x2 = temp;
				temp = y1;	y1 = y2;	y2 = temp;
				temp = z1;	z1 = z2;	z2 = temp;

				temp2 = u1;	u1 = u2;	u2 = temp2;
				temp2 = v1;	v1 = v2;	v2 = temp2;

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

					xEnd += dxdyr;
					zEnd += dzdyr;
					uEnd += dudyr;
					vEnd += dvdyr;

					continue;
				}

				dx = 1. / (xEnd - xStart);
				dzdx = (zEnd - zStart) * dx;
				dudx = (uEnd - uStart) * dx;
				dvdx = (vEnd - vStart) * dx;

				cxStart = (int)(xStart + 0.5);
				cxEnd = xEnd > nw ? nw :(int)(xEnd + 0.5);;

				currZ = zStart;
				currU = uStart;
				currV = vStart;

				//��ʼֵ��Ҫ�ü�
				if (cxStart < 0)
				{
					currZ -= cxStart * dzdx;
					currU -= cxStart * dudx;
					currV -= cxStart * dvdx;

					cxStart = 0;
				}
				if (cxStart == cxEnd)
				{
					pos = cxStart + ypos;
					if(currZ < zBuffer[pos])
					{
						gfxBuffer[pos] = 0xffffffff;
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
							gfxBuffer[pos] = 0xffffffff;
							zBuffer[pos] = currZ;
						}

						currZ += dzdx;
						currU += dudx;
						currV += dvdx;
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

				//ת�۵�
				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1. / (y2 - y1);

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
						dyr = 1. / (y1 - y2);

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

				if (xStart == xEnd)
				{
					pos = cxStart + ypos;
					if(currZ < zBuffer[pos])
					{
						gfxBuffer[pos] = 0xffffffff;
						zBuffer[pos] = currZ;
					}
				}
				else
				{
					dx = 1. / (xEnd - xStart);
					dzdx = (zEnd - zStart) * dx;
					dudx = (uEnd - uStart) * dx;
					dvdx = (vEnd - vStart) * dx;

					for ( xi = cxStart; xi <= cxEnd; xi ++ )
					{
						currU = currU > 1 ? 1 : currU;
						currV = currV > 1 ? 1 : currV;

						pos = xi + ypos;
						if(currZ < zBuffer[pos])
						{
							gfxBuffer[pos] = 0xffffffff;
							zBuffer[pos] = currZ;
						}

						currZ += dzdx;
						currU += dudx;
						currV += dvdx;
					}
				}

				xStart += dxdyl;
				xEnd += dxdyr;
				zStart += dzdyl;
				zEnd += dzdyr;
				uStart += dudyl;
				uEnd += dudyr;
				vStart += dvdyl;
				vEnd += dvdyr;
				ypos += resX;

				if (yi == ys)
				{
					if (side == 0)
					{
						dyl = 1. / (y2 - y1);

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
						dyr = 1. / (y1 - y2);

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

void renderPolygon( ScreenPolygon * f, Number * rgfxBuffer, Number * zBuffer )
{
}

//�Ķ�:
void render( RenderEngine * r )
{
	Screen      * vp = r -> screen;
	ScreenFaces * graphics;

	while( vp != NULL )
	{
		transformScene( vp -> viewport -> scene );
		
		graphics = vp -> viewport -> graphics;

		while( ( graphics = graphics -> next ) == NULL )
		{
			renderPolygon( graphics -> face, r -> gfxBuffer, r -> zBuffer );
		}

		vp = vp -> next;
	}
}

# endif