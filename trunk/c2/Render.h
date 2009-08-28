#ifndef __RENDER_H
#define __RENDER_H

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
void Draw_Textured_Triangle_INVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
{
	void * tmp_ptr;

	int temp = 0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		du,dv,dz,
		xi,yi, // the current interpolated x,y
		ui,vi, // the current interpolated u,v,z
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

	int x0,y0,tu0,tv0,tz0, // cached vertices
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	// extract texture map
	textmap = (DWORD *)face->texture->pRGBABuffer;

	// extract base 2 of texture width
	texture_shift2 = logbase2ofx[face->texture->width];

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	// extract vertices for processing, now that we have order
	x0 = (int)( face->vertex[0]->s_pos->x );
	y0 = (int)( face->vertex[0]->s_pos->y );
	tu0 = (int)( face->vertex[0]->uv->u );
	tv0 = (int)( face->vertex[0]->uv->v );

	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5 );

	x1 = (int)( face->vertex[1]->s_pos->x );
	y1 = (int)( face->vertex[1]->s_pos->y );
	tu1 = (int)( face->vertex[1]->uv->u );
	tv1 = (int)( face->vertex[1]->uv->v );

	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5 );

	x2 = (int)( face->vertex[2]->s_pos->x );
	y2 = (int)( face->vertex[2]->s_pos->y );
	tu2 = (int)( face->vertex[2]->uv->u );
	tv2 = (int)( face->vertex[2]->uv->v );

	tz2 = (1 << FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5 );


	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
			dy = y2 - y0;

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu1 << FIXP16_SHIFT);
				vr = (tv1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				// set starting y
				ystart = y0;

			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu0 << FIXP16_SHIFT);
				vr = (tv0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				// set starting y
				ystart = y0;

			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ????

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ????

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// no clipping y

			// set starting values
			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ????

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Textured_Triangle_INVZB_16

void Draw_Textured_Bilerp_Triangle_INVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
{
	void * tmp_ptr;

	int temp=0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		du,dv,dz,
		xi,yi, // the current interpolated x,y
		ui,vi, // the current interpolated u,v,z
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

	int x0,y0,tu0,tv0,tz0, // cached vertices
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

	// extract texture map
	textmap = (DWORD *)face->texture->pRGBABuffer;

	// extract base 2 of texture width
	texture_shift2 = logbase2ofx[face->texture->width];

	// compute actual size of texture and store it
	texture_size = face->texture->width - 1;

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	// extract vertices for processing, now that we have order
	x0 = (int)( face->vertex[0]->s_pos->x );
	y0 = (int)( face->vertex[0]->s_pos->y );
	tu0 = (int)( face->vertex[0]->uv->u );
	tv0 = (int)( face->vertex[0]->uv->v );

	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5 );

	x1 = (int)( face->vertex[1]->s_pos->x );
	y1 = (int)( face->vertex[1]->s_pos->y );
	tu1 = (int)( face->vertex[1]->uv->u );
	tv1 = (int)( face->vertex[1]->uv->v );

	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5 );

	x2 = (int)( face->vertex[2]->s_pos->x );
	y2 = (int)( face->vertex[2]->s_pos->y );
	tu2 = (int)( face->vertex[2]->uv->u );
	tv2 = (int)( face->vertex[2]->uv->v );

	tz2 = (1 << FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5 );

	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu1 << FIXP16_SHIFT);
				vr = (tv1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				// set starting y
				ystart = y0;

			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu0 << FIXP16_SHIFT);
				vr = (tv0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				// set starting y
				ystart = y0;

			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ????

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// compute integral values of u,v
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

						// extract rgb components
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

						// compute fractional components of u,v in fixed 24.8 point format
						dtu = (ui & (0xffff)) >> 8;
						dtv = (vi & (0xffff)) >> 8;

						one_minus_dtu = (1 << 8) - dtu;
						one_minus_dtv = (1 << 8) - dtv;

						// each interpolant has 3 terms, (du), (dv), textel, however
						// the (du) and (dv) terms repeat during each computation of
						// r_textel, g_textel, and b_textel, so we can compute them once
						one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						dtu_x_one_minus_dtv = (dtu) * (one_minus_dtv);
						dtu_x_dtv = (dtu) * (dtv);
						one_minus_dtu_x_dtv = (one_minus_dtu) * (dtv);

						// now we are ready to sample the texture
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

						// write textel
						screen_ptr[xi] = RGB32BIT( (textel00 >> 24) & 0xff, r_textel >> FIXP16_SHIFT, g_textel >> FIXP16_SHIFT, b_textel >> FIXP16_SHIFT );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ????

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// compute integral values of u,v
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

						// extract rgb components
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

						// compute fractional components of u,v in fixed 24.8 point format
						dtu = (ui & (0xffff)) >> 8;
						dtv = (vi & (0xffff)) >> 8;

						one_minus_dtu = (1 << 8) - dtu;
						one_minus_dtv = (1 << 8) - dtv;

						// each interpolant has 3 terms, (du), (dv), textel, however
						// the (du) and (dv) terms repeat during each computation of
						// r_textel, g_textel, and b_textel, so we can compute them once
						one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						dtu_x_one_minus_dtv = (dtu) * (one_minus_dtv);
						dtu_x_dtv = (dtu) * (dtv);
						one_minus_dtu_x_dtv = (one_minus_dtu) * (dtv);

						// now we are ready to sample the texture
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

						// write textel
						//screen_ptr[xi] = ((r_textel >> FIXP16_SHIFT) << 11) + ((g_textel >> FIXP16_SHIFT) << 5) + (b_textel >> FIXP16_SHIFT);
						screen_ptr[xi] = RGB32BIT( (textel00 >> 24) & 0xff, r_textel >> FIXP16_SHIFT, g_textel >> FIXP16_SHIFT, b_textel >> FIXP16_SHIFT );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// no clipping y

			// set starting values
			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// compute integral values of u,v
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

						// extract rgb components
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

						// compute fractional components of u,v in fixed 24.8 point format
						dtu = (ui & (0xffff)) >> 8;
						dtv = (vi & (0xffff)) >> 8;

						one_minus_dtu = (1 << 8) - dtu;
						one_minus_dtv = (1 << 8) - dtv;

						// each interpolant has 3 terms, (du), (dv), textel, however
						// the (du) and (dv) terms repeat during each computation of
						// r_textel, g_textel, and b_textel, so we can compute them once
						one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						dtu_x_one_minus_dtv = (dtu) * (one_minus_dtv);
						dtu_x_dtv = (dtu) * (dtv);
						one_minus_dtu_x_dtv = (one_minus_dtu) * (dtv);

						// now we are ready to sample the texture
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

						// write textel
						screen_ptr[xi] = RGB32BIT( (textel00 >> 24) & 0xff, r_textel >> FIXP16_SHIFT, g_textel >> FIXP16_SHIFT, b_textel >> FIXP16_SHIFT );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ????

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// compute integral values of u,v
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

						// extract rgb components
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

						// compute fractional components of u,v in fixed 24.8 point format
						dtu = (ui & (0xffff)) >> 8;
						dtv = (vi & (0xffff)) >> 8;

						one_minus_dtu = (1 << 8) - dtu;
						one_minus_dtv = (1 << 8) - dtv;

						// each interpolant has 3 terms, (du), (dv), textel, however
						// the (du) and (dv) terms repeat during each computation of
						// r_textel, g_textel, and b_textel, so we can compute them once
						one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						dtu_x_one_minus_dtv = (dtu) * (one_minus_dtv);
						dtu_x_dtv = (dtu) * (dtv);
						one_minus_dtu_x_dtv = (one_minus_dtu) * (dtv);

						// now we are ready to sample the texture
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

						// write textel
						screen_ptr[xi] = RGB32BIT( (textel00 >> 24) & 0xff, r_textel >> FIXP16_SHIFT, g_textel >> FIXP16_SHIFT, b_textel >> FIXP16_SHIFT );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Textured_Bilerp_Triangle_INVZB_16

void Draw_Textured_Triangle_FSINVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
{
	void * tmp_ptr;

	int temp=0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		du,dv,dz,
		xi,yi, // the current interpolated x,y
		ui,vi, // the current interpolated u,v,z
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

	int x0,y0,tu0,tv0,tz0, // cached vertices
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	// extract texture map
	textmap = (DWORD *)face->texture->pRGBABuffer;

	// extract base 2 of texture width
	texture_shift2 = logbase2ofx[face->texture->width];

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base = face->vertex[0]->color->red;
	g_base = face->vertex[0]->color->green;
	b_base = face->vertex[0]->color->blue;

	// extract vertices for processing, now that we have order
	x0 = (int)(face->vertex[0]->s_pos->x);
	y0 = (int)(face->vertex[0]->s_pos->y);
	tu0 = (int)(face->vertex[0]->uv->u);
	tv0 = (int)(face->vertex[0]->uv->v);
	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);

	x1 = (int)(face->vertex[1]->s_pos->x);
	y1 = (int)(face->vertex[1]->s_pos->y);
	tu1 = (int)(face->vertex[1]->uv->u);
	tv1 = (int)(face->vertex[1]->uv->v);
	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);

	x2 = (int)(face->vertex[2]->s_pos->x);
	y2 = (int)(face->vertex[2]->s_pos->y);
	tu2 = (int)(face->vertex[2]->uv->u);
	tv2 = (int)(face->vertex[2]->uv->v);
	tz2 = (1 << FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);

	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{

		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu1 << FIXP16_SHIFT);
				vr = (tv1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				// set starting y
				ystart = y0;

			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu0 << FIXP16_SHIFT);
				vr = (tv0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				// set starting y
				ystart = y0;

			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl; // + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[(ui >> (FIXP16_SHIFT)) + ((vi >> (FIXP16_SHIFT)) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl; // + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[(ui >> (FIXP16_SHIFT)) + ((vi >> (FIXP16_SHIFT)) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// no clipping y

			// set starting values
			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else


		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[(ui >> (FIXP16_SHIFT)) + ((vi >> (FIXP16_SHIFT)) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else


				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,z interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl; // + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[(ui >> (FIXP16_SHIFT)) + ((vi >> (FIXP16_SHIFT)) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Textured_TriangleFSINVZB2_16

void Draw_Textured_Triangle_GSINVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
{
	void * tmp_ptr;

	int temp=0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		du,dv,dw,dz, ds, dt,
		xi,yi, // the current interpolated x,y
		ui,vi,wi,si,ti, // the current interpolated u,v
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
		dsdyl,
		sl,
		dtdyl,
		tl,
		dudyr,
		ur,
		dvdyr,
		vr,
		dwdyr,
		wr,
		dzdyr,
		zr,
		dsdyr,
		sr,
		dtdyr,
		tr;

	DWORD zi;

	int x0,y0,tu0,tv0,tw0, tz0, ts0,tt0, // cached vertices
		x1,y1,tu1,tv1,tw1, tz1, ts1,tt1,
		x2,y2,tu2,tv2,tw2, tz2, ts2,tt2;

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

	// extract texture map
	textmap = (DWORD *)face->texture->pRGBABuffer;

	// extract base 2 of texture width
	texture_shift2 = logbase2ofx[face->texture->width];

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base0 = face->vertex[0]->color->red;
	g_base0 = face->vertex[0]->color->green;
	b_base0 = face->vertex[0]->color->blue;

	r_base1 = face->vertex[1]->color->red;
	g_base1 = face->vertex[1]->color->green;
	b_base1 = face->vertex[1]->color->blue;

	r_base2 = face->vertex[2]->color->red;
	g_base2 = face->vertex[2]->color->green;
	b_base2 = face->vertex[2]->color->blue;

	// extract vertices for processing, now that we have order
	x0 = (int)(face->vertex[0]->s_pos->x);
	y0 = (int)(face->vertex[0]->s_pos->y);

	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	ts0 = (int)(face->vertex[0]->uv->u);
	tt0 = (int)(face->vertex[0]->uv->v);

	tu0 = r_base0;
	tv0 = g_base0;
	tw0 = b_base0;

	x1 = (int)(face->vertex[1]->s_pos->x);
	y1 = (int)(face->vertex[1]->s_pos->y);

	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	ts1 = (int)(face->vertex[1]->uv->u);
	tt1 = (int)(face->vertex[1]->uv->v);

	tu1 = r_base1;
	tv1 = g_base1;
	tw1 = b_base1;

	x2 = (int)(face->vertex[2]->s_pos->x);
	y2 = (int)(face->vertex[2]->s_pos->y);

	tz2 = (1 << FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	ts2 = (int)(face->vertex[2]->uv->u);
	tt2 = (int)(face->vertex[2]->uv->v);

	tu2 = r_base2;
	tv2 = g_base2;
	tw2 = b_base2;

	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{

		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dwdyl = ((tw2 - tw0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dsdyl = ((ts2 - ts0) << FIXP16_SHIFT)/dy;
			dtdyl = ((tt2 - tt0) << FIXP16_SHIFT)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
			dwdyr = ((tw2 - tw1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			dsdyr = ((ts2 - ts1) << FIXP16_SHIFT)/dy;
			dtdyr = ((tt2 - tt1) << FIXP16_SHIFT)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
				tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				wr = dwdyr*dy + (tw1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				sr = dsdyr*dy + (ts1 << FIXP16_SHIFT);
				tr = dtdyr*dy + (tt1 << FIXP16_SHIFT);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				wl = (tw0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				sl = (ts0 << FIXP16_SHIFT);
				tl = (tt0 << FIXP16_SHIFT);


				ur = (tu1 << FIXP16_SHIFT);
				vr = (tv1 << FIXP16_SHIFT);
				wr = (tw1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				sr = (ts1 << FIXP16_SHIFT);
				tr = (tt1 << FIXP16_SHIFT);

				// set starting y
				ystart = y0;

			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dy;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dy;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dy;


			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
				tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
				tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);


				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				wl = (tw0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				sl = (ts0 << FIXP16_SHIFT);
				tl = (tt0 << FIXP16_SHIFT);

				ur = (tu0 << FIXP16_SHIFT);
				vr = (tv0 << FIXP16_SHIFT);
				wr = (tw0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				sr = (ts0 << FIXP16_SHIFT);
				tr = (tt0 << FIXP16_SHIFT);

				// set starting y
				ystart = y0;

			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					si+=dx*ds;
					ti+=dx*dt;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// write textel assume 5.6.5
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// get textel first
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with gouraud shading
						r_textel*=ui;
						g_textel*=vi;
						b_textel*=wi;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						// update z-buffer
						z_ptr[xi] = zi;

					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;

					si+=ds;
					ti+=dt;

				} // end for xi

				// interpolate u,v,w,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// write textel assume 5.6.5
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// get textel first
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with gouraud shading
						r_textel*=ui;
						g_textel*=vi;
						b_textel*=wi;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						// update z-buffer
						z_ptr[xi] = zi;

					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;

					si+=ds;
					ti+=dt;

				} // end for xi

				// interpolate u,v,w,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);

			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			sl = dsdyl*dyl + (ts1 << FIXP16_SHIFT);
			tl = dtdyl*dyl + (tt1 << FIXP16_SHIFT);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);

			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			sr = dsdyr*dyr + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dyr + (tt0 << FIXP16_SHIFT);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dwdyl,dwdyr,temp);
				SWAP(dzdyl,dzdyr,temp);

				SWAP(dsdyl,dsdyr,temp);
				SWAP(dtdyl,dtdyr,temp);

				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(wl,wr,temp);
				SWAP(zl,zr,temp);

				SWAP(sl,sr,temp);
				SWAP(tl,tr,temp);

				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tw1,tw2,temp);
				SWAP(tz1,tz2,temp);

				SWAP(ts1,ts2,temp);
				SWAP(tt1,tt2,temp);

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
			tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dwdyl,dwdyr,temp);
				SWAP(dzdyl,dzdyr,temp);

				SWAP(dsdyl,dsdyr,temp);
				SWAP(dtdyl,dtdyr,temp);

				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(wl,wr,temp);
				SWAP(zl,zr,temp);

				SWAP(sl,sr,temp);
				SWAP(tl,tr,temp);

				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tw1,tw2,temp);
				SWAP(tz1,tz2,temp);

				SWAP(ts1,ts2,temp);
				SWAP(tt1,tt2,temp);

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			// no clipping y

			// set starting values
			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			sl = (ts0 << FIXP16_SHIFT);
			tl = (tt0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			sr = (ts0 << FIXP16_SHIFT);
			tr = (tt0 << FIXP16_SHIFT);

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dudyl,dudyr,temp);
				SWAP(dvdyl,dvdyr,temp);
				SWAP(dwdyl,dwdyr,temp);
				SWAP(dzdyl,dzdyr,temp);

				SWAP(dsdyl,dsdyr,temp);
				SWAP(dtdyl,dtdyr,temp);


				SWAP(xl,xr,temp);
				SWAP(ul,ur,temp);
				SWAP(vl,vr,temp);
				SWAP(wl,wr,temp);
				SWAP(zl,zr,temp);

				SWAP(sl,sr,temp);
				SWAP(tl,tr,temp);


				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tu1,tu2,temp);
				SWAP(tv1,tv2,temp);
				SWAP(tw1,tw2,temp);
				SWAP(tz1,tz2,temp);

				SWAP(ts1,ts2,temp);
				SWAP(tt1,tt2,temp);

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					si+=dx*ds;
					ti+=dx*dt;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// write textel assume 5.6.5
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// get textel first
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with gouraud shading
						r_textel*=ui;
						g_textel*=vi;
						b_textel*=wi;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						// update z-buffer
						z_ptr[xi] = zi;

					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;

					si+=ds;
					ti+=dt;

				} // end for xi

				// interpolate u,v,w,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;

						sl+=dsdyl;
						tl+=dtdyl;

					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;

						sr+=dsdyr;
						tr+=dtdyr;
					} // end else

				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;

					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;

				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);

					ds = (sr - sl);
					dt = (tr - tl);

				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// write textel assume 5.6.5
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// get textel first
						textel = textmap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with gouraud shading
						r_textel*=ui;
						g_textel*=vi;
						b_textel*=wi;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );

						// update z-buffer
						z_ptr[xi] = zi;

					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;

					si+=ds;
					ti+=dt;
				} // end for xi

				// interpolate u,v,w,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				sr+=dsdyr;
				tr+=dtdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;

						sl+=dsdyl;
						tl+=dtdyl;

					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;

						sr+=dsdyr;
						tr+=dtdyr;

					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Textured_TriangleGSINVZB_16

void Draw_Flat_Triangle_INVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )// bytes per line of zbuffer
{
	void * tmp_ptr;

	int temp = 0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		dz,
		xi,yi, // the current interpolated x,y
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

	int x0,y0,tz0, // cached vertices
		x1,y1,tz1,
		x2,y2,tz2;

	DWORD *screen_ptr = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	DWORD color; // polygon color

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	// extract vertices for processing, now that we have order
	x0 = (int)(face->vertex[0]->s_pos->x);
	y0 = (int)(face->vertex[0]->s_pos->y);

	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);

	x1 = (int)(face->vertex[1]->s_pos->x);
	y1 = (int)(face->vertex[1]->s_pos->y);

	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);

	x2 = (int)(face->vertex[2]->s_pos->x);
	y2 = (int)(face->vertex[2]->s_pos->y);

	tz2 = (1 << FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);

	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// extract constant color
	color =	(face->vertex[0]->color->alpha << FIXP24_SHIFT) +
			(face->vertex[0]->color->red << FIXP16_SHIFT) +
			(face->vertex[0]->color->green << FIXP8_SHIFT) +
			(face->vertex[0]->color->blue);

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{

		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				zl = (tz0 << 0);
				zr = (tz1 << 0);

				// set starting y
				ystart = y0;

			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				zl = (tz0 << 0);
				zr = (tz0 << 0);

				// set starting y
				ystart = y0;

			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					zi+=dx*dz;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel assume 5.6.5
						screen_ptr[xi] = color;

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,w,z
					zi+=dz;
				} // end for xi

				// interpolate z,x along right and left edge
				xl+=dxdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance z-buffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel 5.6.5
						screen_ptr[xi] = color;

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate z
					zi+=dz;
				} // end for xi

				// interpolate x,z along right and left edge
				xl+=dxdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance z-buffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tz1,tz2,temp);

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tz1,tz2,temp);

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// no clipping y

			// set starting values
			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			zl = (tz0 << 0);
			zr = (tz0 << 0);

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,temp);
				SWAP(dzdyl,dzdyr,temp);
				SWAP(xl,xr,temp);
				SWAP(zl,zr,temp);
				SWAP(x1,x2,temp);
				SWAP(y1,y2,temp);
				SWAP(tz1,tz2,temp);

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for z interpolants
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute z interpolants
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					zi+=dx*dz;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel assume 5.6.5
						screen_ptr[xi] = color;

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate z
					zi+=dz;
				} // end for xi

				// interpolate z,x along right and left edge
				xl+=dxdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance z-buffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w,z interpolants
				zi = zl; // + FIXP16_ROUND_UP;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel assume 5.6.5
						screen_ptr[xi] = color;

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate z
					zi+=dz;
				} // end for xi

				// interpolate x,z along right and left edge
				xl+=dxdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance z-buffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						zr+=dzdyr;
					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Triangle_2DINVZB_16

void Draw_Gouraud_Triangle_INVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )// bytes per line of zbuffer
{
	void * tmp_ptr;
	int temp=0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		du,dv,dw,dz,
		xi,yi, // the current interpolated x,y
		ui,vi,wi, // the current interpolated u,v,w,z
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

	int x0,y0,tu0,tv0,tw0,tz0, // cached vertices
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

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	a_base = face->vertex[0]->color->alpha;

	r_base0 = face->vertex[0]->color->red;
	g_base0 = face->vertex[0]->color->green;
	b_base0 = face->vertex[0]->color->blue;

	r_base1 = face->vertex[1]->color->red;
	g_base1 = face->vertex[1]->color->green;
	b_base1 = face->vertex[1]->color->blue;

	r_base2 = face->vertex[2]->color->red;
	g_base2 = face->vertex[2]->color->green;
	b_base2 = face->vertex[2]->color->blue;

	// extract vertices for processing, now that we have order
	x0 = (int)(face->vertex[0]->s_pos->x);
	y0 = (int)(face->vertex[0]->s_pos->y);

	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	tu0 = r_base0;
	tv0 = g_base0;
	tw0 = b_base0;

	x1 = (int)(face->vertex[1]->s_pos->x);
	y1 = (int)(face->vertex[1]->s_pos->y);

	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	tu1 = r_base1;
	tv1 = g_base1;
	tw1 = b_base1;

	x2 = (int)(face->vertex[2]->s_pos->x);
	y2 = (int)(face->vertex[2]->s_pos->y);

	tz2 = (1 <<FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	tu2 = r_base2;
	tv2 = g_base2;
	tw2 = b_base2;

	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
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

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				wr = dwdyr*dy + (tw1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
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

				// set starting y
				ystart = y0;

			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
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

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				// compute new starting y
				ystart = min_clip_y;

			} // end if
			else
			{
				// no clipping

				// set starting values
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

				// set starting y
				ystart = y0;

			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel assume 5.6.5
						screen_ptr[xi] = (a_base << FIXP24_SHIFT) + ((ui >> FIXP16_SHIFT) << 16) + ((vi >> FIXP16_SHIFT) << 8) + (wi >> FIXP16_SHIFT);

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,w,z
					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} // end for xi

				// interpolate u,v,w,z,x along right and left edge
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

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance z-buffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel 5.6.5
						screen_ptr[xi] = (a_base << FIXP24_SHIFT) + ((ui >> FIXP16_SHIFT) << 16) + ((vi >> FIXP16_SHIFT) << 8) + (wi >> FIXP16_SHIFT);

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if


					// interpolate u,v,w,z
					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} // end for xi

				// interpolate u,v,w,x along right and left edge
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

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance z-buffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);

			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);

			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// no clipping y

			// set starting values
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

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel assume 5.6.5
						screen_ptr[xi] = (a_base << FIXP24_SHIFT) + ((ui >> FIXP16_SHIFT) << 16) + ((vi >> FIXP16_SHIFT) << 8) + (wi >> FIXP16_SHIFT);

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,w,z
					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} // end for xi

				// interpolate u,v,w,z,x along right and left edge
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

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance z-buffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v,w,z interpolants
				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;// + FIXP16_ROUND_UP; // ???

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} // end if
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel assume 5.6.5
						screen_ptr[xi] = (a_base << FIXP24_SHIFT) + ((ui >> FIXP16_SHIFT) << 16) + ((vi >> FIXP16_SHIFT) << 8) + (wi >> FIXP16_SHIFT);

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,w,z
					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} // end for xi

				// interpolate u,v,w,x,z along right and left edge
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

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance z-buffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Gouraud_TriangleINVZB_16



///////////////////////////////////支持透视矫正////////////////////////////////////
void Draw_Textured_Perspective_Triangle_INVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
{
	void * tmp_ptr;

	int temp=0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		du,dv,dz,
		xi,yi, // the current interpolated x,y
		ui,vi, // the current interpolated u,v,z
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

	int x0,y0,tu0,tv0,tz0, // cached vertices
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;

	// extract texture map
	textmap = (DWORD *)face->texture->pRGBABuffer;

	// extract base 2 of texture width
	texture_shift2 = logbase2ofx[face->texture->width];

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	// extract vertices for processing, now that we have order
	x0 = (int)(face->vertex[0]->s_pos->x);
	y0 = (int)(face->vertex[0]->s_pos->y);
	tu0 = ((int)(face->vertex[0]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	tv0 = ((int)(face->vertex[0]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);

	x1 = (int)(face->vertex[1]->s_pos->x);
	y1 = (int)(face->vertex[1]->s_pos->y);
	tu1 = ((int)(face->vertex[1]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	tv1 = ((int)(face->vertex[1]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);

	x2 = (int)(face->vertex[2]->s_pos->x);
	y2 = (int)(face->vertex[2]->s_pos->y);
	tu2 = ((int)(face->vertex[2]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	tv2 = ((int)(face->vertex[2]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	tz2 = (1 << FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);


	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);

				// compute new starting y
				ystart = min_clip_y;
			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);

				// set starting y
				ystart = y0;
			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);

				// compute new starting y
				ystart = min_clip_y;
			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);

				// set starting y
				ystart = y0;
			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul + 0;
				vi = vl + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];
						//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;
			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul + 0;
				vi = vl + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				} // end else


				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];
						//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// no clipping y

			// set starting values
			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul + 0;
				vi = vl + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				} // end else


				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];
						//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul + 0;
				vi = vl + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];
						//screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Textured_Perspective_Triangle_INVZB_16

void Draw_Textured_PerspectiveLP_Triangle_INVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
{
	void * tmp_ptr;

	int temp=0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		du,dv,dz,
		xi,yi, // the current interpolated x,y
		ui,vi, // the current interpolated u,v,z
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

	int x0,y0,tu0,tv0,tz0, // cached vertices
		x1,y1,tu1,tv1,tz1,
		x2,y2,tu2,tv2,tz2;

	int texture_shift2;

	DWORD *screen_ptr = NULL,
		*textmap = NULL,
		*dest_buffer = (DWORD *)_dest_buffer;

	DWORD *z_ptr = NULL,
		*zbuffer = (DWORD *)_zbuffer;
	// extract texture map
	textmap = (DWORD *)face->texture->pRGBABuffer;

	// extract base 2 of texture width
	texture_shift2 = logbase2ofx[face->texture->width];

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	// extract vertices for processing, now that we have order
	x0 = (int)(face->vertex[0]->s_pos->x);
	y0 = (int)(face->vertex[0]->s_pos->y);
	tu0 = ((int)(face->vertex[0]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	tv0 = ((int)(face->vertex[0]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);

	x1 = (int)(face->vertex[1]->s_pos->x);
	y1 = (int)(face->vertex[1]->s_pos->y);
	tu1 = ((int)(face->vertex[1]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	tv1 = ((int)(face->vertex[1]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);

	x2 = (int)(face->vertex[2]->s_pos->x);
	y2 = (int)(face->vertex[2]->s_pos->y);
	tu2 = ((int)(face->vertex[2]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	tv2 = ((int)(face->vertex[2]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	tz2 = (1 << FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);


	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);

				// compute new starting y
				ystart = min_clip_y;
			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);

				// set starting y
				ystart = y0;
			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);

				// compute new starting y
				ystart = min_clip_y;
			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);

				// set starting y
				ystart = y0;
			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute linear version of ul, ur, vl, vr
				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul2 + 0;
				vi = vl2 + 0;

				dx = (xend - xstart);

				// compute u,v interpolants
				if ( dx > 0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[(ui >> FIXP22_SHIFT) + ((vi >> FIXP22_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;
			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute linear version of ul, ur, vl, vr
				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul2 + 0;
				vi = vl2 + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				} // end else


				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[(ui >> FIXP22_SHIFT) + ((vi >> FIXP22_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// no clipping y

			// set starting values
			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute linear version of ul, ur, vl, vr
				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;


				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul2 + 0;
				vi = vl2 + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				} // end else


				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[(ui >> FIXP22_SHIFT) + ((vi >> FIXP22_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul2 + 0;
				vi = vl2 + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						screen_ptr[xi] = textmap[(ui >> FIXP22_SHIFT) + ((vi >> FIXP22_SHIFT) << texture_shift2)];

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Textured_PerspectiveLP_Triangle_INVZB_16

void Draw_Textured_Perspective_Triangle_FSINVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
{
	void * tmp_ptr;

	int temp=0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		du,dv,dz,
		xi,yi, // the current interpolated x,y
		ui,vi, // the current interpolated u,v,z
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

	int x0,y0,tu0,tv0,tz0, // cached vertices
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

	// extract texture map
	textmap = (DWORD *)face->texture->pRGBABuffer;

	// extract base 2 of texture width
	texture_shift2 = logbase2ofx[face->texture->width];

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base = face->vertex[0]->color->red;
	g_base = face->vertex[0]->color->green;
	b_base = face->vertex[0]->color->blue;

	// extract vertices for processing, now that we have order
	x0 = (int)(face->vertex[0]->s_pos->x);
	y0 = (int)(face->vertex[0]->s_pos->y);
	tu0 = ((int)(face->vertex[0]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	tv0 = ((int)(face->vertex[0]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);

	x1 = (int)(face->vertex[1]->s_pos->x);
	y1 = (int)(face->vertex[1]->s_pos->y);
	tu1 = ((int)(face->vertex[1]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	tv1 = ((int)(face->vertex[1]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);

	x2 = (int)(face->vertex[2]->s_pos->x);
	y2 = (int)(face->vertex[2]->s_pos->y);
	tu2 = ((int)(face->vertex[2]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	tv2 = ((int)(face->vertex[2]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	tz2 = (1 << FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);


	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);

				// compute new starting y
				ystart = min_clip_y;
			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);

				// set starting y
				ystart = y0;
			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);

				// compute new starting y
				ystart = min_clip_y;
			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);

				// set starting y
				ystart = y0;
			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul + 0;
				vi = vl + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						//screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> (FIXP16_SHIFT + 8), g_textel >> (FIXP16_SHIFT + 8), b_textel >> (FIXP16_SHIFT + 8) );
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if


					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;
			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul + 0;
				vi = vl + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				} // end else


				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// no clipping y

			// set starting values
			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul + 0;
				vi = vl + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				} // end else


				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul + 0;
				vi = vl + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul) / dx;
					dv = (vr - vl) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur - ul) ;
					dv = (vr - vl) ;
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[ ((ui << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) + ( ((vi << (FIXP28_SHIFT - FIXP22_SHIFT)) / zi) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >>  8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Textured_Perspective_Triangle_FSINVZB_16


void Draw_Textured_PerspectiveLP_Triangle_FSINVZB_32( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
{
	void * tmp_ptr;

	int temp=0,
		tri_type = TRI_TYPE_NONE,
		irestart = INTERP_LHS;

	int dx,dy,dyl,dyr, // general deltas
		du,dv,dz,
		xi,yi, // the current interpolated x,y
		ui,vi, // the current interpolated u,v,z
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

	int x0,y0,tu0,tv0,tz0, // cached vertices
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

	// extract texture map
	textmap = (DWORD *)face->texture->pRGBABuffer;

	// extract base 2 of texture width
	texture_shift2 = logbase2ofx[face->texture->width];

	// adjust memory pitch to words, divide by 2
	mem_pitch >>= 2;

	// adjust zbuffer pitch for 32 bit alignment
	zpitch >>= 2;

	// apply fill convention to coordinates
	face->vertex[0]->s_pos->x = (float)((int)(face->vertex[0]->s_pos->x + 0.5));
	face->vertex[0]->s_pos->y = (float)((int)(face->vertex[0]->s_pos->y + 0.5));

	face->vertex[1]->s_pos->x = (float)((int)(face->vertex[1]->s_pos->x + 0.5));
	face->vertex[1]->s_pos->y = (float)((int)(face->vertex[1]->s_pos->y + 0.5));

	face->vertex[2]->s_pos->x = (float)((int)(face->vertex[2]->s_pos->x + 0.5));
	face->vertex[2]->s_pos->y = (float)((int)(face->vertex[2]->s_pos->y + 0.5));

	// first trivial clipping rejection tests
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

	// sort vertices
	if ( face->vertex[1]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[0]->s_pos->y )
	{
		SWAP( face->vertex[0], face->vertex[2], tmp_ptr );
	}

	if ( face->vertex[2]->s_pos->y < face->vertex[1]->s_pos->y )
	{
		SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
	}

	if (FCMP(face->vertex[0]->s_pos->y, face->vertex[1]->s_pos->y) )
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		if (face->vertex[1]->s_pos->x < face->vertex[0]->s_pos->x)
		{
			SWAP( face->vertex[0], face->vertex[1], tmp_ptr );
		}
	}
	else if (FCMP(face->vertex[1]->s_pos->y ,face->vertex[2]->s_pos->y))
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		if (face->vertex[2]->s_pos->x < face->vertex[1]->s_pos->x)
		{
			SWAP( face->vertex[1], face->vertex[2], tmp_ptr );
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	r_base = face->vertex[0]->color->red;
	g_base = face->vertex[0]->color->green;
	b_base = face->vertex[0]->color->blue;

	// extract vertices for processing, now that we have order
	x0  = (int)(face->vertex[0]->s_pos->x);
	y0  = (int)(face->vertex[0]->s_pos->y);
	tu0 = ((int)(face->vertex[0]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	tv0 = ((int)(face->vertex[0]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);
	tz0 = (1 << FIXP28_SHIFT) / (int)(face->vertex[0]->s_pos->w + 0.5);

	x1  = (int)(face->vertex[1]->s_pos->x);
	y1  = (int)(face->vertex[1]->s_pos->y);
	tu1 = ((int)(face->vertex[1]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	tv1 = ((int)(face->vertex[1]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);
	tz1 = (1 << FIXP28_SHIFT) / (int)(face->vertex[1]->s_pos->w + 0.5);

	x2  = (int)(face->vertex[2]->s_pos->x);
	y2  = (int)(face->vertex[2]->s_pos->y);
	tu2 = ((int)(face->vertex[2]->uv->u + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	tv2 = ((int)(face->vertex[2]->uv->v + 0.5) << FIXP22_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);
	tz2 = (1 << FIXP28_SHIFT) / (int)(face->vertex[2]->s_pos->w + 0.5);

	// degenerate triangle
	if ( ((x0 == x1) && (x1 == x2)) || ((y0 == y1) && (y1 == y2)))
		return;

	// set interpolation restart value
	yrestart = y1;

	// what kind of triangle
	if (tri_type & TRI_TYPE_FLAT_MASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			// compute all deltas
			dy = (y2 - y0);

			dxdyl = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << 0)/dy;
			dvdyl = ((tv2 - tv0) << 0)/dy;
			dzdyl = ((tz2 - tz0) << 0)/dy;

			dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << 0)/dy;
			dvdyr = ((tv2 - tv1) << 0)/dy;
			dzdyr = ((tz2 - tz1) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x1 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << 0);
				vr = dvdyr*dy + (tv1 << 0);
				zr = dzdyr*dy + (tz1 << 0);

				// compute new starting y
				ystart = min_clip_y;
			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu1 << 0);
				vr = (tv1 << 0);
				zr = (tz1 << 0);

				// set starting y
				ystart = y0;
			} // end else

		} // end if flat top
		else
		{
			// must be flat bottom

			// compute all deltas
			dy = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << 0)/dy;
			dvdyl = ((tv1 - tv0) << 0)/dy;
			dzdyl = ((tz1 - tz0) << 0)/dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << 0)/dy;
			dvdyr = ((tv2 - tv0) << 0)/dy;
			dzdyr = ((tz2 - tz0) << 0)/dy;

			// test for y clipping
			if (y0 < min_clip_y)
			{
				// compute overclip
				dy = (min_clip_y - y0);

				// computer new LHS starting values
				xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << 0);
				vl = dvdyl*dy + (tv0 << 0);
				zl = dzdyl*dy + (tz0 << 0);

				// compute new RHS starting values
				xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << 0);
				vr = dvdyr*dy + (tv0 << 0);
				zr = dzdyr*dy + (tz0 << 0);

				// compute new starting y
				ystart = min_clip_y;
			} // end if
			else
			{
				// no clipping

				// set starting values
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << 0);
				vl = (tv0 << 0);
				zl = (tz0 << 0);

				ur = (tu0 << 0);
				vr = (tv0 << 0);
				zr = (tz0 << 0);

				// set starting y
				ystart = y0;
			} // end else

		} // end else flat bottom

		// test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;


				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul2 + 0;
				vi = vl2 + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				} // end else

				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// reset vars
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[ (ui >> FIXP22_SHIFT) + ( (vi >> FIXP22_SHIFT) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >> 8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if


					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;
			} // end for y

		} // end if clip
		else
		{
			// non-clip version

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul2 + 0;
				vi = vl2 + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				} // end else


				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[ (ui >> FIXP22_SHIFT) + ( (vi >> FIXP22_SHIFT) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >> 8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

			} // end for y

		} // end if non-clipped

	} // end if
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		// first test for bottom clip, always
		if ((yend = y2) > max_clip_y)
			yend = max_clip_y;

		// pre-test y clipping status
		if (y1 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;
			dvdyl = ((tv2 - tv1) << 0)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dyr = (min_clip_y - y0);
			dyl = (min_clip_y - y1);

			// computer new LHS starting values
			xl = dxdyl*dyl + (x1 << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			// compute new RHS starting values
			xr = dxdyr*dyr + (x0 << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else if (y0 < min_clip_y)
		{
			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// compute overclip
			dy = (min_clip_y - y0);

			// computer new LHS starting values
			xl = dxdyl*dy + (x0 << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			// compute new RHS starting values
			xr = dxdyr*dy + (x0 << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			// compute new starting y
			ystart = min_clip_y;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end if
		else
		{
			// no initial y clipping

			// compute all deltas
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1 - x0) << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;
			dvdyl = ((tv1 - tv0) << 0)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			// RHS
			dyr = (y2 - y0);

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;
			dvdyr = ((tv2 - tv0) << 0)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			// no clipping y

			// set starting values
			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			// set starting y
			ystart = y0;

			// test if we need swap to keep rendering left to right
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

				// set interpolation restart
				irestart = INTERP_RHS;

			} // end if

		} // end else

		// test for horizontal clipping
		if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
			(x1 < min_clip_x) || (x1 > max_clip_x) ||
			(x2 < min_clip_x) || (x2 > max_clip_x))
		{
			// clip version
			// x clipping

			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul2 + 0;
				vi = vl2 + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				} // end else


				///////////////////////////////////////////////////////////////////////

				// test for x clipping, LHS
				if (xstart < min_clip_x)
				{
					// compute x overlap
					dx = min_clip_x - xstart;

					// slide interpolants over
					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					// set x to left clip edge
					xstart = min_clip_x;

				} // end if

				// test for x clipping RHS
				if (xend > max_clip_x)
					xend = max_clip_x;

				///////////////////////////////////////////////////////////////////////

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[ (ui >> FIXP22_SHIFT) + ( (vi >> FIXP22_SHIFT) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >> 8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v,z
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end if
		else
		{
			// no x clipping
			// point screen ptr to starting line
			screen_ptr = dest_buffer + (ystart * mem_pitch);

			// point zbuffer to starting line
			z_ptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				// compute span endpoints
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2 = ((ul << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				ur2 = ((ur << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				vl2 = ((vl << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zl >> 6) ) << 16;
				vr2 = ((vr << (FIXP28_SHIFT - FIXP22_SHIFT)) / (zr >> 6) ) << 16;

				// compute starting points for u,v interpolants
				zi = zl + 0; // ????
				ui = ul2 + 0;
				vi = vl2 + 0;

				// compute u,v interpolants
				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2) / dx;
					dv = (vr2 - vl2) / dx;
					dz = (zr - zl) / dx;
				} // end if
				else
				{
					du = (ur2 - ul2) ;
					dv = (vr2 - vl2) ;
					dz = (zr - zl);
				} // end else

				// draw span
				for (xi=xstart; xi < xend; xi++)
				{
					// test if z of current pixel is nearer than current z buffer value
					if (zi > z_ptr[xi])
					{
						// write textel
						// get textel first
						textel = textmap[ (ui >> FIXP22_SHIFT) + ( (vi >> FIXP22_SHIFT) << texture_shift2)];

						// extract rgb components
						r_textel = ((textel >> 16) & 0xff );
						g_textel = ((textel >> 8) & 0xff);
						b_textel = (textel & 0xff);

						// modulate textel with lit background color
						r_textel*=r_base;
						g_textel*=g_base;
						b_textel*=b_base;

						// finally write pixel, note that we did the math such that the results are r*32, g*64, b*32
						// hence we need to divide the results by 32,64,32 respetively, BUT since we need to shift
						// the results to fit into the destination 5.6.5 word, we can take advantage of the shifts
						// and they all cancel out for the most part, but we will need logical anding, we will do
						// it later when we optimize more...
						screen_ptr[xi] = RGB32BIT( (textel >> FIXP24_SHIFT) & 0xff, r_textel >> 8, g_textel >> 8, b_textel >> 8 );

						// update z-buffer
						z_ptr[xi] = zi;
					} // end if

					// interpolate u,v
					ui+=du;
					vi+=dv;
					zi+=dz;
				} // end for xi

				// interpolate u,v,x along right and left edge
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				// advance screen ptr
				screen_ptr+=mem_pitch;

				// advance zbuffer ptr
				z_ptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					// test interpolation side change flag

					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						// set starting values
						xl = (x1 << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						// interpolate down on LHS to even up
						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} // end if
					else
					{
						// RHS
						dyr = (y1 - y2);

						dxdyr = ((x1 - x2) << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;
						dvdyr = ((tv1 - tv2) << 0)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						// set starting values
						xr = (x2 << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						// interpolate down on RHS to even up
						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;

					} // end else

				} // end if

			} // end for y

		} // end else

	} // end if

} // end Draw_Textured_PerspectiveLP_Triangle_FSINVZB_16

# endif