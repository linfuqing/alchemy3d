# include "Matrix3D.h"

typedef struct
{
	Number x;
	Number y;
	Number z;
	Number w;
}Quaternion;

Queaternion newQuaternion( Number x, Number y, Number z, Number w )
{
	Quaternion q;

	q.x = x;
	q.y = y;
	q.z = z;
	q.w = w;
}

Queaternion clone( Quaternion q )
{
	return newQuaternion( q.x, q.y, q.z, q.w );
}

Queaternion multiply( Queaternion q1, Queaternion q2 )
{
	Queaternion q;
	
	q.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    q.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    q.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    q.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

	return q;
}

Matrix3D toMatrix3D( Queaternion q )
{
}
