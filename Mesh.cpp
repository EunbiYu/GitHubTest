#include "stdafx.h"
#include "Mesh.h"

#define VERTEX_POSITION_ELEMENT			0x01
#define VERTEX_COLOR_ELEMENT			0x02
#define VERTEX_NORMAL_ELEMENT			0x04

CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nType = 0;

	m_nBuffers = 0;
	m_pd3dPositionBuffer = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nSlot = 0;
	m_nStartVertex = 0;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
	m_nIndexOffset = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;

	m_pd3dRasterizerState = NULL;

	m_pd3dxvPositions = NULL;
	m_pnIndices = NULL;

	m_bcBoundingCube.m_d3dxvMinimum = XMFLOAT3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	m_nReferences = 0;
}

CMesh::~CMesh()
{
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

	if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
	if (m_pnVertexStrides) delete[] m_pnVertexStrides;
	if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;

	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pnIndices) delete[] m_pnIndices;
}

void CMesh::AssembleToVertexBuffer(int nBuffers, ID3D11Buffer **ppd3dBuffers, UINT *pnBufferStrides, UINT *pnBufferOffsets)
{
	ID3D11Buffer **ppd3dNewVertexBuffers = new ID3D11Buffer*[m_nBuffers + nBuffers];
	UINT *pnNewVertexStrides = new UINT[m_nBuffers + nBuffers];
	UINT *pnNewVertexOffsets = new UINT[m_nBuffers + nBuffers];

	if (m_nBuffers > 0)
	{
		for (int i = 0; i < m_nBuffers; i++)
		{
			ppd3dNewVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnNewVertexStrides[i] = m_pnVertexStrides[i];
			pnNewVertexOffsets[i] = m_pnVertexOffsets[i];
		}
		if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
		if (m_pnVertexStrides) delete[] m_pnVertexStrides;
		if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
	}

	for (int i = 0; i < nBuffers; i++)
	{
		ppd3dNewVertexBuffers[m_nBuffers + i] = ppd3dBuffers[i];
		pnNewVertexStrides[m_nBuffers + i] = pnBufferStrides[i];
		pnNewVertexOffsets[m_nBuffers + i] = pnBufferOffsets[i];
	}

	m_nBuffers += nBuffers;
	m_ppd3dVertexBuffers = ppd3dNewVertexBuffers;
	m_pnVertexStrides = pnNewVertexStrides;
	m_pnVertexOffsets = pnNewVertexOffsets;
}

void CMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
}

void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}

bool RayIntersectTriangle(XMVECTOR *pd3dxvOrigin, XMVECTOR *pd3dxvDirection, XMVECTOR *pd3dxvP0, XMVECTOR *pd3dxvP1, XMVECTOR *pd3dxvP2, float *pfU, float *pfV, float *pfRayToTriangle)
{
	XMVECTOR d3dxvEdge1 = *pd3dxvP1 - *pd3dxvP0;
	XMVECTOR d3dxvEdge2 = *pd3dxvP2 - *pd3dxvP0;
	XMVECTOR d3dxvP, d3dxvQ;
	d3dxvP = XMVector3Cross(*pd3dxvDirection, d3dxvEdge2);
	
	// 내적값 스칼라가 필요하다.
	// XMVector3Dot을 이용하면 return 값이 VECTOR이다.
	// vector의 모든 값은 같으므로 다음과 같이 쓸 수 있다.
	XMVECTOR vec = XMVector3Dot(d3dxvEdge1, d3dxvP);
	XMFLOAT3 tempv3;
	XMStoreFloat3(&tempv3, vec);
	float a = tempv3.x;

	if (::IsZero(a)) return(false);
	float f = 1.0f / a;
	XMVECTOR d3dxvP0ToOrigin = *pd3dxvOrigin - *pd3dxvP0;
	*pfU = f * XMVectorGetX(XMVector3Dot(d3dxvP0ToOrigin, d3dxvP));
	if ((*pfU < 0.0f) || (*pfU > 1.0f)) return(false);
	
	d3dxvQ = XMVector3Cross(d3dxvP0ToOrigin, d3dxvEdge1);

	*pfV = f * XMVectorGetX(XMVector3Dot(*pd3dxvDirection, d3dxvQ));
	if ((*pfV < 0.0f) || ((*pfU + *pfV) > 1.0f)) return(false);
	*pfRayToTriangle = f * XMVectorGetX(XMVector3Dot(d3dxvEdge2, d3dxvQ));
	return(*pfRayToTriangle >= 0.0f);
}

int CMesh::CheckRayIntersection(XMVECTOR *pd3dxvRayPosition, XMVECTOR *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE *)m_pd3dxvPositions + m_pnVertexOffsets[0];

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	XMVECTOR v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(XMVECTOR *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(XMVECTOR *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(XMVECTOR *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_pnVertexStrides[0]);
#ifdef _WITH_D3DX_LIBRARY
		if (D3DXIntersectTri(&v0, &v1, &v2, pd3dxvRayPosition, pd3dxvRayDirection, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
#else
		if (::RayIntersectTriangle(pd3dxvRayPosition, pd3dxvRayDirection, &v0, &v1, &v2, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
#endif
		{
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}
	return(nIntersections);
}

void AABB::Merge(XMFLOAT3 d3dxvMinimum, XMFLOAT3 d3dxvMaximum)
{
	if (d3dxvMinimum.x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = d3dxvMinimum.x;
	if (d3dxvMinimum.y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = d3dxvMinimum.y;
	if (d3dxvMinimum.z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = d3dxvMinimum.z;
	if (d3dxvMaximum.x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = d3dxvMaximum.x;
	if (d3dxvMaximum.y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = d3dxvMaximum.y;
	if (d3dxvMaximum.z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = d3dxvMaximum.z;
}

void AABB::Merge(AABB * pAABB)
{
	Merge(pAABB->m_d3dxvMinimum, pAABB->m_d3dxvMaximum);
}

void AABB::Update(XMMATRIX& pd3dxmtxTransform)
{
	XMVECTOR vVertices[8];

	vVertices[0] = XMVectorSet(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z, 0.0f);
	vVertices[1] = XMVectorSet(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z, 0.0f);
	vVertices[2] = XMVectorSet(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z, 0.0f);
	vVertices[3] = XMVectorSet(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z, 0.0f);
	vVertices[4] = XMVectorSet(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z, 0.0f);
	vVertices[5] = XMVectorSet(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z, 0.0f);
	vVertices[6] = XMVectorSet(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z, 0.0f);
	vVertices[7] = XMVectorSet(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z, 0.0f);
	
	XMStoreFloat3(&m_d3dxvMinimum, XMVectorSet(+FLT_MAX, +FLT_MAX, +FLT_MAX, 0.0f));
	XMStoreFloat3(&m_d3dxvMaximum, XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0.0f));
	
	for (int i = 0; i < 8; i++)
	{
		XMFLOAT3 xf3Vector;
		XMStoreFloat3(&xf3Vector, XMVector3TransformCoord(vVertices[i], pd3dxmtxTransform));

		if (xf3Vector.x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = xf3Vector.x;
		if (xf3Vector.y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = xf3Vector.y;
		if (xf3Vector.z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = xf3Vector.z;
		if (xf3Vector.x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = xf3Vector.x;
		if (xf3Vector.y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = xf3Vector.y;
		if (xf3Vector.z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = xf3Vector.z;
	}
}

CMeshDiffused::CMeshDiffused(ID3D11Device * pd3dDevice)
	:CMesh(pd3dDevice)
{
	m_pd3dColorBuffer = NULL;
}

CMeshDiffused::~CMeshDiffused()
{
	if (m_pd3dColorBuffer) m_pd3dColorBuffer->Release();
}

//CMeshIlluminated::CMeshIlluminated(ID3D11Device * pd3dDevice)
//	:CMesh(pd3dDevice)
//{
//	m_pd3dNormalBuffer = NULL;
//}
//
//CMeshIlluminated::~CMeshIlluminated()
//{
//	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
//}
//
//XMVECTOR CMeshIlluminated::CalculateTriAngleNormal(UINT nIndex0, UINT nIndex1, UINT nIndex2)
//{
//	XMVECTOR d3dxvNormal(0.0f, 0.0f, 0.0f);
//	XMVECTOR d3dxvP0 = m_pd3dxvPositions[nIndex0];
//	XMVECTOR d3dxvP1 = m_pd3dxvPositions[nIndex1];
//	XMVECTOR d3dxvP2 = m_pd3dxvPositions[nIndex2];
//	XMVECTOR d3dxvEdge1 = d3dxvP1 - d3dxvP0;
//	XMVECTOR d3dxvEdge2 = d3dxvP2 - d3dxvP0;
//	D3DXVec3Cross(&d3dxvNormal, &d3dxvEdge1, &d3dxvEdge2);
//	D3DXVec3Normalize(&d3dxvNormal, &d3dxvNormal);
//	return(d3dxvNormal);
//}
//
//void CMeshIlluminated::SetTriAngleListVertexNormal(XMVECTOR * pd3dxvNormals)
//{
//	int nPrimitives = m_nVertices / 3;
//	for (int i = 0; i < nPrimitives; i++)
//	{
//		XMVECTOR d3dxvNormal = CalculateTriAngleNormal((i * 3 + 0), (i * 3 + 1), (i * 3 + 2));
//		pd3dxvNormals[i * 3 + 0] = pd3dxvNormals[i * 3 + 1] = pd3dxvNormals[i * 3 + 2] = d3dxvNormal;
//	}
//}
//
//void CMeshIlluminated::SetAverageVertexNormal(XMVECTOR * pd3dxvNormals, int nPrimitives, int nOffset, bool bStrip)
//{
//	for (int j = 0; j < m_nVertices; j++)
//	{
//		XMVECTOR d3dxvSumOfNormal = XMVECTOR(0.0f, 0.0f, 0.0f);
//		for (int i = 0; i < nPrimitives; i++)
//		{
//			UINT nIndex0 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 0) : (i*nOffset + 1)) : (i*nOffset + 0);
//			if (m_pnIndices) nIndex0 = m_pnIndices[nIndex0];
//			UINT nIndex1 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 1) : (i*nOffset + 0)) : (i*nOffset + 1);
//			if (m_pnIndices) nIndex1 = m_pnIndices[nIndex1];
//			UINT nIndex2 = (m_pnIndices) ? m_pnIndices[i*nOffset + 2] : (i*nOffset + 2);
//			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)) d3dxvSumOfNormal += CalculateTriAngleNormal(nIndex0, nIndex1, nIndex2);
//		}
//		D3DXVec3Normalize(&d3dxvSumOfNormal, &d3dxvSumOfNormal);
//		pd3dxvNormals[j] = d3dxvSumOfNormal;
//	}
//}
//
//void CMeshIlluminated::CalculateVertexNormal(XMVECTOR * pd3dxvNormals)
//{
//	switch (m_d3dPrimitiveTopology)
//	{
//	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
//		if (m_pnIndices)
//			SetAverageVertexNormal(pd3dxvNormals, (m_nIndices / 3), 3, false);
//		else
//			SetTriAngleListVertexNormal(pd3dxvNormals);
//		break;
//	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
//		SetAverageVertexNormal(pd3dxvNormals, (m_pnIndices) ? (m_nIndices - 2) : (m_nVertices - 2), 1, true);
//		break;
//	default:
//		break;
//	}
//}
//
//CMeshTextured::CMeshTextured(ID3D11Device * pd3dDevice)
//	:CMesh(pd3dDevice)
//{
//	m_pd3dTexCoordBuffer = NULL;
//}
//
//CMeshTextured::~CMeshTextured()
//{
//	if (m_pd3dTexCoordBuffer)
//		m_pd3dTexCoordBuffer->Release();
//}
//
//CMeshDetailTextured::CMeshDetailTextured(ID3D11Device * pd3dDevice)
//	:CMeshTextured(pd3dDevice)
//{
//	m_pd3dDetailTexCoordBuffer = nullptr;
//}
//
//CMeshDetailTextured::~CMeshDetailTextured()
//{
//	if (m_pd3dDetailTexCoordBuffer)
//		m_pd3dDetailTexCoordBuffer->Release();
//}
//
//CMeshTexturedIlluminated::CMeshTexturedIlluminated(ID3D11Device *pd3dDevice)
//	:CMeshIlluminated(pd3dDevice)
//{
//	m_pd3dTexCoordBuffer = nullptr;
//}
//
//CMeshTexturedIlluminated::~CMeshTexturedIlluminated()
//{
//	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
//}
//
//CMeshDetailTexturedIlluminated::CMeshDetailTexturedIlluminated(ID3D11Device *pd3dDevice)
//	:CMeshIlluminated(pd3dDevice)
//{
//	m_pd3dTexCoordBuffer = nullptr;
//	m_pd3dDetailTexCoordBuffer = nullptr;
//}
//
//CMeshDetailTexturedIlluminated::~CMeshDetailTexturedIlluminated()
//{
//	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
//	if (m_pd3dDetailTexCoordBuffer) m_pd3dDetailTexCoordBuffer->Release();
//}
//
//CHeightMapGridMesh::CHeightMapGridMesh(ID3D11Device * pd3dDevice, int xStart, int zStart, int nWidth, int nLength, XMVECTOR d3dxvScale, void * pContext)
//	:CMeshDetailTexturedIlluminated(pd3dDevice)
//{
//	m_nVertices = nWidth * nLength;
//	m_nType = VERTEX_POSITION_ELEMENT | VERTEX_COLOR_ELEMENT;
//	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
//
//	m_pd3dxvPositions = new XMVECTOR[m_nVertices];
//	//XMVECTOR *pd3dxvNormals = new XMVECTOR[m_nVertices];
//	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
//	D3DXVECTOR2 *pd3dxvDetailTexCoords = new D3DXVECTOR2[m_nVertices];
//
//	m_nWidth = nWidth;
//	m_nLength = nLength;
//	m_d3dxvScale = d3dxvScale;
//
//	CHeightMap *pHeightMap = (CHeightMap *)pContext;
//	int cxHeightMap = pHeightMap->GetHeightMapWidth();
//	int czHeightMap = pHeightMap->GetHeightMapLength();
//	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
//	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
//	{
//		for (int x = xStart; x < (xStart + nWidth); x++, i++)
//		{
//			fHeight = OnGetHeight(x, z, pContext);
//			m_pd3dxvPositions[i] = XMVECTOR((x*m_d3dxvScale.x), fHeight, (z*m_d3dxvScale.z));
//			pd3dxvTexCoords[i] = D3DXVECTOR2(float(x) / float(cxHeightMap - 1),
//				float(czHeightMap - 1 - z) / float(czHeightMap - 1));
//			pd3dxvDetailTexCoords[i] = D3DXVECTOR2(float(x) / float(m_d3dxvScale.x *0.5f),
//				float(z) / float(m_d3dxvScale.z*0.5f));
//			//pd3dxvNormals[i] = pHeightMap->GetHeightMapNormal(x, z);
//			if (fHeight < fMinHeight) fMinHeight = fHeight;
//			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
//		}
//	}
//
//	D3D11_BUFFER_DESC d3dBufferDesc;
//	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
//	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	d3dBufferDesc.ByteWidth = sizeof(XMVECTOR) * m_nVertices;
//	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	d3dBufferDesc.CPUAccessFlags = 0;
//	D3D11_SUBRESOURCE_DATA d3dBufferData;
//	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
//	d3dBufferData.pSysMem = m_pd3dxvPositions;
//	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
//
//	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
//	d3dBufferData.pSysMem = pd3dxvTexCoords;
//	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);
//
//	d3dBufferData.pSysMem = pd3dxvDetailTexCoords;
//	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dDetailTexCoordBuffer);
//
//	if (pd3dxvTexCoords) delete[] pd3dxvTexCoords;
//	if (pd3dxvDetailTexCoords) delete[] pd3dxvDetailTexCoords;
//
//	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dDetailTexCoordBuffer };
//	UINT pnBufferStrides[3] = { sizeof(XMVECTOR), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR2) };
//	UINT pnBufferOffsets[3] = { 0, 0, 0 };
//	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
//
//	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
//	m_pnIndices = new UINT[m_nIndices];
//	for (int j = 0, z = 0; z < nLength - 1; z++)
//	{
//		if ((z % 2) == 0)
//		{
//			for (int x = 0; x < nWidth; x++)
//			{
//				if ((x == 0) && (z > 0)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
//				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
//				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
//			}
//		}
//		else
//		{
//			for (int x = nWidth - 1; x >= 0; x--)
//			{
//				if (x == (nWidth - 1)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
//				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
//				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
//			}
//		}
//	}
//
//	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
//	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
//	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	d3dBufferDesc.CPUAccessFlags = 0;
//	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
//	d3dBufferData.pSysMem = m_pnIndices;
//	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);
//
//	m_bcBoundingCube.m_d3dxvMinimum = XMVECTOR(xStart*m_d3dxvScale.x, fMinHeight, zStart*m_d3dxvScale.z);
//	m_bcBoundingCube.m_d3dxvMaximum = XMVECTOR((xStart + nWidth)*m_d3dxvScale.x, fMaxHeight, (zStart + nLength)*m_d3dxvScale.z);
//}
//
//CHeightMapGridMesh::~CHeightMapGridMesh()
//{
//}
//
//float CHeightMapGridMesh::OnGetHeight(int x, int z, void * pContext)
//{
//	CHeightMap *pHeightMap = (CHeightMap *)pContext;
//	BYTE *pHeightMapImage = pHeightMap->GetHeightMapImage();
//	XMVECTOR d3dxvScale = pHeightMap->GetScale();
//	int nWidth = pHeightMap->GetHeightMapWidth();
//	float fHeight = pHeightMapImage[x + (z*nWidth)] * d3dxvScale.y;
//	return(fHeight);
//}


// 그거했는데 이상해