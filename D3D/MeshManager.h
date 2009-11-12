#pragma once

#define TRIANGLE_NODE_NUMBERS 3

typedef struct VERTEX
{
	VECTOR3D position;
	FLOAT    size;

	VERTEX(FLOAT x = 0, FLOAT y = 0, FLOAT z = 0) : position(x, y, z), size(0)
	{
	}

}VERTEX, * LPVERTEX;

typedef struct TRIANGLE
{
	DWORD    index[TRIANGLE_NODE_NUMBERS];
	COLOR    color[TRIANGLE_NODE_NUMBERS];
	FLOAT    w[TRIANGLE_NODE_NUMBERS];
	FLOAT    blend[TRIANGLE_NODE_NUMBERS][MAX_BLEND];
	VECTOR2D uv[TRIANGLE_NODE_NUMBERS][MAX_TEXTURE];

	TRIANGLE(DWORD nIndex1 = 0, DWORD nIndex2 = 0, DWORD nIndex3 = 0)
	{
		index[0] = nIndex1;
		index[1] = nIndex2;
		index[2] = nIndex3;

		for(int j, i = 0; i < TRIANGLE_NODE_NUMBERS; i++)
		{
			color[i] = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			    w[i] = 0;
			
			for(j = 0; j < MAX_BLEND; j++)
				blend[i][j] = 0;

			for(j = 0; j < MAX_TEXTURE; j++)
				uv[i][j] = D3DXVECTOR2(0,0);
		}
	}

}TRIANGLE, * LPTRIANGLE;

class MeshManager
{
private:
	typedef struct TRIANGLEMANAGER
	{
		LPTRIANGLE       triangle;

		DWORD            index;

		TRIANGLEMANAGER* next, * previous;

		TRIANGLEMANAGER(LPTRIANGLE pTriangle, DWORD nIndex) : triangle(pTriangle), index(nIndex), next(NULL), previous(NULL)
		{
		}

		~TRIANGLEMANAGER()
		{
			TRIANGLEMANAGER* pTriangleManager = next, * pPrevious;

			while(pTriangleManager)
			{
				pPrevious = pTriangleManager;

				pTriangleManager = pTriangleManager->next;

				delete pPrevious;
			}
		}

	}TRIANGLEMANAGER, * LPTRIANGLEMANAGER;

	typedef struct VERTEXMANAGER
	{
		LPVERTEX vertex;

		LPTRIANGLEMANAGER triangleManager, currentTriangle;

		VERTEXMANAGER(LPVERTEX pVertex = NULL) : vertex(pVertex), triangleManager(NULL), currentTriangle(NULL)
		{
		}

		~VERTEXMANAGER()
		{
			delete triangleManager;
		}

		void addTriangle(LPTRIANGLE pTriangle, DWORD nIndex)
		{
			if(!currentTriangle)
			{
				currentTriangle = new TRIANGLEMANAGER(pTriangle, nIndex);
				triangleManager = currentTriangle;

				return;
			}

			currentTriangle->next           = new TRIANGLEMANAGER(pTriangle, nIndex);
			currentTriangle->next->previous = currentTriangle;
			currentTriangle                 = currentTriangle->next;
		}

	}VERTEXMANAGER, * LPVERTEXMANAGER;

	LPVERTEXMANAGER m_pVertexManager;

	LPVERTEX   m_pVertices;
	LPTRIANGLE m_pTriangles;

	DWORD m_nNumVertices;
	DWORD m_nNumTriangles;

	DWORD __getTextureCount(DWORD nFVF);

public:
	MeshManager(DWORD nNumVertex, DWORD nNumFaces);
	~MeshManager(void);

	void reset(DWORD nNumVertex, DWORD nNumFaces);

	LPVERTEX   getVertices();
	LPTRIANGLE getTriangles();

	LPMESH createMesh(DWORD nFVF, DWORD nOptions, LPDEVICE pDevice);

	inline DWORD getNumVertices()const;
	inline DWORD getNumTriangles()const;
};

inline DWORD MeshManager::getNumVertices()const
{
	return m_nNumVertices;
}

inline DWORD MeshManager::getNumTriangles()const
{
	return m_nNumTriangles;
}
