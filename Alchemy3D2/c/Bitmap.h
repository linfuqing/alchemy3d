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

#ifdef RGB565

		memset( bitmap->pRGBABuffer, 0, sizeof(USHORT) * bitmap->wh );
#else

		memset( bitmap->pRGBABuffer, 0, sizeof(DWORD) * bitmap->wh );

#endif
		
		free( bitmap->pRGBABuffer );

		memset( bitmap, 0, sizeof( Bitmap ) );

		free( bitmap );

		return 1;
	}
	else
	{
		return 0;
	}
}

void bitmap_destroy( Bitmap **bitmap )
{
	if ( (*bitmap) && (*bitmap)->pRGBABuffer )
	{
		free( (*bitmap)->pRGBABuffer );

		free( *bitmap );

		*bitmap = NULL;
	}
}

//对图像执行双线性滤波
void mipmap_bilerpBlends( Bitmap * src, Bitmap * dest, float gamma )
{
	int x, y;

#ifdef RGB565

	USHORT * src_buffer = (USHORT *)src->pRGBABuffer;
	USHORT * dest_buffer = (USHORT *)dest->pRGBABuffer;

#else

	DWORD * src_buffer = (DWORD *)src->pRGBABuffer;
	DWORD * dest_buffer = (DWORD *)dest->pRGBABuffer;
	
	USHORT a0, a1, a2, a3, a_avg;

#endif

	USHORT r0, g0, b0,
		r1, g1, b1,
		r2, g2, b2,
		r3, g3, b3;

	USHORT r_avg, g_avg, b_avg;

	DWORD d;

	int u0, u1, v0, v1,

		mip_width = dest->width, mip_height = dest->height;

	float t0 = (float)src->width / (float)dest->width, t1 = (float)src->height / (float)dest->height;

	int pitch = (int)( dest->width * t0 );

	gamma = gamma == 0.0f ? 1.01f : gamma;

	//开始迭代计算像素平均值
	for ( x = 0; x < mip_width; x++)
	{
		for ( y = 0; y < mip_height; y++)
		{
			d = x + y * mip_width;

			u0 = (int)(x * t0); u1 = u0 == src->width - 1 ? u0 : u0 + 1;
			v0 = (int)(y * t1); v1 = v0 == src->height - 1 ? v0 : v0 + 1;

#ifdef RGB565

			//拆分argb到byte
			RGB565FROM16BIT( src_buffer[(u0) + (v0) * pitch] , &r0, &g0, &b0);
			RGB565FROM16BIT( src_buffer[(u1) + (v0) * pitch] , &r1, &g1, &b1);
			RGB565FROM16BIT( src_buffer[(u0) + (v1) * pitch] , &r2, &g2, &b2);
			RGB565FROM16BIT( src_buffer[(u1) + (v1) * pitch] , &r3, &g3, &b3);

			//计算平均值
			r_avg = (USHORT)(0.5f + gamma*(r0+r1+r2+r3)/4);
			g_avg = (USHORT)(0.5f + gamma*(g0+g1+g2+g3)/4);
			b_avg = (USHORT)(0.5f + gamma*(b0+b1+b2+b3)/4);

			//检查越界
			if (r_avg > 0x1f) r_avg = 0x1f;
			if (g_avg > 0x3f) g_avg = 0x3f;
			if (b_avg > 0x1f) b_avg = 0x1f;

			//写入数据
			dest_buffer[d] = RGB16BIT565(r_avg, g_avg, b_avg);
			dest->alpha[d] = src->alpha[d];

#else

			//拆分argb到byte
			RGB888FROM32BIT( src_buffer[(u0) + (v0) * pitch] , &a0, &r0, &g0, &b0);
			RGB888FROM32BIT( src_buffer[(u1) + (v0) * pitch] , &a1, &r1, &g1, &b1);
			RGB888FROM32BIT( src_buffer[(u0) + (v1) * pitch] , &a2, &r2, &g2, &b2);
			RGB888FROM32BIT( src_buffer[(u1) + (v1) * pitch] , &a3, &r3, &g3, &b3);

			//计算平均值
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
			dest_buffer[d] = RGB32BIT( a_avg, r_avg, g_avg, b_avg );
#endif
		}
	}
}

//void bilinear_Interpolation( Bitmap * src, Bitmap * dest )
//{
//#ifdef RGB565
//
//	USHORT * source_addr = (USHORT *)src->pRGBABuffer;
//	USHORT * destination_addr = (USHORT *)dest->pRGBABuffer;
//
//#else
//
//	DWORD * source_addr = (DWORD *)src->pRGBABuffer;
//	DWORD * destination_addr = (DWORD *)dest->pRGBABuffer;
//
//#endif
//
//	float f_u;
//	float f_v;
//	int i,j,x,y;
//	int adjacent[4];
//
//	int source_width = src->width;
//	int source_height = src->height;
//	float destination_width = (float)dest->width;
//	float destination_height = (float)dest->height;
//
//	for(j=0;j<destination_height;j++)
//	{
//		for(i=0;i<destination_width;i++)
//		{
//			f_u=i*(source_width/destination_width);
//			f_v=j*(source_height/destination_height);
//
//			x=(int)f_u;
//			f_u=f_u-x;
//
//			y=(int) f_v;
//			f_v=f_v-y;
//
//			adjacent[0]=*(source_addr+x+y*source_width);
//			adjacent[1]=*(source_addr+x+(y+1)*source_width);
//			adjacent[2]=*(source_addr+x+1+y*source_width);
//			adjacent[3]=*(source_addr+x+1+(y+1)*source_width);
//
//			*destination_addr=(1-f_u)*(1-f_v)*adjacent[0]+(1-f_u)*(f_v)*adjacent[1]+f_u*(1-f_v)*adjacent[2]+f_u*f_v*adjacent[3];
//
//			destination_addr++;
//		}
//	}
//}

//构造Midmap
int bitmap_generateMipmaps( Bitmap * source, Bitmap *** mipmaps, int num_mip_levels, float gamma )
{
	Bitmap * * tmipmaps;

	//计算需要创建多少张MIPMAP
	//int num_mip_levels = logbase2ofx[source->width] + 1;
	num_mip_levels = num_mip_levels == OFF ? (logbase2ofx[source->width] + 1) : MIN(num_mip_levels, logbase2ofx[source->width] + 1);

	int mip_level = 1;

	//宽高
	int mip_width = source->width;
	int mip_height = source->height;

	if( ( tmipmaps = ( Bitmap ** )malloc( num_mip_levels * sizeof( Bitmap * ) ) ) == NULL ) exit( TRUE );

	//第0张纹理设置为源纹理
	tmipmaps[0] = newBitmap( source->width, source->height, NULL );

	mipmap_bilerpBlends( source, tmipmaps[0], gamma );

	bitmap_dispose( source );

	//按级数开发迭代
	for ( ; mip_level < num_mip_levels; mip_level++ )
	{
		//向下一级size缩放
		mip_width = mip_width / 2;
		mip_height = mip_height / 2;

		//创建一张新的纹理
		tmipmaps[mip_level] = newBitmap( mip_width, mip_height, NULL );

		//双线性滤波
		mipmap_bilerpBlends( tmipmaps[mip_level-1], tmipmaps[mip_level], gamma );
	}

	* mipmaps = tmipmaps;

	return(num_mip_levels);
}

int bitmap_deleteMipmaps( Bitmap * * mipmaps, int num_mip_levels/*leave_level_0*/ )
{
	Bitmap * * tmipmaps = ( Bitmap * * ) * mipmaps;

	int mip_level = 0;//1;

	//int num_mip_levels = logbase2ofx[tmipmaps[0]->width] + 1;

	if ( !tmipmaps ) return( 0 );

	for ( ; mip_level < num_mip_levels; mip_level++ )
	{
		//bitmap_dispose(tmipmaps[mip_level]);

		//free(tmipmaps[mip_level]);

		bitmap_destroy(&tmipmaps[mip_level]);
	}

	/*if ( leave_level_0 == 1 )
	{
		Bitmap * temp = tmipmaps[0];

		*tmipmaps = temp;
	}
	else
	{
		bitmap_dispose(tmipmaps[0]);

		free(tmipmaps[0]);
	}*/

	return(1);
}

#endif