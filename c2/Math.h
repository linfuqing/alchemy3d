#ifndef __MATH_H
#define __MATH_H

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

enum
{
	LOOKUP_BITS				= 8,							
	EXP_POS					= 23,							
	EXP_BIAS				= 127,							
	LOOKUP_POS				= (EXP_POS-LOOKUP_BITS),
	SEED_POS				= (EXP_POS-8),
	SQRT_TABLE_SIZE			= (2<<LOOKUP_BITS),
	LOOKUP_MASK				= (SQRT_TABLE_SIZE-1)
};

union _flint
{
	dword					i;
	float					f;
};

void swapf(float * x, float * y)
{
	float temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

void swap(int * x, int * y)
{
	int temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

//static dword				iSqrt[SQRT_TABLE_SIZE];
//
//float RSqrt( float x ) {
//	long i;
//	float y, r;
//
//	y = x * 0.5f;
//	i = *(long *)( &x );
//	i = 0x5f3759df - ( i >> 1 );
//	r = *(float *)( &i );
//	r = r * ( 1.5f - r * r * y );
//	return r;
//}
//
//float InvSqrt16( float x )
//{
//	double y, r;
//
//	dword a = ((union _flint*)(&x))->i;
//	union _flint seed;
//
//	//exit( TRUE );
//
//	y = x * 0.5f;
//	seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
//	r = seed.f;
//	r = r * ( 1.5f - r * r * y );
//	return (float) r;
//}
//
//float InvSqrt( float x )
//{
//	double y, r;
//
//	dword a = ((union _flint*)(&x))->i;
//	union _flint seed;
//
//	//exit( TRUE );
//
//	y = x * 0.5f;
//	seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
//	r = seed.f;
//	r = r * ( 1.5f - r * r * y );
//	r = r * ( 1.5f - r * r * y );
//	return (float) r;
//}
//
//double InvSqrt64( float x )
//{
//	double y, r;
//
//	dword a = ((union _flint*)(&x))->i;
//	union _flint seed;
//
//	//exit( TRUE );
//
//	y = x * 0.5f;
//	seed.i = (( ( (3*EXP_BIAS-1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | iSqrt[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
//	r = seed.f;
//	r = r * ( 1.5f - r * r * y );
//	r = r * ( 1.5f - r * r * y );
//	r = r * ( 1.5f - r * r * y );
//	return r;
//}
//
//float Sqrt16( float x )
//{
//	return x * InvSqrt16( x );
//}
//
//float Sqrt( float x )
//{
//	return x * InvSqrt( x );
//}
//
//double Sqrt64( float x )
//{
//	return x * InvSqrt64( x );
//}
//
///****************************************/
///*Function: 开根号处理                  */
///*入口参数：被开方数，长整型            */
///*出口参数：开方结果，整型              */
///****************************************/
//unsigned int sqrt_16(unsigned long M)
//{
//	unsigned int N, i;
//	unsigned long tmp, ttp;   // 结果、循环计数
//	if (M == 0)               // 被开方数，开方结果也为0
//		return 0;
//
//	N = 0;
//
//	tmp = (M >> 30);          // 获取最高位：B[m-1]
//	M <<= 2;
//	if (tmp > 1)              // 最高位为1
//	{
//		N ++;                 // 结果当前位为1，否则为默认的0
//		tmp -= N;
//	}
//
//	for (i=15; i>0; i--)      // 求剩余的15位
//	{
//		N <<= 1;              // 左移一位
//
//		tmp <<= 2;
//		tmp += (M >> 30);     // 假设
//
//		ttp = N;
//		ttp = (ttp<<1)+1;
//
//		M <<= 2;
//		if (tmp >= ttp)       // 假设成立
//		{
//			tmp -= ttp;
//			N ++;
//		}
//
//	}
//
//	return N;
//}

//float Sin( float a )
//{
//	return sinf( a );
//}
//
//float Sin16( float a )
//{
//	float s;
//
//	if ( ( a < 0.0f ) || ( a >= TWO_PI ) ) {
//		a -= floorf( a / TWO_PI ) * TWO_PI;
//	}
//#if 1
//	if ( a < PI ) {
//		if ( a > HALF_PI ) {
//			a = PI - a;
//		}
//	} else {
//		if ( a > PI + HALF_PI ) {
//			a = a - TWO_PI;
//		} else {
//			a = PI - a;
//		}
//	}
//#else
//	a = PI - a;
//	if ( fabs( a ) >= HALF_PI ) {
//		a = ( ( a < 0.0f ) ? -PI : PI ) - a;
//	}
//#endif
//	s = a * a;
//	return a * ( ( ( ( ( -2.39e-08f * s + 2.7526e-06f ) * s - 1.98409e-04f ) * s + 8.3333315e-03f ) * s - 1.666666664e-01f ) * s + 1.0f );
//}
//
//double Sin64( float a ) {
//	return sin( a );
//}
//
//float Cos( float a ) {
//	return cosf( a );
//}
//
//float Cos16( float a ) {
//	float s, d;
//
//	if ( ( a < 0.0f ) || ( a >= TWO_PI ) ) {
//		a -= floorf( a / TWO_PI ) * TWO_PI;
//	}
//#if 1
//	if ( a < PI ) {
//		if ( a > HALF_PI ) {
//			a = PI - a;
//			d = -1.0f;
//		} else {
//			d = 1.0f;
//		}
//	} else {
//		if ( a > PI + HALF_PI ) {
//			a = a - TWO_PI;
//			d = 1.0f;
//		} else {
//			a = PI - a;
//			d = -1.0f;
//		}
//	}
//#else
//	a = PI - a;
//	if ( fabs( a ) >= HALF_PI ) {
//		a = ( ( a < 0.0f ) ? -PI : PI ) - a;
//		d = 1.0f;
//	} else {
//		d = -1.0f;
//	}
//#endif
//	s = a * a;
//	return d * ( ( ( ( ( -2.605e-07f * s + 2.47609e-05f ) * s - 1.3888397e-03f ) * s + 4.16666418e-02f ) * s - 4.999999963e-01f ) * s + 1.0f );
//}
//
//double Cos64( float a ) {
//	return cos( a );
//}
//
//void SinCos( float a, float * s, float * c )
//{
//#ifdef _WIN32
//	_asm {
//		fld		a
//			fsincos
//			mov		ecx, c
//			mov		edx, s
//			fstp	dword ptr [ecx]
//		fstp	dword ptr [edx]
//	}
//#else
//	s = sinf( a );
//	c = cosf( a );
//#endif
//}
//
//void SinCos16( float a, float * s, float * c )
//{
//	float t, d;
//
//	if ( ( a < 0.0f ) || ( a >= TWO_PI ) ) {
//		a -= floorf( a / TWO_PI ) * TWO_PI;
//	}
//#if 1
//	if ( a < PI ) {
//		if ( a > HALF_PI ) {
//			a = PI - a;
//			d = -1.0f;
//		} else {
//			d = 1.0f;
//		}
//	} else {
//		if ( a > PI + HALF_PI ) {
//			a = a - TWO_PI;
//			d = 1.0f;
//		} else {
//			a = PI - a;
//			d = -1.0f;
//		}
//	}
//#else
//	a = PI - a;
//	if ( fabs( a ) >= HALF_PI ) {
//		a = ( ( a < 0.0f ) ? -PI : PI ) - a;
//		d = 1.0f;
//	} else {
//		d = -1.0f;
//	}
//#endif
//	t = a * a;
//	* s = a * ( ( ( ( ( -2.39e-08f * t + 2.7526e-06f ) * t - 1.98409e-04f ) * t + 8.3333315e-03f ) * t - 1.666666664e-01f ) * t + 1.0f );
//	* c = d * ( ( ( ( ( -2.605e-07f * t + 2.47609e-05f ) * t - 1.3888397e-03f ) * t + 4.16666418e-02f ) * t - 4.999999963e-01f ) * t + 1.0f );
//}
//
//void SinCos64( float a, double * s, double * c )
//{
//#ifdef _WIN32
//	_asm {
//		fld		a
//			fsincos
//			mov		ecx, c
//			mov		edx, s
//			fstp	qword ptr [ecx]
//		fstp	qword ptr [edx]
//	}
//#else
//	s = sin( a );
//	c = cos( a );
//#endif
//}
//
//float Tan( float a )
//{
//	return tanf( a );
//}
//
//float Tan16( float a )
//{
//	float s;
//	int reciprocal;
//
//	if ( ( a < 0.0f ) || ( a >= PI ) ) {
//		a -= floorf( a / PI ) * PI;
//	}
//#if 1
//	if ( a < HALF_PI ) {
//		if ( a > ONEFOURTH_PI ) {
//			a = HALF_PI - a;
//			reciprocal = TRUE;
//		} else {
//			reciprocal = FALSE;
//		}
//	} else {
//		if ( a > HALF_PI + ONEFOURTH_PI ) {
//			a = a - PI;
//			reciprocal = FALSE;
//		} else {
//			a = HALF_PI - a;
//			reciprocal = TRUE;
//		}
//	}
//#else
//	a = HALF_PI - a;
//	if ( fabs( a ) >= ONEFOURTH_PI ) {
//		a = ( ( a < 0.0f ) ? -HALF_PI : HALF_PI ) - a;
//		reciprocal = FALSE;
//	} else {
//		reciprocal = TRUE;
//	}
//#endif
//	s = a * a;
//	s = a * ( ( ( ( ( ( 9.5168091e-03f * s + 2.900525e-03f ) * s + 2.45650893e-02f ) * s + 5.33740603e-02f ) * s + 1.333923995e-01f ) * s + 3.333314036e-01f ) * s + 1.0f );
//	if ( reciprocal ) {
//		return 1.0f / s;
//	} else {
//		return s;
//	}
//}
//
//double Tan64( float a )
//{
//	return tan( a );
//}
//
//float ASin( float a )
//{
//	if ( a <= -1.0f ) {
//		return -HALF_PI;
//	}
//	if ( a >= 1.0f ) {
//		return HALF_PI;
//	}
//	return asinf( a );
//}
//
//float ASin16( float a )
//{
//	if ( FLOATSIGNBITSET( a ) )
//	{
//		if ( a <= -1.0f )
//		{
//			return -HALF_PI;
//		}
//		a = fabs( a );
//		return ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a ) - HALF_PI;
//	}
//	else
//	{
//		if ( a >= 1.0f )
//		{
//			return HALF_PI;
//		}
//		return HALF_PI - ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
//	}
//}
//
//double ASin64( float a ) {
//	if ( a <= -1.0f ) {
//		return -HALF_PI;
//	}
//	if ( a >= 1.0f ) {
//		return HALF_PI;
//	}
//	return asin( a );
//}
//
//float ACos( float a ) {
//	if ( a <= -1.0f ) {
//		return PI;
//	}
//	if ( a >= 1.0f ) {
//		return 0.0f;
//	}
//	return acosf( a );
//}
//
//float ACos16( float a ) {
//	if ( FLOATSIGNBITSET( a ) ) {
//		if ( a <= -1.0f ) {
//			return PI;
//		}
//		a = fabs( a );
//		return PI - ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
//	} else {
//		if ( a >= 1.0f ) {
//			return 0.0f;
//		}
//		return ( ( ( -0.0187293f * a + 0.0742610f ) * a - 0.2121144f ) * a + 1.5707288f ) * sqrt( 1.0f - a );
//	}
//}
//
//double ACos64( float a ) {
//	if ( a <= -1.0f ) {
//		return PI;
//	}
//	if ( a >= 1.0f ) {
//		return 0.0f;
//	}
//	return acos( a );
//}
//
//float ATan( float a ) {
//	return atanf( a );
//}
//
//float ATan16( float a ) {
//	float s;
//
//	if ( fabs( a ) > 1.0f ) {
//		a = 1.0f / a;
//		s = a * a;
//		s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
//			* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
//		if ( FLOATSIGNBITSET( a ) ) {
//			return s - HALF_PI;
//		} else {
//			return s + HALF_PI;
//		}
//	} else {
//		s = a * a;
//		return ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
//			* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
//	}
//}
//
//double ATan64( float a ) {
//	return atan( a );
//}
//
//float ATan( float y, float x ) {
//	return atan2f( y, x );
//}
//
//float ATan16( float y, float x ) {
//	float a, s;
//
//	if ( fabs( y ) > fabs( x ) ) {
//		a = x / y;
//		s = a * a;
//		s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
//			* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
//		if ( FLOATSIGNBITSET( a ) ) {
//			return s - HALF_PI;
//		} else {
//			return s + HALF_PI;
//		}
//	} else {
//		a = y / x;
//		s = a * a;
//		return ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
//			* s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
//	}
//}
//
//double ATan64( float y, float x ) {
//	return atan2( y, x );
//}

#endif