#include "StdAfx.h"
#include "Object.h"
#include "Utility.h"

Object::Object(LPD3DXVECTOR3 pTarget):
m_Position(0, 0, 0), 
m_Direction(0.0f, 0.0f, 0.0f), 
m_Scale(1.0f, 1.0f, 1.0f), 
m_pTarget(pTarget), 
m_pWorldMatrix(NULL)
{
	getTransform();
}

Object::~Object(void)
{
}

D3DXMATRIX& Object::getTransform()
{
	if( m_pTarget != NULL )
	{
#ifdef RIGHT_HANDED
		D3DXMatrixLookAtRH(&m_Matrix, &m_Position, m_pTarget, &Y_AXIS);
#else
		D3DXMatrixLookAtLH(&m_Matrix, &m_Position, m_pTarget, &Y_AXIS);
#endif
	}
	else
	{
		D3DXMATRIX m;

		D3DXQuaternionRotationYawPitchRoll(&m_Rotation, m_Direction.y, m_Direction.x, m_Direction.z);

		D3DXMatrixRotationQuaternion(&m_Matrix, &m_Rotation);

		D3DXMatrixScaling(&m, m_Scale.x, m_Scale.y, m_Scale.z);

		m_Matrix *= m;

		D3DXMatrixTranslation(&m, m_Position.x, m_Position.y, m_Position.z);

		m_Matrix *= m;

		if(m_pWorldMatrix)
			m_Matrix *= *m_pWorldMatrix;
	}

	return m_Matrix;
}