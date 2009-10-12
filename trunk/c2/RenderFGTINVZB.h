#ifndef __RENDERFGTINVZB_H
#define __RENDERFGTINVZB_H

#include "Triangle.h"

#define TRI_TYPE_NONE 0
#define TRI_TYPE_FLAT_TOP 1
#define TRI_TYPE_FLAT_BOTTOM 2
#define TRI_TYPE_FLAT_MASK 3
#define TRI_TYPE_GENERAL 4
#define INTERP_LHS 0
#define INTERP_RHS 1

#define RASTERIZER_ACCURATE 0
#define RASTERIZER_FAST 1
#define RASTERIZER_FASTEST 2

#define RASTERIZER_MODE RASTERIZER_ACCURATE

///////////////////////////////////支持1/z深度缓冲////////////////////////////////////
void Draw_Textured_Triangle_INVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dz,
		xi,yi,
		ui,vi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dvdyl,
		vl,
		dzdyl,
		zl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dzdyr,
		zr;

	DWORD zi;

	int x0,y0,tu0,tv0,tz0,
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	x0 = (int)( face->vertex[v0]->s_pos->x );
	y0 = (int)( face->vertex[v0]->s_pos->y );
	tu0 = (int)( face->t_uv[v0]->u );
	tv0 = (int)( face->t_uv[v0]->v );

	tz0 = face->vertex[v0]->fix_inv_z;

	x1 = (int)( face->vertex[v1]->s_pos->x );
	y1 = (int)( face->vertex[v1]->s_pos->y );
	tu1 = (int)( face->t_uv[v1]->u );
	tv1 = (int)( face->t_uv[v1]->v );

	tz1 = face->vertex[v1]->fix_inv_z;

	x2 = (int)( face->vertex[v2]->s_pos->x );
	y2 = (int)( face->vertex[v2]->s_pos->y );
	tu2 = (int)( face->t_uv[v2]->u );
	tv2 = (int)( face->t_uv[v2]->v );

	tz2 = face->vertex[v2]->fix_inv_z;


	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = y2 - y0;

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu1 << FIXP16_SHIFT);
				vr = (tv1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu0 << FIXP16_SHIFT);
				vr = (tv0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];


						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];


						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
	}
}

void Draw_Textured_Bilerp_Triangle_INVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dz,
		xi,yi,
		ui,vi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dvdyl,
		vl,
		dzdyl,
		zl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dzdyr,
		zr;

	DWORD zi;

	int x0,y0,tu0,tv0,tz0,
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2, texture_size;

	DWORD	*screen_ptr = NULL,
			*textmap = NULL,
			*dest_buffer = (DWORD *)_dest_buffer;

	DWORD	*z_ptr = NULL,
			*zbuffer = (DWORD *)_zbuffer;

	DWORD textel00, textel10, textel01, textel11;

	int uint, vint, uint_pls_1, vint_pls_1, r_textel00, g_textel00, b_textel00, r_textel10, g_textel10, b_textel10,
		r_textel01, g_textel01, b_textel01, r_textel11, g_textel11, b_textel11, dtu, dtv, one_minus_dtu, one_minus_dtv, one_minus_dtu_x_one_minus_dtv,
		dtu_x_one_minus_dtv, dtu_x_dtv,one_minus_dtu_x_dtv, r_textel, g_textel, b_textel;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	texture_size = face->texture->mipmaps[face->miplevel]->width - 1;

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	x0 = (int)( face->vertex[v0]->s_pos->x );
	y0 = (int)( face->vertex[v0]->s_pos->y );
	tu0 = (int)( face->t_uv[v0]->u );
	tv0 = (int)( face->t_uv[v0]->v );

	tz0 = face->vertex[v0]->fix_inv_z;

	x1 = (int)( face->vertex[v1]->s_pos->x );
	y1 = (int)( face->vertex[v1]->s_pos->y );
	tu1 = (int)( face->t_uv[v1]->u );
	tv1 = (int)( face->t_uv[v1]->v );

	tz1 = face->vertex[v1]->fix_inv_z;

	x2 = (int)( face->vertex[v2]->s_pos->x );
	y2 = (int)( face->vertex[v2]->s_pos->y );
	tu2 = (int)( face->t_uv[v2]->u );
	tv2 = (int)( face->t_uv[v2]->v );

	tz2 = face->vertex[v2]->fix_inv_z;

	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu1 << FIXP16_SHIFT);
				vr = (tv1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu0 << FIXP16_SHIFT);
				vr = (tv0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						uint = ui >> FIXP16_SHIFT;
						vint = vi >> FIXP16_SHIFT;

						uint_pls_1 = uint+1;
						if (uint_pls_1 > texture_size) uint_pls_1 = texture_size;

						vint_pls_1 = vint+1;
						if (vint_pls_1 > texture_size) vint_pls_1 = texture_size;

						textel00 = textmap[(uint+0) + ((vint+0) << texture_shift2)];
						textel10 = textmap[(uint_pls_1) + ((vint+0) << texture_shift2)];
						textel01 = textmap[(uint+0) + ((vint_pls_1) << texture_shift2)];
						textel11 = textmap[(uint_pls_1) + ((vint_pls_1) << texture_shift2)];

						r_textel00 = ((textel00 >> 16) & 0xff );
						g_textel00 = ((textel00 >> 8) & 0xff);
						b_textel00 = (textel00 & 0xff);

						r_textel10 = ((textel10 >> 16) & 0xff );
						g_textel10 = ((textel10 >> 8) & 0xff);
						b_textel10 = (textel10 & 0xff);

						r_textel01 = ((textel01 >> 16) & 0xff );
						g_textel01 = ((textel01 >> 8) & 0xff);
						b_textel01 = (textel01 & 0xff);

						r_textel11 = ((textel11 >> 16) & 0xff );
						g_textel11 = ((textel11 >> 8) & 0xff);
						b_textel11 = (textel11 & 0xff);

						dtu = (ui & (0xffff)) >> 8;
						dtv = (vi & (0xffff)) >> 8;

						one_minus_dtu = (1 << 8) - dtu;
						one_minus_dtv = (1 << 8) - dtv;

						one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						dtu_x_one_minus_dtv = (dtu) * (one_minus_dtv);
						dtu_x_dtv = (dtu) * (dtv);
						one_minus_dtu_x_dtv = (one_minus_dtu) * (dtv);

						r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 +
							dtu_x_one_minus_dtv * r_textel10 +
							dtu_x_dtv * r_textel11 +
							one_minus_dtu_x_dtv * r_textel01;

						g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 +
							dtu_x_one_minus_dtv * g_textel10 +
							dtu_x_dtv * g_textel11 +
							one_minus_dtu_x_dtv * g_textel01;

						b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 +
							dtu_x_one_minus_dtv * b_textel10 +
							dtu_x_dtv * b_textel11 +
							one_minus_dtu_x_dtv * b_textel01;

						screen_ptr[xi] = RGB32BIT( (textel00 >> 24) & 0xff, r_textel >> FIXP16_SHIFT, g_textel >> FIXP16_SHIFT, b_textel >> FIXP16_SHIFT );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						uint = ui >> FIXP16_SHIFT;
						vint = vi >> FIXP16_SHIFT;

						uint_pls_1 = uint+1;
						if (uint_pls_1 > texture_size) uint_pls_1 = texture_size;

						vint_pls_1 = vint+1;
						if (vint_pls_1 > texture_size) vint_pls_1 = texture_size;

						textel00 = textmap[(uint+0) + ((vint+0) << texture_shift2)];
						textel10 = textmap[(uint_pls_1) + ((vint+0) << texture_shift2)];
						textel01 = textmap[(uint+0) + ((vint_pls_1) << texture_shift2)];
						textel11 = textmap[(uint_pls_1) + ((vint_pls_1) << texture_shift2)];

						r_textel00 = ((textel00 >> 16) & 0xff );
						g_textel00 = ((textel00 >> 8) & 0xff);
						b_textel00 = (textel00 & 0xff);

						r_textel10 = ((textel10 >> 16) & 0xff );
						g_textel10 = ((textel10 >> 8) & 0xff);
						b_textel10 = (textel10 & 0xff);

						r_textel01 = ((textel01 >> 16) & 0xff );
						g_textel01 = ((textel01 >> 8) & 0xff);
						b_textel01 = (textel01 & 0xff);

						r_textel11 = ((textel11 >> 16) & 0xff );
						g_textel11 = ((textel11 >> 8) & 0xff);
						b_textel11 = (textel11 & 0xff);

						dtu = (ui & (0xffff)) >> 8;
						dtv = (vi & (0xffff)) >> 8;

						one_minus_dtu = (1 << 8) - dtu;
						one_minus_dtv = (1 << 8) - dtv;

						one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						dtu_x_one_minus_dtv = (dtu) * (one_minus_dtv);
						dtu_x_dtv = (dtu) * (dtv);
						one_minus_dtu_x_dtv = (one_minus_dtu) * (dtv);

						r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 +
							dtu_x_one_minus_dtv * r_textel10 +
							dtu_x_dtv * r_textel11 +
							one_minus_dtu_x_dtv * r_textel01;

						g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 +
							dtu_x_one_minus_dtv * g_textel10 +
							dtu_x_dtv * g_textel11 +
							one_minus_dtu_x_dtv * g_textel01;

						b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 +
							dtu_x_one_minus_dtv * b_textel10 +
							dtu_x_dtv * b_textel11 +
							one_minus_dtu_x_dtv * b_textel01;

						//screen_ptr[xi] = ((r_textel >> FIXP16_SHIFT) << 11) + ((g_textel >> FIXP16_SHIFT) << 5) + (b_textel >> FIXP16_SHIFT);
						screen_ptr[xi] = RGB32BIT( (textel00 >> 24) & 0xff, r_textel >> FIXP16_SHIFT, g_textel >> FIXP16_SHIFT, b_textel >> FIXP16_SHIFT );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						uint = ui >> FIXP16_SHIFT;
						vint = vi >> FIXP16_SHIFT;

						uint_pls_1 = uint+1;
						if (uint_pls_1 > texture_size) uint_pls_1 = texture_size;

						vint_pls_1 = vint+1;
						if (vint_pls_1 > texture_size) vint_pls_1 = texture_size;

						textel00 = textmap[(uint+0) + ((vint+0) << texture_shift2)];
						textel10 = textmap[(uint_pls_1) + ((vint+0) << texture_shift2)];
						textel01 = textmap[(uint+0) + ((vint_pls_1) << texture_shift2)];
						textel11 = textmap[(uint_pls_1) + ((vint_pls_1) << texture_shift2)];

						r_textel00 = ((textel00 >> 16) & 0xff );
						g_textel00 = ((textel00 >> 8) & 0xff);
						b_textel00 = (textel00 & 0xff);

						r_textel10 = ((textel10 >> 16) & 0xff );
						g_textel10 = ((textel10 >> 8) & 0xff);
						b_textel10 = (textel10 & 0xff);

						r_textel01 = ((textel01 >> 16) & 0xff );
						g_textel01 = ((textel01 >> 8) & 0xff);
						b_textel01 = (textel01 & 0xff);

						r_textel11 = ((textel11 >> 16) & 0xff );
						g_textel11 = ((textel11 >> 8) & 0xff);
						b_textel11 = (textel11 & 0xff);

						dtu = (ui & (0xffff)) >> 8;
						dtv = (vi & (0xffff)) >> 8;

						one_minus_dtu = (1 << 8) - dtu;
						one_minus_dtv = (1 << 8) - dtv;

						one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						dtu_x_one_minus_dtv = (dtu) * (one_minus_dtv);
						dtu_x_dtv = (dtu) * (dtv);
						one_minus_dtu_x_dtv = (one_minus_dtu) * (dtv);

						r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 +
							dtu_x_one_minus_dtv * r_textel10 +
							dtu_x_dtv * r_textel11 +
							one_minus_dtu_x_dtv * r_textel01;

						g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 +
							dtu_x_one_minus_dtv * g_textel10 +
							dtu_x_dtv * g_textel11 +
							one_minus_dtu_x_dtv * g_textel01;

						b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 +
							dtu_x_one_minus_dtv * b_textel10 +
							dtu_x_dtv * b_textel11 +
							one_minus_dtu_x_dtv * b_textel01;

						screen_ptr[xi] = RGB32BIT( (textel00 >> 24) & 0xff, r_textel >> FIXP16_SHIFT, g_textel >> FIXP16_SHIFT, b_textel >> FIXP16_SHIFT );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						uint = ui >> FIXP16_SHIFT;
						vint = vi >> FIXP16_SHIFT;

						uint_pls_1 = uint+1;
						if (uint_pls_1 > texture_size) uint_pls_1 = texture_size;

						vint_pls_1 = vint+1;
						if (vint_pls_1 > texture_size) vint_pls_1 = texture_size;

						textel00 = textmap[(uint+0) + ((vint+0) << texture_shift2)];
						textel10 = textmap[(uint_pls_1) + ((vint+0) << texture_shift2)];
						textel01 = textmap[(uint+0) + ((vint_pls_1) << texture_shift2)];
						textel11 = textmap[(uint_pls_1) + ((vint_pls_1) << texture_shift2)];

						r_textel00 = ((textel00 >> 16) & 0xff );
						g_textel00 = ((textel00 >> 8) & 0xff);
						b_textel00 = (textel00 & 0xff);

						r_textel10 = ((textel10 >> 16) & 0xff );
						g_textel10 = ((textel10 >> 8) & 0xff);
						b_textel10 = (textel10 & 0xff);

						r_textel01 = ((textel01 >> 16) & 0xff );
						g_textel01 = ((textel01 >> 8) & 0xff);
						b_textel01 = (textel01 & 0xff);

						r_textel11 = ((textel11 >> 16) & 0xff );
						g_textel11 = ((textel11 >> 8) & 0xff);
						b_textel11 = (textel11 & 0xff);

						dtu = (ui & (0xffff)) >> 8;
						dtv = (vi & (0xffff)) >> 8;

						one_minus_dtu = (1 << 8) - dtu;
						one_minus_dtv = (1 << 8) - dtv;

						one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						dtu_x_one_minus_dtv = (dtu) * (one_minus_dtv);
						dtu_x_dtv = (dtu) * (dtv);
						one_minus_dtu_x_dtv = (one_minus_dtu) * (dtv);

						r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 +
							dtu_x_one_minus_dtv * r_textel10 +
							dtu_x_dtv * r_textel11 +
							one_minus_dtu_x_dtv * r_textel01;

						g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 +
							dtu_x_one_minus_dtv * g_textel10 +
							dtu_x_dtv * g_textel11 +
							one_minus_dtu_x_dtv * g_textel01;

						b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 +
							dtu_x_one_minus_dtv * b_textel10 +
							dtu_x_dtv * b_textel11 +
							one_minus_dtu_x_dtv * b_textel01;

						screen_ptr[xi] = RGB32BIT( (textel00 >> 24) & 0xff, r_textel >> FIXP16_SHIFT, g_textel >> FIXP16_SHIFT, b_textel >> FIXP16_SHIFT );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
	}
}

void Draw_Textured_Triangle_FSINVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dz,
		xi,yi,
		ui,vi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dzdyl,
		zl,
		dvdyl,
		vl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dzdyr,
		zr;

	DWORD zi;

	DWORD r_base, g_base, b_base,
		r_textel, g_textel, b_textel, textel;

	int x0,y0,tu0,tv0,tz0,
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base = face->vertex[v0]->color->red;
	g_base = face->vertex[v0]->color->green;
	b_base = face->vertex[v0]->color->blue;

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);
	tu0 = (int)(face->t_uv[v0]->u);
	tv0 = (int)(face->t_uv[v0]->v);
	tz0 = face->vertex[v0]->fix_inv_z;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);
	tu1 = (int)(face->t_uv[v1]->u);
	tv1 = (int)(face->t_uv[v1]->v);
	tz1 = face->vertex[v1]->fix_inv_z;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);
	tu2 = (int)(face->t_uv[v2]->u);
	tv2 = (int)(face->t_uv[v2]->v);
	tz2 = face->vertex[v2]->fix_inv_z;

	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{

		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu1 << FIXP16_SHIFT);
				vr = (tv1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu0 << FIXP16_SHIFT);
				vr = (tv0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl; 

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(ui >> (FIXP16_SHIFT)) + ((vi >> (FIXP16_SHIFT)) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl; 

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(ui >> (FIXP16_SHIFT)) + ((vi >> (FIXP16_SHIFT)) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}


		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(ui >> (FIXP16_SHIFT)) + ((vi >> (FIXP16_SHIFT)) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}

				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,z interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl; 

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(ui >> (FIXP16_SHIFT)) + ((vi >> (FIXP16_SHIFT)) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
	}
}

void Draw_Textured_Triangle_GSINVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		dr,dg,db,dz, ds, dt,
		xi,yi,
		ri,gi,bi,si,ti,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		drdyl,
		rl,
		dgdyl,
		gl,
		dbdyl,
		bl,
		dzdyl,
		zl,
		dsdyl,
		sl,
		dtdyl,
		tl,
		drdyr,
		rr,
		dgdyr,
		gr,
		dbdyr,
		br,
		dzdyr,
		zr,
		dsdyr,
		sr,
		dtdyr,
		tr;

	DWORD zi;

	int x0,y0,tr0,tg0,tb0, tz0, ts0,tt0,
		x1,y1,tr1,tg1,tb1, tz1, ts1,tt1,
		x2,y2,tr2,tg2,tb2, tz2, ts2,tt2;

	int r_base0, g_base0, b_base0,
		r_base1, g_base1, b_base1,
		r_base2, g_base2, b_base2;

	int texture_shift2;

	DWORD r_textel, g_textel, b_textel;
	DWORD textel;

	DWORD	*screen_ptr = NULL,
			*textmap = NULL,
			*dest_buffer = (DWORD *)_dest_buffer;

	DWORD	*z_ptr = NULL,
			*zbuffer = (DWORD *)_zbuffer;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base0 = face->vertex[v0]->color->red;
	g_base0 = face->vertex[v0]->color->green;
	b_base0 = face->vertex[v0]->color->blue;

	r_base1 = face->vertex[v1]->color->red;
	g_base1 = face->vertex[v1]->color->green;
	b_base1 = face->vertex[v1]->color->blue;

	r_base2 = face->vertex[v2]->color->red;
	g_base2 = face->vertex[v2]->color->green;
	b_base2 = face->vertex[v2]->color->blue;

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);

	tz0 = face->vertex[v0]->fix_inv_z;
	ts0 = (int)(face->t_uv[v0]->u);
	tt0 = (int)(face->t_uv[v0]->v);

	tr0 = r_base0;
	tg0 = g_base0;
	tb0 = b_base0;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);

	tz1 = face->vertex[v1]->fix_inv_z;
	ts1 = (int)(face->t_uv[v1]->u);
	tt1 = (int)(face->t_uv[v1]->v);

	tr1 = r_base1;
	tg1 = g_base1;
	tb1 = b_base1;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);

	tz2 = face->vertex[v2]->fix_inv_z;
	ts2 = (int)(face->t_uv[v2]->u);
	tt2 = (int)(face->t_uv[v2]->v);

	tr2 = r_base2;
	tg2 = g_base2;
	tb2 = b_base2;

	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			drdyl = ((tr2 - tr0) << FIXP16_SHIFT)/dy;
			dgdyl = ((tg2 - tg0) << FIXP16_SHIFT)/dy;
			dbdyl = ((tb2 - tb0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dsdyl = ((ts2 - ts0) << FIXP16_SHIFT)/dy;
			dtdyl = ((tt2 - tt0) << FIXP16_SHIFT)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			drdyr = ((tr2 - tr1) << FIXP16_SHIFT)/dy;
			dgdyr = ((tg2 - tg1) << FIXP16_SHIFT)/dy;
			dbdyr = ((tb2 - tb1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			dsdyr = ((ts2 - ts1) << FIXP16_SHIFT)/dy;
			dtdyr = ((tt2 - tt1) << FIXP16_SHIFT)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
				gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
				bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
				tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				rr = drdyr*dy + (tr1 << FIXP16_SHIFT);
				gr = dgdyr*dy + (tg1 << FIXP16_SHIFT);
				br = dbdyr*dy + (tb1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				sr = dsdyr*dy + (ts1 << FIXP16_SHIFT);
				tr = dtdyr*dy + (tt1 << FIXP16_SHIFT);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				rl = (tr0 << FIXP16_SHIFT);
				gl = (tg0 << FIXP16_SHIFT);
				bl = (tb0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				sl = (ts0 << FIXP16_SHIFT);
				tl = (tt0 << FIXP16_SHIFT);


				rr = (tr1 << FIXP16_SHIFT);
				gr = (tg1 << FIXP16_SHIFT);
				br = (tb1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				sr = (ts1 << FIXP16_SHIFT);
				tr = (tt1 << FIXP16_SHIFT);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dy;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dy;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dy;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dy;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dy;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dy;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dy;


			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
				gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
				bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
				tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				rr = drdyr*dy + (tr0 << FIXP16_SHIFT);
				gr = dgdyr*dy + (tg0 << FIXP16_SHIFT);
				br = dbdyr*dy + (tb0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
				tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);


				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				rl = (tr0 << FIXP16_SHIFT);
				gl = (tg0 << FIXP16_SHIFT);
				bl = (tb0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				sl = (ts0 << FIXP16_SHIFT);
				tl = (tt0 << FIXP16_SHIFT);

				rr = (tr0 << FIXP16_SHIFT);
				gr = (tg0 << FIXP16_SHIFT);
				br = (tb0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				sr = (ts0 << FIXP16_SHIFT);
				tr = (tt0 << FIXP16_SHIFT);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;
				zi = zl;

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dr = (rr - rl)/dx;
					dg = (gr - gl)/dx;
					db = (br - bl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				}
				else
				{
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ri+=dx*dr;
					gi+=dx*dg;
					bi+=dx*db;
					zi+=dx*dz;

					si+=dx*ds;
					ti+=dx*dt;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=ri;
						g_textel*=gi;
						b_textel*=bi;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						z_ptr[xi] = zi;
					}
					ri+=dr;
					gi+=dg;
					bi+=db;
					zi+=dz;

					si+=ds;
					ti+=dt;

				}

				xl+=dxdyl;
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;
				zi = zl;

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dr = (rr - rl)/dx;
					dg = (gr - gl)/dx;
					db = (br - bl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				}
				else
				{
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=ri;
						g_textel*=gi;
						b_textel*=bi;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						z_ptr[xi] = zi;
					}
					ri+=dr;
					gi+=dg;
					bi+=db;
					zi+=dz;

					si+=ds;
					ti+=dt;

				}

				xl+=dxdyl;
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			rl = drdyl*dyl + (tr1 << FIXP16_SHIFT);
			gl = dgdyl*dyl + (tg1 << FIXP16_SHIFT);
			bl = dbdyl*dyl + (tb1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			sl = dsdyl*dyl + (ts1 << FIXP16_SHIFT);
			tl = dtdyl*dyl + (tt1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			rr = drdyr*dyr + (tr0 << FIXP16_SHIFT);
			gr = dgdyr*dyr + (tg0 << FIXP16_SHIFT);
			br = dbdyr*dyr + (tb0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			sr = dsdyr*dyr + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dyr + (tt0 << FIXP16_SHIFT);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				SWAP(dzdyl,dzdyr,temp);

				SWAP(dsdyl,dsdyr,temp);
				SWAP(dtdyl,dtdyr,temp);

				SWAP(xl,xr,temp);
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				SWAP(zl,zr,temp);

				SWAP(sl,sr,temp);
				SWAP(tl,tr,temp);

				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);
				SWAP(tz1,tz2,temp);

				SWAP(ts1,ts2,temp);
				SWAP(tt1,tt2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
			gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
			bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
			tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			rr = drdyr*dy + (tr0 << FIXP16_SHIFT);
			gr = dgdyr*dy + (tg0 << FIXP16_SHIFT);
			br = dbdyr*dy + (tb0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				SWAP(dzdyl,dzdyr,temp);

				SWAP(dsdyl,dsdyr,temp);
				SWAP(dtdyl,dtdyr,temp);

				SWAP(xl,xr,temp);
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				SWAP(zl,zr,temp);

				SWAP(sl,sr,temp);
				SWAP(tl,tr,temp);

				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);
				SWAP(tz1,tz2,temp);

				SWAP(ts1,ts2,temp);
				SWAP(tt1,tt2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			rl = (tr0 << FIXP16_SHIFT);
			gl = (tg0 << FIXP16_SHIFT);
			bl = (tb0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			sl = (ts0 << FIXP16_SHIFT);
			tl = (tt0 << FIXP16_SHIFT);

			rr = (tr0 << FIXP16_SHIFT);
			gr = (tg0 << FIXP16_SHIFT);
			br = (tb0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			sr = (ts0 << FIXP16_SHIFT);
			tr = (tt0 << FIXP16_SHIFT);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				SWAP(dzdyl,dzdyr,temp);

				SWAP(dsdyl,dsdyr,temp);
				SWAP(dtdyl,dtdyr,temp);


				SWAP(xl,xr,temp);
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				SWAP(zl,zr,temp);

				SWAP(sl,sr,temp);
				SWAP(tl,tr,temp);


				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);
				SWAP(tz1,tz2,temp);

				SWAP(ts1,ts2,temp);
				SWAP(tt1,tt2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;
				zi = zl;

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dr = (rr - rl)/dx;
					dg = (gr - gl)/dx;
					db = (br - bl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				}
				else
				{
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ri+=dx*dr;
					gi+=dx*dg;
					bi+=dx*db;
					zi+=dx*dz;

					si+=dx*ds;
					ti+=dx*dt;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=ri;
						g_textel*=gi;
						b_textel*=bi;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						z_ptr[xi] = zi;
					}
					ri+=dr;
					gi+=dg;
					bi+=db;
					zi+=dz;

					si+=ds;
					ti+=dt;

				}

				xl+=dxdyl;
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
						dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
						dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						rl = (tr1 << FIXP16_SHIFT);
						gl = (tg1 << FIXP16_SHIFT);
						bl = (tb1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						rl+=drdyl;
						gl+=dgdyl;
						bl+=dbdyl;
						zl+=dzdyl;

						sl+=dsdyl;
						tl+=dtdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						drdyr = ((tr1 - tr2) << FIXP16_SHIFT)/dyr;
						dgdyr = ((tg1 - tg2) << FIXP16_SHIFT)/dyr;
						dbdyr = ((tb1 - tb2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						rr = (tr2 << FIXP16_SHIFT);
						gr = (tg2 << FIXP16_SHIFT);
						br = (tb2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						rr+=drdyr;
						gr+=dgdyr;
						br+=dbdyr;
						zr+=dzdyr;

						sr+=dsdyr;
						tr+=dtdyr;
					}

				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;
				zi = zl;

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dr = (rr - rl)/dx;
					dg = (gr - gl)/dx;
					db = (br - bl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				}
				else
				{
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=ri;
						g_textel*=gi;
						b_textel*=bi;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						z_ptr[xi] = zi;
					}
					ri+=dr;
					gi+=dg;
					bi+=db;
					zi+=dz;

					si+=ds;
					ti+=dt;
				}

				xl+=dxdyl;
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
						dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
						dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						rl = (tr1 << FIXP16_SHIFT);
						gl = (tg1 << FIXP16_SHIFT);
						bl = (tb1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						rl+=drdyl;
						gl+=dgdyl;
						bl+=dbdyl;
						zl+=dzdyl;

						sl+=dsdyl;
						tl+=dtdyl;
					}					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						drdyr = ((tr1 - tr2) << FIXP16_SHIFT)/dyr;
						dgdyr = ((tg1 - tg2) << FIXP16_SHIFT)/dyr;
						dbdyr = ((tb1 - tb2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						rr = (tr2 << FIXP16_SHIFT);
						gr = (tg2 << FIXP16_SHIFT);
						br = (tb2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						rr+=drdyr;
						gr+=dgdyr;
						br+=dbdyr;
						zr+=dzdyr;

						sr+=dsdyr;
						tr+=dtdyr;
					}
				}
			}
		}
	}
}

void Draw_Flat_Triangle_INVZB_32( Triangle * face, struct Viewport * viewport )// bytes per line of zbuffer
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		dz,
		xi,yi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dzdyl,
		zl,
		dzdyr,
		zr;

	DWORD zi;

	int x0,y0,tz0,
		x1,y1,tz1,
		x2,y2,tz2;

	DWORD *screen_ptr = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	DWORD color; // polygon color

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);

	tz0 = face->vertex[v0]->fix_inv_z;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);

	tz1 = face->vertex[v1]->fix_inv_z;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);

	tz2 = face->vertex[v2]->fix_inv_z;

	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// extract constant color
	color = RGB32BIT( face->vertex[v0]->color->alpha, face->vertex[v0]->color->red, face->vertex[v0]->color->green, face->vertex[v0]->color->blue );

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{

		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				zl = (tz0 << 0);
				zr = (tz1 << 0);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				zl = (tz0 << 0);
				zr = (tz0 << 0);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = color;

						z_ptr[xi] = zi;
					}

					zi+=dz;
				}

				// interpolate z,x along right and left edge
				xl+=dxdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = color;

						z_ptr[xi] = zi;
					}

					// interpolate z
					zi+=dz;
				}

				// interpolate x,z along right and left edge
				xl+=dxdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			zl = (tz0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for z interpolants
				zi = zl;

				// compute z interpolants
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = color;

						z_ptr[xi] = zi;
					}

					// interpolate z
					zi+=dz;
				}

				// interpolate z,x along right and left edge
				xl+=dxdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						zr+=dzdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl; // + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = color;

						z_ptr[xi] = zi;
					}

					// interpolate z
					zi+=dz;
				}

				// interpolate x,z along right and left edge
				xl+=dxdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						zr+=dzdyr;
					}

				}
			}
		}
	}
}

void Draw_Gouraud_Triangle_INVZB_32( Triangle * face, struct Viewport * viewport )// bytes per line of zbuffer
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dw,dz,
		xi,yi,
		ui,vi,wi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dvdyl,
		vl,
		dwdyl,
		wl,
		dzdyl,
		zl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dwdyr,
		wr,
		dzdyr,
		zr;

	DWORD zi;

	int x0,y0,tu0,tv0,tw0,tz0,
		x1,y1,tu1,tv1,tw1,tz1,
		x2,y2,tu2,tv2,tw2,tz2;

	int r_base0, g_base0, b_base0,
		r_base1, g_base1, b_base1,
		r_base2, g_base2, b_base2,
		a_base;

	DWORD *screen_ptr = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	a_base = face->vertex[v0]->color->alpha;

	r_base0 = face->vertex[v0]->color->red;
	g_base0 = face->vertex[v0]->color->green;
	b_base0 = face->vertex[v0]->color->blue;

	r_base1 = face->vertex[v1]->color->red;
	g_base1 = face->vertex[v1]->color->green;
	b_base1 = face->vertex[v1]->color->blue;

	r_base2 = face->vertex[v2]->color->red;
	g_base2 = face->vertex[v2]->color->green;
	b_base2 = face->vertex[v2]->color->blue;

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);

	tz0 = face->vertex[v0]->fix_inv_z;
	tu0 = r_base0;
	tv0 = g_base0;
	tw0 = b_base0;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);

	tz1 = face->vertex[v1]->fix_inv_z;
	tu1 = r_base1;
	tv1 = g_base1;
	tw1 = b_base1;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);

	tz2 = face->vertex[v2]->fix_inv_z;
	tu2 = r_base2;
	tv2 = g_base2;
	tw2 = b_base2;

	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dwdyl = ((tw2 - tw0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
			dwdyr = ((tw2 - tw1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				wr = dwdyr*dy + (tw1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				wl = (tw0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu1 << FIXP16_SHIFT);
				vr = (tv1 << FIXP16_SHIFT);
				wr = (tw1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				wl = (tw0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu0 << FIXP16_SHIFT);
				vr = (tv0 << FIXP16_SHIFT);
				wr = (tw0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = (a_base << FIXP24_SHIFT) + ((ui >> FIXP16_SHIFT) << 16) + ((vi >> FIXP16_SHIFT) << 8) + (wi >> FIXP16_SHIFT);

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				}

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = (a_base << FIXP24_SHIFT) + ((ui >> FIXP16_SHIFT) << 16) + ((vi >> FIXP16_SHIFT) << 8) + (wi >> FIXP16_SHIFT);

						z_ptr[xi] = zi;
					}


					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);

			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);

			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dwdyl,dwdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(wl,wr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tw1,tw2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dwdyl,dwdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(wl,wr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tw1,tw2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dwdyl,dwdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(wl,wr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tw1,tw2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = (a_base << FIXP24_SHIFT) + ((ui >> FIXP16_SHIFT) << 16) + ((vi >> FIXP16_SHIFT) << 8) + (wi >> FIXP16_SHIFT);

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				}

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = (a_base << FIXP24_SHIFT) + ((ui >> FIXP16_SHIFT) << 16) + ((vi >> FIXP16_SHIFT) << 8) + (wi >> FIXP16_SHIFT);

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					}

				}
			}
		}
	}
}

///////////////////////////////////支持透视矫正////////////////////////////////////
void Draw_Textured_Perspective_Triangle_INVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dz,
		xi,yi,
		ui,vi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dvdyl,
		vl,
		dzdyl,
		zl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dzdyr,
		zr;

	DWORD zi;

	int x0,y0,tu0,tv0,tz0,
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);
	tu0 = ((int)(face->t_uv[v0]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5);
	tv0 = ((int)(face->t_uv[v0]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5);
	tz0 = face->vertex[v0]->fix_inv_z;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);
	tu1 = ((int)(face->t_uv[v1]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5);
	tv1 = ((int)(face->t_uv[v1]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5);
	tz1 = face->vertex[v1]->fix_inv_z;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);
	tu2 = ((int)(face->t_uv[v2]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5);
	tv2 = ((int)(face->t_uv[v2]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5);
	tz2 = face->vertex[v2]->fix_inv_z;


	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];
						//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				}


				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];
						//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				}


				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];
						//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];
						//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
	}
}

void Draw_Textured_PerspectiveLP_Triangle_INVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dz,
		xi,yi,
		ui,vi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dvdyl,
		vl,
		dzdyl,
		zl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dzdyr,
		zr;

	DWORD zi;

	int ul2, ur2, vl2, vr2;

	int x0,y0,tu0,tv0,tz0,
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;
	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);
	tu0 = ((int)(face->t_uv[v0]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5);
	tv0 = ((int)(face->t_uv[v0]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5);
	tz0 = face->vertex[v0]->fix_inv_z;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);
	tu1 = ((int)(face->t_uv[v1]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5);
	tv1 = ((int)(face->t_uv[v1]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5);
	tz1 = face->vertex[v1]->fix_inv_z;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);
	tu2 = ((int)(face->t_uv[v2]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5);
	tv2 = ((int)(face->t_uv[v2]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5);
	tz2 = face->vertex[v2]->fix_inv_z;


	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				zi = zl + 0;
				ui = ul2 + 0;
				vi = vl2 + 0;

				dx = (xend - xstart);

				if ( dx > 0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[(ui >> FIXP22_SHIFT) + ((vi >> FIXP22_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				zi = zl + 0;
				ui = ul2 + 0;
				vi = vl2 + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				}


				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[(ui >> FIXP22_SHIFT) + ((vi >> FIXP22_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;


				zi = zl + 0;
				ui = ul2 + 0;
				vi = vl2 + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				}


				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[(ui >> FIXP22_SHIFT) + ((vi >> FIXP22_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				zi = zl + 0;
				ui = ul2 + 0;
				vi = vl2 + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						screen_ptr[xi] = textmap[(ui >> FIXP22_SHIFT) + ((vi >> FIXP22_SHIFT) << texture_shift2)];

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
	}
}

void Draw_Textured_Perspective_Triangle_FSINVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dz,
		xi,yi,
		ui,vi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dvdyl,
		vl,
		dzdyl,
		zl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dzdyr,
		zr;

	DWORD zi;

	int x0,y0,tu0,tv0,tz0,
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD r_base, g_base, b_base,
		r_textel, g_textel, b_textel, textel;


	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base = face->vertex[v0]->color->red;
	g_base = face->vertex[v0]->color->green;
	b_base = face->vertex[v0]->color->blue;

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);
	tu0 = ((int)(face->t_uv[v0]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5);
	tv0 = ((int)(face->t_uv[v0]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5);
	tz0 = face->vertex[v0]->fix_inv_z;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);
	tu1 = ((int)(face->t_uv[v1]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5);
	tv1 = ((int)(face->t_uv[v1]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5);
	tz1 = face->vertex[v1]->fix_inv_z;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);
	tu2 = ((int)(face->t_uv[v2]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5);
	tv2 = ((int)(face->t_uv[v2]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5);
	tz2 = face->vertex[v2]->fix_inv_z;


	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						//screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						z_ptr[xi] = zi;
					}


					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				}


				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				}


				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
	}
}


void Draw_Textured_PerspectiveLP_Triangle_FSINVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dz,
		xi,yi,
		ui,vi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dvdyl,
		vl,
		dzdyl,
		zl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dzdyr,
		zr;

	DWORD zi;

	int ur2, ul2, vr2, vl2;

	int x0,y0,tu0,tv0,tz0,
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD r_base, g_base, b_base,
		r_textel, g_textel, b_textel, textel;


	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base = face->vertex[v0]->color->red;
	g_base = face->vertex[v0]->color->green;
	b_base = face->vertex[v0]->color->blue;

	x0  = (int)(face->vertex[v0]->s_pos->x);
	y0  = (int)(face->vertex[v0]->s_pos->y);
	tu0 = ((int)(face->t_uv[v0]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5);
	tv0 = ((int)(face->t_uv[v0]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5);
	tz0 = face->vertex[v0]->fix_inv_z;

	x1  = (int)(face->vertex[v1]->s_pos->x);
	y1  = (int)(face->vertex[v1]->s_pos->y);
	tu1 = ((int)(face->t_uv[v1]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5);
	tv1 = ((int)(face->t_uv[v1]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5);
	tz1 = face->vertex[v1]->fix_inv_z;

	x2  = (int)(face->vertex[v2]->s_pos->x);
	y2  = (int)(face->vertex[v2]->s_pos->y);
	tu2 = ((int)(face->t_uv[v2]->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5);
	tv2 = ((int)(face->t_uv[v2]->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5);
	tz2 = face->vertex[v2]->fix_inv_z;

	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;


				zi = zl + 0;
				ui = ul2 + 0;
				vi = vl2 + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ (ui >> FIXP22_SHIFT) + ( (vi >> FIXP22_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >> 8 );

						z_ptr[xi] = zi;
					}


					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				zi = zl + 0;
				ui = ul2 + 0;
				vi = vl2 + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				}


				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ (ui >> FIXP22_SHIFT) + ( (vi >> FIXP22_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >> 8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				zi = zl + 0;
				ui = ul2 + 0;
				vi = vl2 + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				}


				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ (ui >> FIXP22_SHIFT) + ( (vi >> FIXP22_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >> 8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				zi = zl + 0;
				ui = ul2 + 0;
				vi = vl2 + 0;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				}
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ (ui >> FIXP22_SHIFT) + ( (vi >> FIXP22_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >> 8 );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				}
			}
		}
	}
}

void Draw_Textured_Perspective_Triangle_GSINVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dz,
		xi,yi,
		ui,vi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dvdyl,
		vl,
		dzdyl,
		zl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dzdyr,
		zr,
		drdyl,
		dgdyl,
		dbdyl,
		drdyr,
		dgdyr,
		dbdyr,
		rl,rr,
		gl,gr,
		bl,br,
		ri,gi,bi,
		dr,dg,db;

	DWORD zi;

	int x0,y0,tr0,tg0,tb0,tu0,tv0,tz0,
		x1,y1,tr1,tg1,tb1,tu1,tv1,tz1,
		x2,y2,tr2,tg2,tb2,tu2,tv2,tz2;

	int r_base0, g_base0, b_base0,
		r_base1, g_base1, b_base1,
		r_base2, g_base2, b_base2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD r_textel, g_textel, b_textel, textel;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base0 = face->vertex[v0]->color->red;
	g_base0 = face->vertex[v0]->color->green;
	b_base0 = face->vertex[v0]->color->blue;

	r_base1 = face->vertex[v1]->color->red;
	g_base1 = face->vertex[v1]->color->green;
	b_base1 = face->vertex[v1]->color->blue;

	r_base2 = face->vertex[v2]->color->red;
	g_base2 = face->vertex[v2]->color->green;
	b_base2 = face->vertex[v2]->color->blue;

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);
	tu0 = ((int)(face->t_uv[v0]->u + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5f);
	tv0 = ((int)(face->t_uv[v0]->v + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5f);
	tz0 = face->vertex[v0]->fix_inv_z;
	tr0 = r_base0;
	tg0 = g_base0;
	tb0 = b_base0;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);
	tu1 = ((int)(face->t_uv[v1]->u + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5f);
	tv1 = ((int)(face->t_uv[v1]->v + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5f);
	tz1 = face->vertex[v1]->fix_inv_z;
	tr1 = r_base1;
	tg1 = g_base1;
	tb1 = b_base1;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);
	tu2 = ((int)(face->t_uv[v2]->u + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5f);
	tv2 = ((int)(face->t_uv[v2]->v + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5f);
	tz2 = face->vertex[v2]->fix_inv_z;
	tr2 = r_base2;
	tg2 = g_base2;
	tb2 = b_base2;

	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;
			
			drdyl = ((tr2 - tr0) << FIXP16_SHIFT)/dy;
			dgdyl = ((tg2 - tg0) << FIXP16_SHIFT)/dy;
			dbdyl = ((tb2 - tb0) << FIXP16_SHIFT)/dy;

			drdyr = ((tr2 - tr1) << FIXP16_SHIFT)/dy;
			dgdyr = ((tg2 - tg1) << FIXP16_SHIFT)/dy;
			dbdyr = ((tb2 - tb1) << FIXP16_SHIFT)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);
				
				rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
				gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
				bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
				
				rr = drdyr*dy + (tr1 << FIXP16_SHIFT);
				gr = dgdyr*dy + (tg1 << FIXP16_SHIFT);
				br = dbdyr*dy + (tb1 << FIXP16_SHIFT);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);
				
				rl = (tr0 << FIXP16_SHIFT);
				gl = (tg0 << FIXP16_SHIFT);
				bl = (tb0 << FIXP16_SHIFT);
				
				rr = (tr1 << FIXP16_SHIFT);
				gr = (tg1 << FIXP16_SHIFT);
				br = (tb1 << FIXP16_SHIFT);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;
			
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dy;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dy;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dy;
			
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dy;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dy;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);
				
				rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
				gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
				bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
				
				rr = drdyr*dy + (tr0 << FIXP16_SHIFT);
				gr = dgdyr*dy + (tg0 << FIXP16_SHIFT);
				br = dbdyr*dy + (tb0 << FIXP16_SHIFT);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);
				
				rl = (tr0 << FIXP16_SHIFT);
				gl = (tg0 << FIXP16_SHIFT);
				bl = (tb0 << FIXP16_SHIFT);
				
				rr = (tr0 << FIXP16_SHIFT);
				gr = (tg0 << FIXP16_SHIFT);
				br = (tb0 << FIXP16_SHIFT);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;
				
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;

					dr = (rr - rl) / dx;
					dg = (gr - gl) / dx;
					db = (br - bl) / dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
					
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;
					
					ri+=dx*dr;
					gi+=dx*dg;
					bi+=dx*db;

					xstart = min_clip_x;
				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=ri;
						g_textel*=gi;
						b_textel*=bi;

						//screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
					
					ri+=dr;
					gi+=dg;
					bi+=db;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;
				
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;
				
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
					
					dr = (rr - rl) / dx;
					dg = (gr - gl) / dx;
					db = (br - bl) / dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
					
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=ri;
						g_textel*=gi;
						b_textel*=bi;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
					
					ri+=dr;
					gi+=dg;
					bi+=db;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;
				
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;
			
			drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);
			
			rl = drdyl*dyl + (tr1 << FIXP16_SHIFT);
			gl = dgdyl*dyl + (tg1 << FIXP16_SHIFT);
			bl = dbdyl*dyl + (tb1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);
			
			rr = drdyr*dyr + (tr0 << FIXP16_SHIFT);
			gr = dgdyr*dyr + (tg0 << FIXP16_SHIFT);
			br = dbdyr*dyr + (tb0 << FIXP16_SHIFT);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);
				
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;
			
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);
			
			rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
			gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
			bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
			
			rr = drdyr*dy + (tr0 << FIXP16_SHIFT);
			gr = dgdyr*dy + (tg0 << FIXP16_SHIFT);
			br = dbdyr*dy + (tb0 << FIXP16_SHIFT);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);
				
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;
			
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);
			
			rl = (tr0 << FIXP16_SHIFT);
			gl = (tg0 << FIXP16_SHIFT);
			bl = (tb0 << FIXP16_SHIFT);
			
			rr = (tr0 << FIXP16_SHIFT);
			gr = (tg0 << FIXP16_SHIFT);
			br = (tb0 << FIXP16_SHIFT);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);
				
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;
				
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
					
					dr = (rr - rl) / dx;
					dg = (gr - gl) / dx;
					db = (br - bl) / dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
					
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
				}


				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;
					
					ri+=dx*dr;
					gi+=dx*dg;
					bi+=dx*db;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=ri;
						g_textel*=gi;
						b_textel*=bi;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
					
					ri+=dr;
					gi+=dg;
					bi+=db;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;
				
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;
						
						drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
						dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
						dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);
						
						rl = (tr1 << FIXP16_SHIFT);
						gl = (tg1 << FIXP16_SHIFT);
						bl = (tb1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
						
						rl+=drdyl;
						gl+=dgdyl;
						bl+=dbdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						
						drdyr = ((tr1 - tr2) << FIXP16_SHIFT)/dyr;
						dgdyr = ((tg1 - tg2) << FIXP16_SHIFT)/dyr;
						dbdyr = ((tb1 - tb2) << FIXP16_SHIFT)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);
						
						rr = (tr2 << FIXP16_SHIFT);
						gr = (tg2 << FIXP16_SHIFT);
						br = (tb2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
						
						rr+=drdyr;
						gr+=dgdyr;
						br+=dbdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;
				
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
					
					dr = (rr - rl) / dx;
					dg = (gr - gl) / dx;
					db = (br - bl) / dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
					
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel*=ri;
						g_textel*=gi;
						b_textel*=bi;

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
					
					ri+=dr;
					gi+=dg;
					bi+=db;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;
				
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;
						
						drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
						dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
						dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);
						
						rl = (tr1 << FIXP16_SHIFT);
						gl = (tg1 << FIXP16_SHIFT);
						bl = (tb1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
						
						rl+=drdyl;
						gl+=dgdyl;
						bl+=dbdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						
						drdyr = ((tr1 - tr2) << FIXP16_SHIFT)/dyr;
						dgdyr = ((tg1 - tg2) << FIXP16_SHIFT)/dyr;
						dbdyr = ((tb1 - tb2) << FIXP16_SHIFT)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);
						
						rr = (tr2 << FIXP16_SHIFT);
						gr = (tg2 << FIXP16_SHIFT);
						br = (tb2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
						
						rr+=drdyr;
						gr+=dgdyr;
						br+=dbdyr;
					}
				}
			}
		}
	}
}

void Draw_Textured_Perspective_Triangle_FOG_GSINVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;
	Fog * fog = viewport->scene->fog;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		du,dv,dz,
		xi,yi,
		ui,vi,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		dudyl,
		ul,
		dvdyl,
		vl,
		dzdyl,
		zl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dzdyr,
		zr,
		drdyl,
		dgdyl,
		dbdyl,
		drdyr,
		dgdyr,
		dbdyr,
		rl,rr,
		gl,gr,
		bl,br,
		ri,gi,bi,
		dr,dg,db;

	DWORD zi;

	int x0,y0,tr0,tg0,tb0,tu0,tv0,tz0,
		x1,y1,tr1,tg1,tb1,tu1,tv1,tz1,
		x2,y2,tr2,tg2,tb2,tu2,tv2,tz2;

	int r_base0, g_base0, b_base0,
		r_base1, g_base1, b_base1,
		r_base2, g_base2, b_base2,
		f_r, f_g, f_b, f_a;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD r_textel, g_textel, b_textel, textel;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	float f_concentration;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base0 = face->vertex[v0]->color->red;
	g_base0 = face->vertex[v0]->color->green;
	b_base0 = face->vertex[v0]->color->blue;

	r_base1 = face->vertex[v1]->color->red;
	g_base1 = face->vertex[v1]->color->green;
	b_base1 = face->vertex[v1]->color->blue;

	r_base2 = face->vertex[v2]->color->red;
	g_base2 = face->vertex[v2]->color->green;
	b_base2 = face->vertex[v2]->color->blue;

	f_r = fog->global->red;
	f_g = fog->global->green;
	f_b = fog->global->blue;
	f_a = fog->global->alpha;

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);
	tu0 = ((int)(face->t_uv[v0]->u + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5f);
	tv0 = ((int)(face->t_uv[v0]->v + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v0]->s_pos->w + 0.5f);
	tz0 = face->vertex[v0]->fix_inv_z;
	tr0 = r_base0;
	tg0 = g_base0;
	tb0 = b_base0;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);
	tu1 = ((int)(face->t_uv[v1]->u + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5f);
	tv1 = ((int)(face->t_uv[v1]->v + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v1]->s_pos->w + 0.5f);
	tz1 = face->vertex[v1]->fix_inv_z;
	tr1 = r_base1;
	tg1 = g_base1;
	tb1 = b_base1;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);
	tu2 = ((int)(face->t_uv[v2]->u + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5f);
	tv2 = ((int)(face->t_uv[v2]->v + 0.5f) << FIXP22_SHIFT) / (int)(face->vertex[v2]->s_pos->w + 0.5f);
	tz2 = face->vertex[v2]->fix_inv_z;
	tr2 = r_base2;
	tg2 = g_base2;
	tb2 = b_base2;

	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;
			
			drdyl = ((tr2 - tr0) << FIXP16_SHIFT)/dy;
			dgdyl = ((tg2 - tg0) << FIXP16_SHIFT)/dy;
			dbdyl = ((tb2 - tb0) << FIXP16_SHIFT)/dy;

			drdyr = ((tr2 - tr1) << FIXP16_SHIFT)/dy;
			dgdyr = ((tg2 - tg1) << FIXP16_SHIFT)/dy;
			dbdyr = ((tb2 - tb1) << FIXP16_SHIFT)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);
				
				rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
				gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
				bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
				
				rr = drdyr*dy + (tr1 << FIXP16_SHIFT);
				gr = dgdyr*dy + (tg1 << FIXP16_SHIFT);
				br = dbdyr*dy + (tb1 << FIXP16_SHIFT);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);
				
				rl = (tr0 << FIXP16_SHIFT);
				gl = (tg0 << FIXP16_SHIFT);
				bl = (tb0 << FIXP16_SHIFT);
				
				rr = (tr1 << FIXP16_SHIFT);
				gr = (tg1 << FIXP16_SHIFT);
				br = (tb1 << FIXP16_SHIFT);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;
			
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dy;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dy;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dy;
			
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dy;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dy;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);
				
				rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
				gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
				bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
				
				rr = drdyr*dy + (tr0 << FIXP16_SHIFT);
				gr = dgdyr*dy + (tg0 << FIXP16_SHIFT);
				br = dbdyr*dy + (tb0 << FIXP16_SHIFT);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);
				
				rl = (tr0 << FIXP16_SHIFT);
				gl = (tg0 << FIXP16_SHIFT);
				bl = (tb0 << FIXP16_SHIFT);
				
				rr = (tr0 << FIXP16_SHIFT);
				gr = (tg0 << FIXP16_SHIFT);
				br = (tb0 << FIXP16_SHIFT);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;
				
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;

					dr = (rr - rl) / dx;
					dg = (gr - gl) / dx;
					db = (br - bl) / dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
					
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;
					
					ri+=dx*dr;
					gi+=dx*dg;
					bi+=dx*db;

					xstart = min_clip_x;
				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel *= ri;
						g_textel *= gi;
						b_textel *= bi;

						r_textel >>= (FIXP16_SHIFT + 8);
						g_textel >>= (FIXP16_SHIFT + 8);
						b_textel >>= (FIXP16_SHIFT + 8);

						f_concentration = fog->fog_table[( 1 << FIXP28_SHIFT ) / zi];

						r_textel += (int)((f_r - r_textel) * f_concentration);
						g_textel += (int)((f_g - g_textel) * f_concentration);
						b_textel += (int)((f_b - b_textel) * f_concentration);

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel, g_textel, b_textel );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
					
					ri+=dr;
					gi+=dg;
					bi+=db;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;
				
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;
				
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
					
					dr = (rr - rl) / dx;
					dg = (gr - gl) / dx;
					db = (br - bl) / dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
					
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel *= ri;
						g_textel *= gi;
						b_textel *= bi;

						r_textel >>= (FIXP16_SHIFT + 8);
						g_textel >>= (FIXP16_SHIFT + 8);
						b_textel >>= (FIXP16_SHIFT + 8);

						f_concentration = fog->fog_table[( 1 << FIXP28_SHIFT ) / zi];

						r_textel += (int)((f_r - r_textel) * f_concentration);
						g_textel += (int)((f_g - g_textel) * f_concentration);
						b_textel += (int)((f_b - b_textel) * f_concentration);

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel, g_textel, b_textel );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
					
					ri+=dr;
					gi+=dg;
					bi+=db;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;
				
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;
			
			drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);
			
			rl = drdyl*dyl + (tr1 << FIXP16_SHIFT);
			gl = dgdyl*dyl + (tg1 << FIXP16_SHIFT);
			bl = dbdyl*dyl + (tb1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);
			
			rr = drdyr*dyr + (tr0 << FIXP16_SHIFT);
			gr = dgdyr*dyr + (tg0 << FIXP16_SHIFT);
			br = dbdyr*dyr + (tb0 << FIXP16_SHIFT);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);
				
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;
			
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);
			
			rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
			gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
			bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
			
			rr = drdyr*dy + (tr0 << FIXP16_SHIFT);
			gr = dgdyr*dy + (tg0 << FIXP16_SHIFT);
			br = dbdyr*dy + (tb0 << FIXP16_SHIFT);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);
				
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;
			
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);
			
			rl = (tr0 << FIXP16_SHIFT);
			gl = (tg0 << FIXP16_SHIFT);
			bl = (tb0 << FIXP16_SHIFT);
			
			rr = (tr0 << FIXP16_SHIFT);
			gr = (tg0 << FIXP16_SHIFT);
			br = (tb0 << FIXP16_SHIFT);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tz1,tz2,temp);
				
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;
				
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
					
					dr = (rr - rl) / dx;
					dg = (gr - gl) / dx;
					db = (br - bl) / dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
					
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
				}


				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;
					
					ri+=dx*dr;
					gi+=dx*dg;
					bi+=dx*db;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel *= ri;
						g_textel *= gi;
						b_textel *= bi;

						r_textel >>= (FIXP16_SHIFT + 8);
						g_textel >>= (FIXP16_SHIFT + 8);
						b_textel >>= (FIXP16_SHIFT + 8);

						f_concentration = fog->fog_table[( 1 << FIXP28_SHIFT ) / zi];

						r_textel += (int)((f_r - r_textel) * f_concentration);
						g_textel += (int)((f_g - g_textel) * f_concentration);
						b_textel += (int)((f_b - b_textel) * f_concentration);

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel, g_textel, b_textel );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
					
					ri+=dr;
					gi+=dg;
					bi+=db;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;
				
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;
						
						drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
						dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
						dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);
						
						rl = (tr1 << FIXP16_SHIFT);
						gl = (tg1 << FIXP16_SHIFT);
						bl = (tb1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
						
						rl+=drdyl;
						gl+=dgdyl;
						bl+=dbdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						
						drdyr = ((tr1 - tr2) << FIXP16_SHIFT)/dyr;
						dgdyr = ((tg1 - tg2) << FIXP16_SHIFT)/dyr;
						dbdyr = ((tb1 - tb2) << FIXP16_SHIFT)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);
						
						rr = (tr2 << FIXP16_SHIFT);
						gr = (tg2 << FIXP16_SHIFT);
						br = (tb2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
						
						rr+=drdyr;
						gr+=dgdyr;
						br+=dbdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + 0;
				ui = ul + 0;
				vi = vl + 0;
				
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
					
					dr = (rr - rl) / dx;
					dg = (gr - gl) / dx;
					db = (br - bl) / dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
					
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel *= ri;
						g_textel *= gi;
						b_textel *= bi;

						r_textel >>= (FIXP16_SHIFT + 8);
						g_textel >>= (FIXP16_SHIFT + 8);
						b_textel >>= (FIXP16_SHIFT + 8);

						f_concentration = fog->fog_table[( 1 << FIXP28_SHIFT ) / zi];

						r_textel += (int)((f_r - r_textel) * f_concentration);
						g_textel += (int)((f_g - g_textel) * f_concentration);
						b_textel += (int)((f_b - b_textel) * f_concentration);

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel, g_textel, b_textel );

						z_ptr[xi] = zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
					
					ri+=dr;
					gi+=dg;
					bi+=db;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;
				
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;
						
						drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
						dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
						dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);
						
						rl = (tr1 << FIXP16_SHIFT);
						gl = (tg1 << FIXP16_SHIFT);
						bl = (tb1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
						
						rl+=drdyl;
						gl+=dgdyl;
						bl+=dbdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						
						drdyr = ((tr1 - tr2) << FIXP16_SHIFT)/dyr;
						dgdyr = ((tg1 - tg2) << FIXP16_SHIFT)/dyr;
						dbdyr = ((tb1 - tb2) << FIXP16_SHIFT)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);
						
						rr = (tr2 << FIXP16_SHIFT);
						gr = (tg2 << FIXP16_SHIFT);
						br = (tb2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
						
						rr+=drdyr;
						gr+=dgdyr;
						br+=dbdyr;
					}
				}
			}
		}
	}
}


void Draw_Textured_Triangle_FOG_GSINVZB_32( Triangle * face, struct Viewport * viewport )
{
	BYTE * _dest_buffer = viewport->videoBuffer;
	BYTE * _zbuffer = viewport->zBuffer;
	Fog * fog = viewport->scene->fog;

	int mempitch = viewport->mempitch;
	int zpitch = viewport->zpitch;
	int min_clip_x = 0;
	int max_clip_x = viewport->width;
	int min_clip_y = 0;
	int max_clip_y = viewport->height;

	int temp=0,
		v0=0,
		v1=1,
		v2=2,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr,
		dr,dg,db,dz, ds, dt,
		xi,yi,
		ri,gi,bi,si,ti,
		xstart,
		xend,
		ystart,
		yrestart,
		yend,
		xl,
		dxdyl,
		xr,
		dxdyr,
		drdyl,
		rl,
		dgdyl,
		gl,
		dbdyl,
		bl,
		dzdyl,
		zl,
		dsdyl,
		sl,
		dtdyl,
		tl,
		drdyr,
		rr,
		dgdyr,
		gr,
		dbdyr,
		br,
		dzdyr,
		zr,
		dsdyr,
		sr,
		dtdyr,
		tr;

	DWORD zi;

	int x0,y0,tr0,tg0,tb0, tz0, ts0,tt0,
		x1,y1,tr1,tg1,tb1, tz1, ts1,tt1,
		x2,y2,tr2,tg2,tb2, tz2, ts2,tt2;

	int r_base0, g_base0, b_base0,
		r_base1, g_base1, b_base1,
		r_base2, g_base2, b_base2,
		f_r, f_g, f_b, f_a;

	int texture_shift2;

	DWORD r_textel, g_textel, b_textel;
	DWORD textel;

	DWORD	*screen_ptr = NULL,
			*textmap = NULL,
			*dest_buffer = (DWORD *)_dest_buffer;

	DWORD	*z_ptr = NULL,
			*zbuffer = (DWORD *)_zbuffer;
	
	float f_concentration;

	textmap = (DWORD *)face->texture->mipmaps[face->miplevel]->pRGBABuffer;

	texture_shift2 = logbase2ofx[face->texture->mipmaps[face->miplevel]->width];

	mempitch >>= 2;

	zpitch >>= 2;

	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.0f));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.0f));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.0f));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.0f));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.0f));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.0f));

	if (((face->vertex[0]->s_pos->y < min_clip_y) &&
		(face->vertex[1]->s_pos->y < min_clip_y) &&
		(face->vertex[2]->s_pos->y < min_clip_y)) ||

		((face->vertex[0]->s_pos->y > max_clip_y) &&
		(face->vertex[1]->s_pos->y > max_clip_y) &&
		(face->vertex[2]->s_pos->y > max_clip_y)) ||

		((face->vertex[0]->s_pos->x < min_clip_x) &&
		(face->vertex[1]->s_pos->x < min_clip_x) &&
		(face->vertex[2]->s_pos->x < min_clip_x)) ||

		((face->vertex[0]->s_pos->x > max_clip_x) &&
		(face->vertex[1]->s_pos->x > max_clip_x) &&
		(face->vertex[2]->s_pos->x > max_clip_x)))
		return;

	if ( face->vertex[v1]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v1,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v0]->s_pos->y )
	{
		SWAP(v0,v2,temp);
	}

	if ( face->vertex[v2]->s_pos->y < face->vertex[v1]->s_pos->y )
	{
		SWAP(v1,v2,temp);
	}

	if (FCMP(face->vertex[v0]->s_pos->y, face->vertex[v1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[v1]->s_pos->x < face->vertex[v0]->s_pos->x)
		{
			SWAP(v0,v1,temp);
		}
	}
	else if (FCMP(face->vertex[v1]->s_pos->y ,face->vertex[v2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[v2]->s_pos->x < face->vertex[v1]->s_pos->x)
		{
			SWAP(v1,v2,temp);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base0 = face->vertex[v0]->color->red;
	g_base0 = face->vertex[v0]->color->green;
	b_base0 = face->vertex[v0]->color->blue;

	r_base1 = face->vertex[v1]->color->red;
	g_base1 = face->vertex[v1]->color->green;
	b_base1 = face->vertex[v1]->color->blue;

	r_base2 = face->vertex[v2]->color->red;
	g_base2 = face->vertex[v2]->color->green;
	b_base2 = face->vertex[v2]->color->blue;

	f_r = fog->global->red;
	f_g = fog->global->green;
	f_b = fog->global->blue;
	f_a = fog->global->alpha;

	x0 = (int)(face->vertex[v0]->s_pos->x);
	y0 = (int)(face->vertex[v0]->s_pos->y);

	tz0 = face->vertex[v0]->fix_inv_z;
	ts0 = (int)(face->t_uv[v0]->u);
	tt0 = (int)(face->t_uv[v0]->v);

	tr0 = r_base0;
	tg0 = g_base0;
	tb0 = b_base0;

	x1 = (int)(face->vertex[v1]->s_pos->x);
	y1 = (int)(face->vertex[v1]->s_pos->y);

	tz1 = face->vertex[v1]->fix_inv_z;
	ts1 = (int)(face->t_uv[v1]->u);
	tt1 = (int)(face->t_uv[v1]->v);

	tr1 = r_base1;
	tg1 = g_base1;
	tb1 = b_base1;

	x2 = (int)(face->vertex[v2]->s_pos->x);
	y2 = (int)(face->vertex[v2]->s_pos->y);

	tz2 = face->vertex[v2]->fix_inv_z;
	ts2 = (int)(face->t_uv[v2]->u);
	tt2 = (int)(face->t_uv[v2]->v);

	tr2 = r_base2;
	tg2 = g_base2;
	tb2 = b_base2;

	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	yrestart = y1;

	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			drdyl = ((tr2 - tr0) << FIXP16_SHIFT)/dy;
			dgdyl = ((tg2 - tg0) << FIXP16_SHIFT)/dy;
			dbdyl = ((tb2 - tb0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dsdyl = ((ts2 - ts0) << FIXP16_SHIFT)/dy;
			dtdyl = ((tt2 - tt0) << FIXP16_SHIFT)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			drdyr = ((tr2 - tr1) << FIXP16_SHIFT)/dy;
			dgdyr = ((tg2 - tg1) << FIXP16_SHIFT)/dy;
			dbdyr = ((tb2 - tb1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			dsdyr = ((ts2 - ts1) << FIXP16_SHIFT)/dy;
			dtdyr = ((tt2 - tt1) << FIXP16_SHIFT)/dy;

			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
				gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
				bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
				tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				rr = drdyr*dy + (tr1 << FIXP16_SHIFT);
				gr = dgdyr*dy + (tg1 << FIXP16_SHIFT);
				br = dbdyr*dy + (tb1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				sr = dsdyr*dy + (ts1 << FIXP16_SHIFT);
				tr = dtdyr*dy + (tt1 << FIXP16_SHIFT);

				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				rl = (tr0 << FIXP16_SHIFT);
				gl = (tg0 << FIXP16_SHIFT);
				bl = (tb0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				sl = (ts0 << FIXP16_SHIFT);
				tl = (tt0 << FIXP16_SHIFT);


				rr = (tr1 << FIXP16_SHIFT);
				gr = (tg1 << FIXP16_SHIFT);
				br = (tb1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				sr = (ts1 << FIXP16_SHIFT);
				tr = (tt1 << FIXP16_SHIFT);

				ystart = y0;
			}
		}
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dy;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dy;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dy;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dy;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dy;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dy;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dy;


			if (y0 < min_clip_y)
			{
				dy = (min_clip_y - y0);

				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
				gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
				bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
				tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				rr = drdyr*dy + (tr0 << FIXP16_SHIFT);
				gr = dgdyr*dy + (tg0 << FIXP16_SHIFT);
				br = dbdyr*dy + (tb0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
				tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);


				ystart = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				rl = (tr0 << FIXP16_SHIFT);
				gl = (tg0 << FIXP16_SHIFT);
				bl = (tb0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				sl = (ts0 << FIXP16_SHIFT);
				tl = (tt0 << FIXP16_SHIFT);

				rr = (tr0 << FIXP16_SHIFT);
				gr = (tg0 << FIXP16_SHIFT);
				br = (tb0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				sr = (ts0 << FIXP16_SHIFT);
				tr = (tt0 << FIXP16_SHIFT);

				ystart = y0;
			}
		}

		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;
				zi = zl;

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dr = (rr - rl)/dx;
					dg = (gr - gl)/dx;
					db = (br - bl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				}
				else
				{
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ri+=dx*dr;
					gi+=dx*dg;
					bi+=dx*db;
					zi+=dx*dz;

					si+=dx*ds;
					ti+=dx*dt;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel *= ri;
						g_textel *= gi;
						b_textel *= bi;

						r_textel >>= (FIXP16_SHIFT + 8);
						g_textel >>= (FIXP16_SHIFT + 8);
						b_textel >>= (FIXP16_SHIFT + 8);

						f_concentration = fog->fog_table[( 1 << FIXP28_SHIFT ) / zi];

						r_textel += (int)((f_r - r_textel) * f_concentration);
						g_textel += (int)((f_g - g_textel) * f_concentration);
						b_textel += (int)((f_b - b_textel) * f_concentration);

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel, g_textel, b_textel );

						z_ptr[xi] = zi;
					}
					ri+=dr;
					gi+=dg;
					bi+=db;
					zi+=dz;

					si+=ds;
					ti+=dt;

				}

				xl+=dxdyl;
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;
				zi = zl;

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dr = (rr - rl)/dx;
					dg = (gr - gl)/dx;
					db = (br - bl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				}
				else
				{
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel *= ri;
						g_textel *= gi;
						b_textel *= bi;

						r_textel >>= (FIXP16_SHIFT + 8);
						g_textel >>= (FIXP16_SHIFT + 8);
						b_textel >>= (FIXP16_SHIFT + 8);

						f_concentration = fog->fog_table[( 1 << FIXP28_SHIFT ) / zi];

						r_textel += (int)((f_r - r_textel) * f_concentration);
						g_textel += (int)((f_g - g_textel) * f_concentration);
						b_textel += (int)((f_b - b_textel) * f_concentration);

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel, g_textel, b_textel );

						z_ptr[xi] = zi;
					}
					ri+=dr;
					gi+=dg;
					bi+=db;
					zi+=dz;

					si+=ds;
					ti+=dt;

				}

				xl+=dxdyl;
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;
			}
		}

	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		if (y1 < min_clip_y)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			rl = drdyl*dyl + (tr1 << FIXP16_SHIFT);
			gl = dgdyl*dyl + (tg1 << FIXP16_SHIFT);
			bl = dbdyl*dyl + (tb1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			sl = dsdyl*dyl + (ts1 << FIXP16_SHIFT);
			tl = dtdyl*dyl + (tt1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			rr = drdyr*dyr + (tr0 << FIXP16_SHIFT);
			gr = dgdyr*dyr + (tg0 << FIXP16_SHIFT);
			br = dbdyr*dyr + (tb0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			sr = dsdyr*dyr + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dyr + (tt0 << FIXP16_SHIFT);

			ystart = min_clip_y;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				SWAP(dzdyl,dzdyr,temp);

				SWAP(dsdyl,dsdyr,temp);
				SWAP(dtdyl,dtdyr,temp);

				SWAP(xl,xr,temp);
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				SWAP(zl,zr,temp);

				SWAP(sl,sr,temp);
				SWAP(tl,tr,temp);

				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);
				SWAP(tz1,tz2,temp);

				SWAP(ts1,ts2,temp);
				SWAP(tt1,tt2,temp);

				irestart = INTERP_RHS;
			}
		}
		else if (y0 < min_clip_y)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dy = (min_clip_y - y0);

			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			rl = drdyl*dy + (tr0 << FIXP16_SHIFT);
			gl = dgdyl*dy + (tg0 << FIXP16_SHIFT);
			bl = dbdyl*dy + (tb0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
			tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			rr = drdyr*dy + (tr0 << FIXP16_SHIFT);
			gr = dgdyr*dy + (tg0 << FIXP16_SHIFT);
			br = dbdyr*dy + (tb0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);

			ystart = min_clip_y;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				SWAP(dzdyl,dzdyr,temp);

				SWAP(dsdyl,dsdyr,temp);
				SWAP(dtdyl,dtdyr,temp);

				SWAP(xl,xr,temp);
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				SWAP(zl,zr,temp);

				SWAP(sl,sr,temp);
				SWAP(tl,tr,temp);

				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);
				SWAP(tz1,tz2,temp);

				SWAP(ts1,ts2,temp);
				SWAP(tt1,tt2,temp);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			drdyl = ((tr1 - tr0) << FIXP16_SHIFT)/dyl;
			dgdyl = ((tg1 - tg0) << FIXP16_SHIFT)/dyl;
			dbdyl = ((tb1 - tb0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			drdyr = ((tr2 - tr0) << FIXP16_SHIFT)/dyr;
			dgdyr = ((tg2 - tg0) << FIXP16_SHIFT)/dyr;
			dbdyr = ((tb2 - tb0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			rl = (tr0 << FIXP16_SHIFT);
			gl = (tg0 << FIXP16_SHIFT);
			bl = (tb0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			sl = (ts0 << FIXP16_SHIFT);
			tl = (tt0 << FIXP16_SHIFT);

			rr = (tr0 << FIXP16_SHIFT);
			gr = (tg0 << FIXP16_SHIFT);
			br = (tb0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			sr = (ts0 << FIXP16_SHIFT);
			tr = (tt0 << FIXP16_SHIFT);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(drdyl,drdyr,temp);
				SWAP(dgdyl,dgdyr,temp);
				SWAP(dbdyl,dbdyr,temp);
				SWAP(dzdyl,dzdyr,temp);

				SWAP(dsdyl,dsdyr,temp);
				SWAP(dtdyl,dtdyr,temp);


				SWAP(xl,xr,temp);
				SWAP(rl,rr,temp);
				SWAP(gl,gr,temp);
				SWAP(bl,br,temp);
				SWAP(zl,zr,temp);

				SWAP(sl,sr,temp);
				SWAP(tl,tr,temp);


				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tr1,tr2,temp);
				SWAP(tg1,tg2,temp);
				SWAP(tb1,tb2,temp);
				SWAP(tz1,tz2,temp);

				SWAP(ts1,ts2,temp);
				SWAP(tt1,tt2,temp);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;
				zi = zl;

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dr = (rr - rl)/dx;
					dg = (gr - gl)/dx;
					db = (br - bl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				}
				else
				{
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				}

				///////////////////////////////////////////////////////////////////////

				if (xstart < min_clip_x)
				{
					dx = min_clip_x - xstart;

					ri+=dx*dr;
					gi+=dx*dg;
					bi+=dx*db;
					zi+=dx*dz;

					si+=dx*ds;
					ti+=dx*dt;

					xstart = min_clip_x;

				}

				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel *= ri;
						g_textel *= gi;
						b_textel *= bi;

						r_textel >>= (FIXP16_SHIFT + 8);
						g_textel >>= (FIXP16_SHIFT + 8);
						b_textel >>= (FIXP16_SHIFT + 8);

						f_concentration = fog->fog_table[( 1 << FIXP28_SHIFT ) / zi];

						r_textel += (int)((f_r - r_textel) * f_concentration);
						g_textel += (int)((f_g - g_textel) * f_concentration);
						b_textel += (int)((f_b - b_textel) * f_concentration);

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel, g_textel, b_textel );

						z_ptr[xi] = zi;
					}
					ri+=dr;
					gi+=dg;
					bi+=db;
					zi+=dz;

					si+=ds;
					ti+=dt;

				}

				xl+=dxdyl;
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
						dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
						dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						rl = (tr1 << FIXP16_SHIFT);
						gl = (tg1 << FIXP16_SHIFT);
						bl = (tb1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						rl+=drdyl;
						gl+=dgdyl;
						bl+=dbdyl;
						zl+=dzdyl;

						sl+=dsdyl;
						tl+=dtdyl;
					}
					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						drdyr = ((tr1 - tr2) << FIXP16_SHIFT)/dyr;
						dgdyr = ((tg1 - tg2) << FIXP16_SHIFT)/dyr;
						dbdyr = ((tb1 - tb2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						rr = (tr2 << FIXP16_SHIFT);
						gr = (tg2 << FIXP16_SHIFT);
						br = (tb2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						rr+=drdyr;
						gr+=dgdyr;
						br+=dbdyr;
						zr+=dzdyr;

						sr+=dsdyr;
						tr+=dtdyr;
					}

				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * mempitch);

			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ri = rl + FIXP16_ROUND_UP;
				gi = gl + FIXP16_ROUND_UP;
				bi = bl + FIXP16_ROUND_UP;
				zi = zl;

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dr = (rr - rl)/dx;
					dg = (gr - gl)/dx;
					db = (br - bl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				}
				else
				{
					dr = (rr - rl);
					dg = (gr - gl);
					db = (br - bl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				}

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > z_ptr[xi])
					{
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						r_textel *= ri;
						g_textel *= gi;
						b_textel *= bi;

						r_textel >>= (FIXP16_SHIFT + 8);
						g_textel >>= (FIXP16_SHIFT + 8);
						b_textel >>= (FIXP16_SHIFT + 8);

						f_concentration = fog->fog_table[( 1 << FIXP28_SHIFT ) / zi];

						r_textel += (int)((f_r - r_textel) * f_concentration);
						g_textel += (int)((f_g - g_textel) * f_concentration);
						b_textel += (int)((f_b - b_textel) * f_concentration);

						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel, g_textel, b_textel );

						z_ptr[xi] = zi;
					}
					ri+=dr;
					gi+=dg;
					bi+=db;
					zi+=dz;

					si+=ds;
					ti+=dt;
				}

				xl+=dxdyl;
				rl+=drdyl;
				gl+=dgdyl;
				bl+=dbdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				rr+=drdyr;
				gr+=dgdyr;
				br+=dbdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				screen_ptr+=mempitch;

				z_ptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						drdyl = ((tr2 - tr1) << FIXP16_SHIFT)/dyl;
						dgdyl = ((tg2 - tg1) << FIXP16_SHIFT)/dyl;
						dbdyl = ((tb2 - tb1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						rl = (tr1 << FIXP16_SHIFT);
						gl = (tg1 << FIXP16_SHIFT);
						bl = (tb1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						rl+=drdyl;
						gl+=dgdyl;
						bl+=dbdyl;
						zl+=dzdyl;

						sl+=dsdyl;
						tl+=dtdyl;
					}					else
					{
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						drdyr = ((tr1 - tr2) << FIXP16_SHIFT)/dyr;
						dgdyr = ((tg1 - tg2) << FIXP16_SHIFT)/dyr;
						dbdyr = ((tb1 - tb2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2 << FIXP16_SHIFT);
						rr = (tr2 << FIXP16_SHIFT);
						gr = (tg2 << FIXP16_SHIFT);
						br = (tb2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						rr+=drdyr;
						gr+=dgdyr;
						br+=dbdyr;
						zr+=dzdyr;

						sr+=dsdyr;
						tr+=dtdyr;
					}
				}
			}
		}
	}
}

# endif
