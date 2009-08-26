#ifndef __RENDER_H
#define __RENDER_H

#include "Texture.h"

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

void Draw_Textured_TriangleINVZB_16( Triangle * face, BYTE *_dest_buffer, int mem_pitch, BYTE *_zbuffer, int zpitch, int min_clip_x, int max_clip_x, int min_clip_y, int max_clip_y )
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
# endif