#include "stdafx.h"
#include "Mesh.h"
#include "TerrainMesh.h"

CCubeMesh::CCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, XMVECTOR d3dxColor)
	:CMesh(pd3dDevice)
{
	m_nVertices = 8;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new XMFLOAT3[m_nVertices];

	m_pd3dxvPositions[0] = XMFLOAT3(-fx, +fy, -fz);
	m_pd3dxvPositions[1] = XMFLOAT3(+fx, +fy, -fz);
	m_pd3dxvPositions[2] = XMFLOAT3(+fx, +fy, +fz);
	m_pd3dxvPositions[3] = XMFLOAT3(-fx, +fy, +fz);
	m_pd3dxvPositions[4] = XMFLOAT3(-fx, -fy, -fz);
	m_pd3dxvPositions[5] = XMFLOAT3(+fx, -fy, -fz);
	m_pd3dxvPositions[6] = XMFLOAT3(+fx, -fy, +fz);
	m_pd3dxvPositions[7] = XMFLOAT3(-fx, -fy, +fz);


	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	XMCOLOR pd3dxColors[8];
	for (int i = 0; i < 8; i++) XMStoreColor(&pd3dxColors[i], d3dxColor);

	d3dBufferDesc.ByteWidth = sizeof(XMCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(XMFLOAT3), sizeof(XMCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

#define _WITH_CORRECT_STRIP
#ifdef _WITH_CORRECT_STRIP
	m_nIndices = 18;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 5; //5,6,4 - cw
	m_pnIndices[1] = 6; //6,4,7 - ccw
	m_pnIndices[2] = 4; //4,7,0 - cw
	m_pnIndices[3] = 7; //7,0,3 - ccw
	m_pnIndices[4] = 0; //0,3,1 - cw
	m_pnIndices[5] = 3; //3,1,2 - ccw
	m_pnIndices[6] = 1; //1,2,2 - cw 
	m_pnIndices[7] = 2; //2,2,3 - ccw
	m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index
	m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index
	m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index
	m_pnIndices[11] = 7;//7,2,6 - ccw
	m_pnIndices[12] = 2;//2,6,1 - cw
	m_pnIndices[13] = 6;//6,1,5 - ccw
	m_pnIndices[14] = 1;//1,5,0 - cw
	m_pnIndices[15] = 5;//5,0,4 - ccw
	m_pnIndices[16] = 0;
	m_pnIndices[17] = 4;
#else
	//m_nIndices = 16;
	//m_pnIndices = new UINT[m_nIndices];

	//m_pnIndices[0] = 5; //5,6,4 - cw
	//m_pnIndices[1] = 6; //6,4,7 - ccw
	//m_pnIndices[2] = 4; //4,7,0 - cw
	//m_pnIndices[3] = 7; //7,0,3 - ccw
	//m_pnIndices[4] = 0; //0,3,1 - cw
	//m_pnIndices[5] = 3; //3,1,2 - ccw
	//m_pnIndices[6] = 1; //1,2,3 - cw 
	//m_pnIndices[7] = 2; //2,3,7 - ccw
	//m_pnIndices[8] = 3; //3,7,2 - cw - Degenerated Index
	//m_pnIndices[9] = 7; //7,2,6 - ccw
	//m_pnIndices[10] = 2;//2,6,1 - cw
	//m_pnIndices[11] = 6;//6,1,5 - ccw
	//m_pnIndices[12] = 1;//1,5,0 - cw
	//m_pnIndices[13] = 5;//5,0,4 - ccw
	//m_pnIndices[14] = 0;
	//m_pnIndices[15] = 4;
#endif

	::ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	::ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	m_bcBoundingCube.m_d3dxvMinimum = XMFLOAT3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = XMFLOAT3(+fx, +fy, +fz);

	CreateRasterizerState(pd3dDevice);
}


CCubeMesh::~CCubeMesh()
{
}
