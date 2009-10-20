#ifndef __MATH3D_H
#define __MATH3D_H

#ifdef INFINITY
#undef INFINITY
#endif

#ifdef FLT_EPSILON
#undef FLT_EPSILON
#endif

#define DEG2RAD(a)				( (a) * M_DEG2RAD )
#define RAD2DEG(a)				( (a) * M_RAD2DEG )

#define SEC2MS(t)				( FtoiFast( (t) * M_SEC2MS ) )
#define MS2SEC(t)				( (t) * M_MS2SEC )

#define	ANGLE2SHORT(x)			( FtoiFast( (x) * 65536.0f / 360.0f ) & 65535 )
#define	SHORT2ANGLE(x)			( (x) * ( 360.0f / 65536.0f ) )

#define	ANGLE2BYTE(x)			( FtoiFast( (x) * 256.0f / 360.0f ) & 255 )
#define	BYTE2ANGLE(x)			( (x) * ( 360.0f / 256.0f ) )

#define FLOATSIGNBITSET(f)		((*(const unsigned long *)&(f)) >> 31)
#define FLOATSIGNBITNOTSET(f)	((~(*(const unsigned long *)&(f))) >> 31)
#define FLOATNOTZERO(f)			((*(const unsigned long *)&(f)) & ~(1<<31) )
#define INTSIGNBITSET(i)		(((const unsigned long)(i)) >> 31)
#define INTSIGNBITNOTSET(i)		((~((const unsigned long)(i))) >> 31)

#define	FLOAT_IS_NAN(x)			(((*(const unsigned long *)&x) & 0x7f800000) == 0x7f800000)
#define FLOAT_IS_INF(x)			(((*(const unsigned long *)&x) & 0x7fffffff) == 0x7f800000)
#define FLOAT_IS_IND(x)			((*(const unsigned long *)&x) == 0xffc00000)
#define	FLOAT_IS_DENORMAL(x)	(((*(const unsigned long *)&x) & 0x7f800000) == 0x00000000 && \
								 ((*(const unsigned long *)&x) & 0x007fffff) != 0x00000000 )
#define IEEE_FLT_MANTISSA_BITS	23
#define IEEE_FLT_EXPONENT_BITS	8
#define IEEE_FLT_EXPONENT_BIAS	127
#define IEEE_FLT_SIGN_BIT		31

#define IEEE_DBL_MANTISSA_BITS	52
#define IEEE_DBL_EXPONENT_BITS	11
#define IEEE_DBL_EXPONENT_BIAS	1023
#define IEEE_DBL_SIGN_BIT		63

#define IEEE_DBLE_MANTISSA_BITS	63
#define IEEE_DBLE_EXPONENT_BITS	15
#define IEEE_DBLE_EXPONENT_BIAS	0
#define IEEE_DBLE_SIGN_BIT		79

const float PI				= 3.14159265358979323846f;
const float TWO_PI			= 6.28318530717958647692f;
const float HALF_PI			= 1.57079632679489661923f;
const float ONEFOURTH_PI	= 0.78539816339744830962f;
const float E				= 2.71828182845904523536f;
const float SQRT_TWO		= 1.41421356237309504880f;
const float SQRT_THREE		= 1.73205080756887729352f;
const float SQRT_1OVER2		= 0.70710678118654752440f;
const float SQRT_1OVER3		= 0.57735026918962576450f;
const float M_DEG2RAD		= 0.01745329251994329577f;
const float M_RAD2DEG		= 57.2957795130823208768f;
const float M_SEC2MS		= 1000.0f;
const float INFINITY		= 1e30f;
const float FLT_EPSILON		= 1.192092896e-07f;
const float INV_255			= 0.00392156862745098039f;

#define FIXP8_SHIFT     8
#define FIXP16_SHIFT     16
#define FIXP16_MAG       65536
#define FIXP16_DP_MASK   0x0000ffff
#define FIXP16_WP_MASK   0xffff0000
#define FIXP16_ROUND_UP  0x00008000
#define FIXP22_SHIFT     22
#define FIXP24_SHIFT     24
#define FIXP28_SHIFT     28

#define EPSILON_E3 (float)(1E-3) 
#define EPSILON_E4 (float)(1E-4) 
#define EPSILON_E5 (float)(1E-5)
#define EPSILON_E6 (float)(1E-6)

#define MATRIX_INVERSE_EPSILON		1e-14

BYTE logbase2ofx[513] = 
{
	0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4, 
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5, 5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5, 
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, 
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, 
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,

	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
};

#define RGB16BIT555(r, g, b) ((b & 31) + ((g & 31) << 5) + ((r & 31) << 10))
#define RGB16BIT565(r, g, b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))
#define RGB24BIT(r, g, b) ((b) + ((g) << 8) + ((r) << 16) )
#define RGB32BIT(a, r, g, b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

#define RGB555FROM16BIT(RGB, r, g, b) { *r = (((RGB) >> 10) & 0x1f); *g = (((RGB) >> 5) & 0x1f); *b = ((RGB) & 0x1f); }
#define RGB565FROM16BIT(RGB, r, g, b) { *r = (((RGB) >> 11) & 0x1f); *g = (((RGB) >> 5) & 0x3f); *b = ((RGB) & 0x1f); }
#define RGB888FROM32BIT(ARGB, a, r, g, b) { *a = ( ((ARGB) >> 24) & 0xff ); *r = ( ((ARGB) >> 16) & 0xff); *g = (((ARGB) >> 8) & 0xff); *b = ((ARGB) & 0xff); }

//#define RGB565TOARGB888(RGB, a) (((a) << 24) + ((((RGB) >> 11) & 0x1f) << (19)) + ((((RGB) >> 5) & 0x3f) << (10)) + (((RGB) & 0x1f) << 3) )
#define RGB565TOARGB888(a, r, g, b) (((a) << 24) + ((r) << 19) + ((g) << (10)) + ((b) << 3) )

#define FCMP(a,b) ( (fabs(a-b) < EPSILON_E3) ? 1 : 0)
#define ABS(x)	(((x) < 0) ? -(x) : (((x) > 0) ? (x) : 0))
#define BOUND(x,a,b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define MAX(a, b) (a > b) ? (a) : (b)
#define MIN(a, b) (a < b) ? (a) : (b)
#define SWAP(a,b,t) {t=a; a=b; b=t;}

#ifdef __NOT_AS3__

INLINE void Mem_Set_QUAD( void *dest, DWORD data, int count )
{
	__asm
	{
		mov edi, dest ; edi points to destination memory
		mov ecx, count ; number of 32-bit words to move
		mov eax, data ; 32-bit data
		rep stosd ; move data
	}
}

#endif

INLINE void swapf(float * x, float * y)
{
	float temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

INLINE void swap(int * x, int * y)
{
	int temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

INLINE float invSqrt( float x )
{
	float xhalf = 0.5f * x;
	int i = *(int*)(&x);
	i = 0x5f3759df - ( i >> 1 );
	x = *(float*)(&i);
	x = x * ( 1.5f - xhalf * x * x );

	return x;
}

#define NUM_ALPHA_LEVELS 256

#ifdef RGB565
int rgb565_Alpha_Table_Builder( int num_alpha_levels, USHORT rgb_alpha_table[NUM_ALPHA_LEVELS][65536] )
{
	int r,g,b;
	int alpha_level;
	int rgbindex;
	float alpha;
	float delta_alpha;

	if (!rgb_alpha_table)
		return(-1);

	alpha = 0;
	delta_alpha = EPSILON_E6 + 1/((float)(num_alpha_levels-1));

	// we need num_alpha_level_rows
	for ( alpha_level = 0; alpha_level < num_alpha_levels; alpha_level ++ )
	{
		// there are 65536 RGB values we need to compute, assuming that we are in RGB: 4.4.4 format
		for ( rgbindex = 0; rgbindex < 65536; rgbindex ++ )
		{
			// extract r,g,b from rgbindex, assuming an encoding of 5.6.5
			RGB565FROM16BIT(rgbindex, &r, &g, &b);

			// scale
			r = (int)( (float)r * (float)alpha );
			g = (int)( (float)g * (float)alpha );
			b = (int)( (float)b * (float)alpha );

			// build pixel back up and store
			rgb_alpha_table[alpha_level][rgbindex] = RGB16BIT565(r,g,b);

		} // end for rgbindex

		// decrease alpha level
		alpha+=delta_alpha;

	} // end for row

	// return success
	return(1);

} // end RGB_Alpha_Table_Builder
#endif

int alpha_Table_Builder( int num_alpha_levels, BYTE alpha_table[NUM_ALPHA_LEVELS][256])
{
	int t;
	int alpha_level;
	int index;
	float alpha;
	float delta_alpha;

	if ( !alpha_table )
		return(-1);

	alpha = 0;
	delta_alpha = EPSILON_E6 + 1/((float)(num_alpha_levels-1));

	for ( alpha_level = 0; alpha_level < num_alpha_levels; alpha_level ++ )
	{
		for ( index = 0; index < 256; index ++ )
		{
			t = (int)( (float)index * (float)alpha );

			alpha_table[alpha_level][index] = t;
		}

		alpha += delta_alpha;
	}

	return(1);
}

int multiply256_Table_Builder( DWORD multiply256_table[256][256] )
{
	int index, alpha_level, alpha;

	if ( !multiply256_table )
		return(-1);

	alpha = 0;

	for ( alpha_level = 0; alpha_level < 256; alpha_level ++ )
	{
		for ( index = 0; index < 256; index ++ )
		{
			multiply256_table[alpha_level][index] = alpha * index;
		}

		alpha ++;
	}

	return(1);
}

#endif