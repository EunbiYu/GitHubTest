#pragma once
#include "Mesh.h"
class CCubeMesh :
	public CMesh
{
public:
	CCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, XMVECTOR d3dxColor);
	virtual ~CCubeMesh();

protected:
	ID3D11Buffer					*m_pd3dColorBuffer;
};

