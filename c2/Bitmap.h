#ifndef __BITMAP_H
#define __BITMAP_H

#include <malloc.h>

typedef struct Bitmap
{
	int width;
	int height;
	int wh;
	
	BYTE * pRGBABuffer;

#ifdef RGB565
	BYTE * alpha;
#endif

}Bitmap;

Bitmap * newBitmap( int width, int height, BYTE * data )
{
	Bitmap * bitmap;

#ifdef RGB565

	DWORD * data2 = ( DWORD * )data;
	USHORT * data3;
	BYTE r, g, b;
	int i;

#endif

	if( ( bitmap = ( Bitmap * )malloc( sizeof( Bitmap ) ) ) == NULL ) exit( TRUE );

	bitmap->width = width;
	bitmap->height = height;
	bitmap->wh = width * height;

#ifdef RGB565

	if ( data )
	{
		if( ( data3 = ( USHORT * )calloc( bitmap->wh, sizeof( USHORT ) ) ) == NULL ) exit( TRUE );

		if( ( bitmap->alpha = ( BYTE * )calloc( bitmap->wh, sizeof( BYTE ) ) ) == NULL ) exit( TRUE );

		for ( i = 0; i < bitmap->wh; i ++ )
		{
			RGB888FROM32BIT( data2[i], &bitmap->alpha[i], &r, &g, &b );

			r >>= 3;
			g >>= 2;
			b >>= 3;

			data3[i] = ( USHORT )RGB16BIT565( r, g, b );
		}

		bitmap->pRGBABuffer = ( BYTE * )data3;

		memset( data, 0, sizeof(USHORT) * bitmap->wh );

		free( data );
	}
	else
	{
		if( ( bitmap->pRGBABuffer = ( BYTE * )calloc( bitmap->wh, sizeof( USHORT ) ) ) == NULL ) exit( TRUE );

		if( ( bitmap->alpha = ( BYTE * )calloc( bitmap->wh, sizeof( BYTE ) ) ) == NULL ) exit( TRUE );
	}

#else

	if ( data )
	{
		bitmap->pRGBABuffer = data;
	}
	else
	{
		if( ( bitmap->pRGBABuffer = ( BYTE * )calloc( bitmap->wh, sizeof( DWORD ) ) ) == NULL ) exit( TRUE );
	}

#endif

	return bitmap;
}

int bitmap_dispose( Bitmap * bitmap )
{
	if ( bitmap && bitmap->pRGBABuffer )
	{
		memset( bitmap, 0, sizeof( Bitmap ) );
		
		free( bitmap->pRGBABuffer );

		return 1;
	}
	else
	{
		return 0;
	}
}

//Midmap函数
int bitmap_generateMipmaps( Bitmap * source, Bitmap *** mipmaps, float gamma )
{
	Bitmap * * tmipmaps;

	//计算需要创建多少张MIPMAP
	int num_mip_levels = logbase2ofx[source->width] + 1;

	int mip_level = 1, x, y;

	//宽高
	int mip_width = source->width;
	int mip_height = source->height;
	
	USHORT r0, g0, b0,
		r1, g1, b1,
		r2, g2, b2,
		r3, g3, b3;

	USHORT r_avg, g_avg, b_avg;

	DWORD d;

#ifdef RGB565

	USHORT *src_buffer, *dest_buffer;

#else

	USHORT a0, a1, a2, a3, a_avg;
	DWORD *src_buffer, *dest_buffer;

#endif

	gamma = gamma == 0 ? 1.01f : gamma;

	if( ( tmipmaps = ( Bitmap ** )malloc( num_mip_levels * sizeof( Bitmap * ) ) ) == NULL ) exit( TRUE );

	//第0张纹理设置为源纹理
	tmipmaps[0] = source;

	//按级数开发迭代
	for ( ; mip_level < num_mip_levels; mip_level++ )
	{
		//向下一级size缩放
		mip_width = mip_width / 2;
		mip_height = mip_height / 2;

		//创建一张新的纹理
		tmipmaps[mip_level] = newBitmap( mip_width, mip_height, NULL );

		//开始迭代计算像素平均值
		for ( x = 0; x < mip_width; x++)
		{
			for ( y = 0; y < mip_height; y++)
			{
				d = x + y * mip_width;
#ifdef RGB565
				src_buffer = (USHORT *)tmipmaps[mip_level-1]->pRGBABuffer;
				dest_buffer = (USHORT *)tmipmaps[mip_level]->pRGBABuffer;

				//拆分argb到byte
				RGB565FROM16BIT( src_buffer[(x*2+0) + (y*2+0)*mip_width*2] , &r0, &g0, &b0);
				RGB565FROM16BIT( src_buffer[(x*2+1) + (y*2+0)*mip_width*2] , &r1, &g1, &b1);
				RGB565FROM16BIT( src_buffer[(x*2+0) + (y*2+1)*mip_width*2] , &r2, &g2, &b2);
				RGB565FROM16BIT( src_buffer[(x*2+1) + (y*2+1)*mip_width*2] , &r3, &g3, &b3);

				//考虑gamma值，计算平均值
				r_avg = (USHORT)(0.5f + gamma*(r0+r1+r2+r3)/4);
				g_avg = (USHORT)(0.5f + gamma*(g0+g1+g2+g3)/4);
				b_avg = (USHORT)(0.5f + gamma*(b0+b1+b2+b3)/4);

				//检查越界
				if (r_avg > 0x1f) r_avg = 0x1f;
				if (g_avg > 0x3f) g_avg = 0x3f;
				if (b_avg > 0x1f) b_avg = 0x1f;

				//写入数据
				dest_buffer[d] = RGB16BIT565(r_avg, g_avg, b_avg);
				tmipmaps[mip_level]->alpha[d] = tmipmaps[mip_level-1]->alpha[d];
#else
				src_buffer = (DWORD *)tmipmaps[mip_level-1]->pRGBABuffer;
				dest_buffer = (DWORD *)tmipmaps[mip_level]->pRGBABuffer;

				//拆分argb到byte
				RGB888FROM32BIT( src_buffer[(x*2+0) + (y*2+0)*mip_width*2] , &a0, &r0, &g0, &b0);
				RGB888FROM32BIT( src_buffer[(x*2+1) + (y*2+0)*mip_width*2] , &a1, &r1, &g1, &b1);
				RGB888FROM32BIT( src_buffer[(x*2+0) + (y*2+1)*mip_width*2] , &a2, &r2, &g2, &b2);
				RGB888FROM32BIT( src_buffer[(x*2+1) + (y*2+1)*mip_width*2] , &a3, &r3, &g3, &b3);

				//考虑gamma值，计算平均值
				a_avg = (USHORT)(0.5f + gamma*(a0+a1+a2+a3)/4);
				r_avg = (USHORT)(0.5f + gamma*(r0+r1+r2+r3)/4);
				g_avg = (USHORT)(0.5f + gamma*(g0+g1+g2+g3)/4);
				b_avg = (USHORT)(0.5f + gamma*(b0+b1+b2+b3)/4);

				//检查越界
				if (a_avg > 0xff) a_avg = 0xff;
				if (r_avg > 0xff) r_avg = 0xff;
				if (g_avg > 0xff) g_avg = 0xff;
				if (b_avg > 0xff) b_avg = 0xff;

				//写入数据
				dest_buffer[d] = RGB32BIT(a_avg, r_avg, g_avg, b_avg);
#endif
			}
		}
	}

	* mipmaps = tmipmaps;

	return(num_mip_levels);
}

int bitmap_deleteMipmaps( Bitmap * * mipmaps, int leave_level_0 )
{
	Bitmap * * tmipmaps = ( Bitmap * * ) * mipmaps;

	int mip_level = 1;

	int num_mip_levels = logbase2ofx[tmipmaps[0]->width] + 1;

	if ( !tmipmaps ) return( 0 );

	for ( ; mip_level < num_mip_levels; mip_level++ )
	{
		bitmap_dispose(tmipmaps[mip_level]);

		free(tmipmaps[mip_level]);

	}

	if ( leave_level_0 == 1 )
	{
		Bitmap * temp = tmipmaps[0];

		*tmipmaps = temp;
	}
	else
	{
		bitmap_dispose(tmipmaps[0]);

		free(tmipmaps[0]);
	}

	return(1);
}

#endif