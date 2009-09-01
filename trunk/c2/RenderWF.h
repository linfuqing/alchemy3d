#ifndef __RENDERFWF_H
#define __RENDERFWF_H

int Draw_Line32(int x0, int y0, int x1, int y1, DWORD color, BYTE * vb_start, int lpitch)// video buffer and memory pitch
{
	int dx,
		dy,
		dx2,
		dy2,
		x_inc,
		y_inc,
		error,
		index;

	int lpitch_2 = lpitch >> 2;

	DWORD *vb_start2 = (DWORD *)vb_start + x0 + y0*lpitch_2;

	dx = x1-x0;
	dy = y1-y0;

	if (dx>=0)
	{
		x_inc = 1;

	}
	else
	{
		x_inc = -1;
		dx = -dx;

	}


	if (dy>=0)
	{
		y_inc = lpitch_2;
	}
	else
	{
		y_inc = -lpitch_2;
		dy = -dy;

	}

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)
	{
		error = dy2 - dx;

		for (index=0; index <= dx; index++)
		{
			*vb_start2 = color;

			if (error >= 0)
			{
				error-=dx2;

				vb_start2+=y_inc;

			}

			error+=dy2;

			vb_start2+=x_inc;

		} // end for

	} // end if |slope| <= 1
	else
	{
		error = dx2 - dy;

		for (index=0; index <= dy; index++)
		{
			*vb_start2 = color;

			if (error >= 0)
			{
				error-=dy2;

				vb_start2+=x_inc;

			}

			error+=dx2;

			vb_start2+=y_inc;

		} // end for

	} // end else |slope| > 1

	return(1);

}

int Clip_Line(int * x1p,int * y1p,int * x2p, int * y2p, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y)
{
#define CLIP_CODE_C 0x0000
#define CLIP_CODE_N 0x0008
#define CLIP_CODE_S 0x0004
#define CLIP_CODE_E 0x0002
#define CLIP_CODE_W 0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009
#define CLIP_CODE_SW 0x0005

	int x1 = *x1p;
	int x2 = *x2p;
	int y1 = *y1p;
	int y2 = *y2p;

	int xc1 = x1,
		yc1 = y1,
		xc2 = x2,
		yc2 = y2;

	int p1_code=0,
		p2_code=0;

	if (y1 < min_clip_y)
		p1_code|=CLIP_CODE_N;
	else
		if (y1 > max_clip_y)
			p1_code|=CLIP_CODE_S;

	if (x1 < min_clip_x)
		p1_code|=CLIP_CODE_W;
	else
		if (x1 > max_clip_x)
			p1_code|=CLIP_CODE_E;

	if (y2 < min_clip_y)
		p2_code|=CLIP_CODE_N;
	else
		if (y2 > max_clip_y)
			p2_code|=CLIP_CODE_S;

	if (x2 < min_clip_x)
		p2_code|=CLIP_CODE_W;
	else
		if (x2 > max_clip_x)
			p2_code|=CLIP_CODE_E;

	if ((p1_code & p2_code))
		return(0);

	if (p1_code==0 && p2_code==0)
		return(1);

	switch(p1_code)
	{
		case CLIP_CODE_C: break;

		case CLIP_CODE_N:
			{
				yc1 = min_clip_y;
				xc1 = x1 + (int)( 0.5 + (min_clip_y - y1)*(x2 - x1) / (y2 - y1) );
			}break;
		case CLIP_CODE_S:
			{
				yc1 = max_clip_y;
				xc1 = x1 + (int)( 0.5 + (max_clip_y-y1)*(x2 - x1) / (y2 - y1) );
			}break;

		case CLIP_CODE_W:
			{
				xc1 = min_clip_x;
				yc1 = y1 + (int)( 0.5 + (min_clip_x-x1)*(y2 - y1) / (x2 - x1) );
			}break;

		case CLIP_CODE_E:
			{
				xc1 = max_clip_x;
				yc1 = y1 + (int)( 0.5 + (max_clip_x-x1)*(y2 - y1) / (x2 - x1) );
			}break;

		case CLIP_CODE_NE:
			{
				yc1 = min_clip_y;
				xc1 = x1 + (int)( 0.5+(min_clip_y - y1)*(x2-x1)/(y2-y1) );

				if (xc1 < min_clip_x || xc1 > max_clip_x)
				{
					xc1 = max_clip_x;
					yc1 = y1 + (int)( 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1) );
				}

			}break;

		case CLIP_CODE_SE:
			{
				yc1 = max_clip_y;
				xc1 = x1 + (int)( 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1) );

				if (xc1 < min_clip_x || xc1 > max_clip_x)
				{
					xc1 = max_clip_x;
					yc1 = y1 + (int)( 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1) );
				}

			}break;

		case CLIP_CODE_NW:
			{
				yc1 = min_clip_y;
				xc1 = x1 + (int)( 0.5+(min_clip_y - y1)*(x2-x1)/(y2-y1) );

				if (xc1 < min_clip_x || xc1 > max_clip_x)
				{
					xc1 = min_clip_x;
					yc1 = y1 + (int)( 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1) );
				}

			}break;

		case CLIP_CODE_SW:
			{
				yc1 = max_clip_y;
				xc1 = x1 + (int)( 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1) );

				if (xc1 < min_clip_x || xc1 > max_clip_x)
				{
					xc1 = min_clip_x;
					yc1 = y1 + (int)( 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1) );
				}

			}break;

		default:break;
	}

	switch(p2_code)
	{
		case CLIP_CODE_C: break;

		case CLIP_CODE_N:
			{
				yc2 = min_clip_y;
				xc2 = x2 + (min_clip_y-y2)*(x1-x2)/(y1-y2);
			}break;

		case CLIP_CODE_S:
			{
				yc2 = max_clip_y;
				xc2 = x2 + (max_clip_y-y2)*(x1-x2)/(y1-y2);
			}break;

		case CLIP_CODE_W:
			{
				xc2 = min_clip_x;
				yc2 = y2 + (min_clip_x-x2)*(y1-y2)/(x1-x2);
			}break;

		case CLIP_CODE_E:
			{
				xc2 = max_clip_x;
				yc2 = y2 + (max_clip_x-x2)*(y1-y2)/(x1-x2);
			}break;

		case CLIP_CODE_NE:
			{
				yc2 = min_clip_y;
				xc2 = x2 + (int)( 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2) );

				if (xc2 < min_clip_x || xc2 > max_clip_x)
				{
					xc2 = max_clip_x;
					yc2 = y2 + (int)( 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2) );
				} // end if

			}break;

		case CLIP_CODE_SE:
			{
				yc2 = max_clip_y;
				xc2 = x2 + (int)( 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2) );

				if (xc2 < min_clip_x || xc2 > max_clip_x)
				{
					xc2 = max_clip_x;
					yc2 = y2 + (int)( 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2) );
				} // end if

			}break;

		case CLIP_CODE_NW:
			{
				yc2 = min_clip_y;
				xc2 = x2 + (int)( 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2) );

				if (xc2 < min_clip_x || xc2 > max_clip_x)
				{
					xc2 = min_clip_x;
					yc2 = y2 + (int)( 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2) );
				}

			}break;

		case CLIP_CODE_SW:
			{
				yc2 = max_clip_y;
				xc2 = x2 + (int)( 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2) );

				if (xc2 < min_clip_x || xc2 > max_clip_x)
				{
					xc2 = min_clip_x;
					yc2 = y2 + (int)( 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2) );
				}

			}break;

		default:break;
	}

	if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
		(yc1 < min_clip_y) || (yc1 > max_clip_y) ||
		(xc2 < min_clip_x) || (xc2 > max_clip_x) ||
		(yc2 < min_clip_y) || (yc2 > max_clip_y) )
	{
		return(0);
	}

	* x1p = xc1;
	* y1p = yc1;
	* x2p = xc2;
	* y2p = yc2;

	return(1);

}

int Draw_Clip_Line32(int x0,int y0, int x1, int y1, int color, BYTE * dest_buffer, int lpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y)
{
	int cxs, cys,
		cxe, cye;

	cxs = x0;
	cys = y0;
	cxe = x1;
	cye = y1;

	if (Clip_Line(& cxs,& cys,& cxe,& cye, min_clip_x, max_clip_x, min_clip_y, max_clip_y))
		Draw_Line32(cxs, cys, cxe,cye,color,dest_buffer,lpitch);

	return(1);
}

void Draw_Wireframe_Triangle_32( Triangle * face, BYTE * dest_buffer, int mempitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
{
	int x0, y0, x1, y1, x2, y2;
	DWORD color;

	x0 = (int)( face->vertex[0]->s_pos->x + 0.5f );
	y0 = (int)( face->vertex[0]->s_pos->y + 0.5f );
	x1 = (int)( face->vertex[1]->s_pos->x + 0.5f );
	y1 = (int)( face->vertex[1]->s_pos->y + 0.5f );
	x2 = (int)( face->vertex[2]->s_pos->x + 0.5f );
	y2 = (int)( face->vertex[2]->s_pos->y + 0.5f );

	color = RGB32BIT( face->vertex[0]->color->alpha, face->vertex[0]->color->red, face->vertex[0]->color->green, face->vertex[0]->color->blue );

	Draw_Clip_Line32( x0, y0, x1, y1, color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y );
	Draw_Clip_Line32( x1, y1, x2, y2, color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y );
	Draw_Clip_Line32( x2, y2, x0, y0, color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y );
}

void Draw_Top_Tri32(int x1,int y1, int x2,int y2, int x3,int y3, DWORD color, BYTE * _dest_buffer, int mempitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y)
{
	float dx_right,
		dx_left,
		xs,xe,
		height;

	int temp_x,
		temp_y,
		right,
		left;

	DWORD *dest_buffer = (DWORD *)_dest_buffer;

	DWORD *dest_addr = NULL;

	mempitch = (mempitch >> 2);

	if (x2 < x1)
	{
		temp_x = x2;
		x2 = x1;
		x1 = temp_x;
	}

	height = (float)(y3-y1);

	dx_left = (x3-x1)/height;
	dx_right = (x3-x2)/height;

	xs = (float)x1;
	xe = (float)x2;

	if (y1 < min_clip_y)
	{
		xs = xs+dx_left*(float)(-y1+min_clip_y);
		xe = xe+dx_right*(float)(-y1+min_clip_y);

		y1=min_clip_y;

	}

	if (y3>max_clip_y)
		y3=max_clip_y;

	dest_addr = dest_buffer+y1*mempitch;

	if (x1>=min_clip_x && x1<=max_clip_x &&
		x2>=min_clip_x && x2<=max_clip_x &&
		x3>=min_clip_x && x3<=max_clip_x)
	{
		for (temp_y=y1; temp_y<=y3; temp_y++,dest_addr+=mempitch)
		{
			for (temp_x = (int)xs; temp_x < (int)xe + 1; temp_x ++)
			{
				dest_addr[temp_x] = color;
			}

			xs+=dx_left;
			xe+=dx_right;
		}
	}
	else
	{
		for (temp_y=y1; temp_y<=y3; temp_y++,dest_addr+=mempitch)
		{
			left = (int)xs;
			right = (int)xe;
			xs+=dx_left;
			xe+=dx_right;

			if (left < min_clip_x)
			{
				left = min_clip_x;

				if (right < min_clip_x)
					continue;
			}

			if (right > max_clip_x)
			{
				right = max_clip_x;

				if (left > max_clip_x)
					continue;
			}

			for (temp_x = (int)left; temp_x < (int)right + 1; temp_x ++)
			{
				dest_addr[temp_x] = color;
			}
		}
	}
}

void Draw_Bottom_Tri32(int x1, int y1, int x2, int y2, int x3, int y3, DWORD color, BYTE * _dest_buffer, int mempitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y)
{
	float dx_right,
		dx_left,
		xs,xe,
		height;

	int temp_x,
		temp_y,
		right,
		left;

	DWORD *dest_buffer = (DWORD *)_dest_buffer;

	DWORD *dest_addr = NULL;

	mempitch = (mempitch >> 2);

	if (x3 < x2)
	{
		temp_x = x2;
		x2 = x3;
		x3 = temp_x;
	}

	height = (float)(y3-y1);

	dx_left = (x2-x1)/height;
	dx_right = (x3-x1)/height;

	xs = (float)x1;
	xe = (float)x1;

	if (y1 < min_clip_y)
	{
		xs = xs+dx_left*(float)(-y1+min_clip_y);
		xe = xe+dx_right*(float)(-y1+min_clip_y);

		y1 = min_clip_y;

	}

	if (y3 > max_clip_y)
		y3 = max_clip_y;

	dest_addr = dest_buffer + y1 * mempitch;

	if (x1>=min_clip_x && x1<=max_clip_x &&
		x2>=min_clip_x && x2<=max_clip_x &&
		x3>=min_clip_x && x3<=max_clip_x)
	{
		for (temp_y=y1; temp_y<=y3; temp_y++,dest_addr+=mempitch)
		{
			for (temp_x = (int)xs; temp_x < (int)xe + 1; temp_x ++)
			{
				dest_addr[temp_x] = color;
			}
			xs+=dx_left;
			xe+=dx_right;
		}
	}
	else
	{
		for (temp_y=y1; temp_y<=y3; temp_y++,dest_addr+=mempitch)
		{
			left = (int)xs;
			right = (int)xe;
			xs+=dx_left;
			xe+=dx_right;

			if (left < min_clip_x)
			{
				left = min_clip_x;

				if (right < min_clip_x)
					continue;
			}

			if (right > max_clip_x)
			{
				right = max_clip_x;

				if (left > max_clip_x)
					continue;
			}

			for (temp_x = (int)left; temp_x < (int)right + 1; temp_x ++)
			{
				dest_addr[temp_x] = color;
			}
		}
	}
}

void Draw_Flat_Triangle_32(Triangle * face, BYTE * dest_buffer, int mempitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y)
{
	int temp_x,
		temp_y,
		new_x;

	int x1, y1, x2, y2, x3, y3;

	DWORD color;

	x1 = (int)( face->vertex[0]->s_pos->x + 0.5f );
	y1 = (int)( face->vertex[0]->s_pos->y + 0.5f );
	x2 = (int)( face->vertex[1]->s_pos->x + 0.5f );
	y2 = (int)( face->vertex[1]->s_pos->y + 0.5f );
	x3 = (int)( face->vertex[2]->s_pos->x + 0.5f );
	y3 = (int)( face->vertex[2]->s_pos->y + 0.5f );

	color = RGB32BIT( face->vertex[0]->color->alpha, face->vertex[0]->color->red, face->vertex[0]->color->green, face->vertex[0]->color->blue );

	if ((x1==x2 && x2==x3) || (y1==y2 && y2==y3))
		return;

	if (y2<y1)
	{
		temp_x = x2;
		temp_y = y2;
		x2 = x1;
		y2 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if

	if (y3<y1)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x1;
		y3 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if

	if (y3<y2)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x2;
		y3 = y2;
		x2 = temp_x;
		y2 = temp_y;

	} // end if

	if ( y3<min_clip_y || y1>max_clip_y ||
		(x1<min_clip_x && x2<min_clip_x && x3<min_clip_x) ||
		(x1>max_clip_x && x2>max_clip_x && x3>max_clip_x) )
		return;

	if (y1==y2)
	{
		Draw_Top_Tri32(x1,y1,x2,y2,x3,y3,color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y);
	} // end if
	else if (y2==y3)
	{
		Draw_Bottom_Tri32(x1,y1,x2,y2,x3,y3,color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y);
	} // end if bottom is flat
	else
	{
		new_x = x1 + (int)(0.5+(float)(y2-y1)*(float)(x3-x1)/(float)(y3-y1));

		Draw_Bottom_Tri32(x1,y1,new_x,y2,x2,y2,color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y);
		Draw_Top_Tri32(x2,y2,new_x,y2,x3,y3,color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y);
	}
}

void Draw_Top_TriFP(int x1,int y1, int x2,int y2, int x3,int y3, DWORD color, BYTE * dest_buffer, int mempitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y)
{
	int dx_right,
		dx_left,
		xs,xe,
		height;

	int temp_x,
		temp_y,
		right,
		left;

	DWORD *dest_addr;

	mempitch >>= 2;

	if (y1==y3 || y2==y3)
		return;

	if (x2 < x1)
	{
		temp_x = x2;
		x2 = x1;
		x1 = temp_x;
	}

	height = y3-y1;

	dx_left = ((x3-x1)<<FIXP16_SHIFT)/height;
	dx_right = ((x3-x2)<<FIXP16_SHIFT)/height;

	xs = (x1<<FIXP16_SHIFT);
	xe = (x2<<FIXP16_SHIFT);

	if (y1<min_clip_y)
	{
		xs = xs+dx_left*(-y1+min_clip_y);
		xe = xe+dx_right*(-y1+min_clip_y);

		y1=min_clip_y;

	}

	if (y3>max_clip_y)
		y3=max_clip_y;

	dest_addr = (DWORD *)dest_buffer+y1*mempitch;

	if (x1>=min_clip_x && x1<=max_clip_x &&
		x2>=min_clip_x && x2<=max_clip_x &&
		x3>=min_clip_x && x3<=max_clip_x)
	{
		for (temp_y=y1; temp_y<=y3; temp_y++,dest_addr+=mempitch)
		{
			/*memset((BYTE * )dest_addr+((xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT),
			color, (((xe-xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT)+1));*/

			for (temp_x = ((xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT); temp_x < ((xe+FIXP16_ROUND_UP)>>FIXP16_SHIFT)+1; temp_x ++)
			{
				dest_addr[temp_x] = color;
			}
			xs+=dx_left;
			xe+=dx_right;
		}
	}
	else
	{
		for (temp_y=y1; temp_y<=y3; temp_y++,dest_addr+=mempitch)
		{
			left = ((xs+FIXP16_ROUND_UP)>>16);
			right = ((xe+FIXP16_ROUND_UP)>>16);
			xs+=dx_left;
			xe+=dx_right;

			if (left < min_clip_x)
			{
				left = min_clip_x;

				if (right < min_clip_x)
					continue;
			}

			if (right > max_clip_x)
			{
				right = max_clip_x;

				if (left > max_clip_x)
					continue;
			}

			/*memset((BYTE * )dest_addr+(unsigned int)left,
			color,(unsigned int)(right-left+1));*/

			for (temp_x = (int)left; temp_x < (int)right + 1; temp_x ++)
			{
				dest_addr[temp_x] = color;
			}
		}
	}
}

void Draw_Bottom_TriFP(int x1,int y1, int x2,int y2, int x3,int y3, DWORD color, BYTE * dest_buffer, int mempitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y)
{
	int dx_right,
		dx_left,
		xs,xe,
		height;

	int temp_x,
		temp_y,
		right,
		left;

	DWORD *dest_addr;

	mempitch >>= 2;

	if (y1==y2 || y1==y3)
		return;

	if (x3 < x2)
	{
		temp_x = x2;
		x2 = x3;
		x3 = temp_x;

	}

	height = y3-y1;

	dx_left = ((x2-x1)<<FIXP16_SHIFT)/height;
	dx_right = ((x3-x1)<<FIXP16_SHIFT)/height;

	xs = (x1<<FIXP16_SHIFT);
	xe = (x1<<FIXP16_SHIFT);

	if (y1<min_clip_y)
	{
		xs = xs+dx_left*(-y1+min_clip_y);
		xe = xe+dx_right*(-y1+min_clip_y);

		y1=min_clip_y;

	}

	if (y3>max_clip_y)
		y3=max_clip_y;

	dest_addr = (DWORD *)dest_buffer+y1*mempitch;

	if (x1>=min_clip_x && x1<=max_clip_x &&
		x2>=min_clip_x && x2<=max_clip_x &&
		x3>=min_clip_x && x3<=max_clip_x)
	{
		for (temp_y=y1; temp_y<=y3; temp_y++,dest_addr+=mempitch)
		{
			/*memset((DWORD *)dest_addr+((xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT),
			color, (((xe-xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT)+1)<<2);*/

			for (temp_x = ((xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT); temp_x < ((xe+FIXP16_ROUND_UP)>>FIXP16_SHIFT)+1; temp_x ++)
			{
				dest_addr[temp_x] = color;
			}
			xs+=dx_left;
			xe+=dx_right;
		}
	}
	else
	{
		for (temp_y=y1; temp_y<=y3; temp_y++,dest_addr+=mempitch)
		{
			left = ((xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
			right = ((xe+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
			xs+=dx_left;
			xe+=dx_right;

			if (left < min_clip_x)
			{
				left = min_clip_x;

				if (right < min_clip_x)
					continue;
			}

			if (right > max_clip_x)
			{
				right = max_clip_x;

				if (left > max_clip_x)
					continue;
			}

			/*memset((DWORD *)dest_addr+left,
			color, (right-left+1)<<2);*/

			for (temp_x = (int)left; temp_x < (int)right + 1; temp_x ++)
			{
				dest_addr[temp_x] = color;
			}
		}
	}
}

void Draw_Flat_TriangleFP_32(Triangle * face, BYTE * dest_buffer, int mempitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y)
{
	int temp_x,
		temp_y,
		new_x;

	int x1, y1, x2, y2, x3, y3;

	DWORD color;

	x1 = (int)( face->vertex[0]->s_pos->x + 0.5f );
	y1 = (int)( face->vertex[0]->s_pos->y + 0.5f );
	x2 = (int)( face->vertex[1]->s_pos->x + 0.5f );
	y2 = (int)( face->vertex[1]->s_pos->y + 0.5f );
	x3 = (int)( face->vertex[2]->s_pos->x + 0.5f );
	y3 = (int)( face->vertex[2]->s_pos->y + 0.5f );

	color = RGB32BIT( face->vertex[0]->color->alpha, face->vertex[0]->color->red, face->vertex[0]->color->green, face->vertex[0]->color->blue );

	if ((x1==x2 && x2==x3) || (y1==y2 && y2==y3))
		return;

	if (y2<y1)
	{
		temp_x = x2;
		temp_y = y2;
		x2 = x1;
		y2 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if

	if (y3<y1)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x1;
		y3 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if

	if (y3<y2)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x2;
		y3 = y2;
		x2 = temp_x;
		y2 = temp_y;

	} // end if

	if ( y3<min_clip_y || y1>max_clip_y ||
		(x1<min_clip_x && x2<min_clip_x && x3<min_clip_x) ||
		(x1>max_clip_x && x2>max_clip_x && x3>max_clip_x) )
		return;

	if (y1==y2)
	{
		Draw_Top_TriFP(x1,y1,x2,y2,x3,y3,color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y);
	} // end if
	else if (y2==y3)
	{
		Draw_Bottom_TriFP(x1,y1,x2,y2,x3,y3,color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y);
	} // end if bottom is flat
	else
	{
		new_x = x1 + (int)(0.5+(float)(y2-y1)*(float)(x3-x1)/(float)(y3-y1));

		Draw_Bottom_TriFP(x1,y1,new_x,y2,x2,y2,color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y);
		Draw_Top_TriFP(x2,y2,new_x,y2,x3,y3,color, dest_buffer, mempitch, min_clip_x, max_clip_x, min_clip_y, max_clip_y);
	}
}

#endif