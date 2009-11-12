#pragma once

#include "Viewport.h"

class ObjectContainer;
class Geometry;

class Object
{
	
	friend class ObjectContainer;
	friend class Geometry;

private:

	D3DXQUATERNION m_Rotation;

protected:
		
	VECTOR3D m_Position;
	VECTOR3D m_Direction;
	VECTOR3D m_Scale;

	LPVECTOR3D m_pTarget;

	LPMATRIX3D m_pWorldMatrix;
	MATRIX3D   m_Matrix;

public:
	Object(LPD3DXVECTOR3 target = NULL);

	virtual ~Object(void);

	/*inline void SetX(FLOAT fValue);
	inline void SetY(FLOAT fValue);
	inline void SetZ(FLOAT fValue);

	inline void SetRotationX(FLOAT fValue);
	inline void SetRotationY(FLOAT fValue);
	inline void SetRotationZ(FLOAT fValue);

	inline void SetScaleX(FLOAT fValue);
	inline void SetScaleY(FLOAT fValue);
	inline void SetScaleZ(FLOAT fValue);*/

	inline void setTarget(LPD3DXVECTOR3 pTarget);

	/*inline FLOAT GetX()const;
	inline FLOAT GetY()const;
	inline FLOAT GetZ()const;

	inline FLOAT GetRotationX()const;
	inline FLOAT GetRotationY()const;
	inline FLOAT GetRotationZ()const;

	inline FLOAT GetScaleX()const;
	inline FLOAT GetScaleY()const;
	inline FLOAT GetScaleZ()const;*/

	inline VECTOR3D& position();
	inline VECTOR3D& rotation();
	inline VECTOR3D& scale();

	inline LPVECTOR3D getTarget()const;

	virtual D3DXMATRIX& getTransform();

	virtual void render(Viewport* pViewport) = 0;
};

/*inline void Object::SetX(FLOAT fValue)
{
	m_Position.x = fValue;
}

inline void  Object::SetY(FLOAT fValue)
{
	m_Position.y = fValue;
}

inline void  Object::SetZ(FLOAT fValue)
{
	m_Position.z = fValue;
}


inline void  Object::SetRotationX(FLOAT fValue)
{
	m_Direction.x = fValue;
}

inline void  Object::SetRotationY(FLOAT fValue)
{
	m_Direction.y = fValue;
}

inline void  Object::SetRotationZ(FLOAT fValue)
{
	m_Direction.z = fValue;
}

inline void  Object::SetScaleX(FLOAT fValue)
{
	m_Scale.x = fValue;
}

inline void  Object::SetScaleY(FLOAT fValue)
{
	m_Scale.y = fValue;
}

inline void  Object::SetScaleZ(FLOAT fValue)
{
	m_Scale.z = fValue;
}*/

inline void Object::setTarget(LPD3DXVECTOR3 pTarget)
{
	m_pTarget = pTarget;
}

/*inline FLOAT  Object::GetX()const
{
	return m_Position.x;
}

inline FLOAT  Object::GetY()const
{
	return m_Position.y;
}

inline FLOAT  Object::GetZ()const
{
	return m_Position.z;
}


inline FLOAT  Object::GetRotationX()const
{
	return m_Direction.x;
}

inline FLOAT  Object::GetRotationY()const
{
	return m_Direction.y;
}

inline FLOAT  Object::GetRotationZ()const
{
	return m_Direction.z;
}


inline FLOAT  Object::GetScaleX()const
{
	return m_Scale.x;
}

inline FLOAT  Object::GetScaleY()const
{
	return m_Scale.y;
}

inline FLOAT  Object::GetScaleZ()const
{
	return m_Scale.z;
}*/

inline VECTOR3D& Object::position()
{
	return m_Position;
}

inline VECTOR3D& Object::rotation()
{
	return m_Direction;
}

inline VECTOR3D& Object::scale()
{
	return m_Scale;
}

inline LPVECTOR3D Object::getTarget()const
{
	return m_pTarget;
}