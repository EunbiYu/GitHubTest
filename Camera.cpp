#include "stdafx.h"
#include "Player.h"
#include "Camera.h"

CCamera::CCamera()
{
	m_pPlayer = NULL;
	
	XMStoreFloat4x4(&m_d3dxmtxView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_d3dxmtxProjection, XMMatrixIdentity());

	m_pd3dcbCamera = NULL;
}


CCamera::~CCamera()
{
	if (m_pd3dcbCamera)
		m_pd3dcbCamera->Release();
}

void CCamera::SetViewport(ID3D11DeviceContext * pd3dDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
	pd3dDeviceContext->RSSetViewports(1, &m_d3dViewport);
}

void CCamera::GenerateViewMatrix(XMVECTOR d3dxvEyePosition, XMVECTOR d3dxvLookAt, XMVECTOR d3dxvUp)
{
	XMStoreFloat4x4(&m_d3dxmtxView, XMMatrixLookAtLH(d3dxvEyePosition, d3dxvLookAt, d3dxvUp));
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	XMStoreFloat4x4(&m_d3dxmtxProjection, XMMatrixPerspectiveFovLH((float)XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance,
		fFarPlaneDistance));
}

void CCamera::CreateShaderVariables(ID3D11Device * pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_CAMERA);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbCamera);
}

void CCamera::UpdateShaderVariables(ID3D11DeviceContext * pd3dDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	// 상수 버퍼의 메모리 주소를 가져와서 카메라 변환 행렬과 투영 변환 행렬을 복사한다.
	// 쉐이더에서 행렬의 행과 열이 바뀌는 것에 주의하라.
	pd3dDeviceContext->Map(m_pd3dcbCamera, 0, D3D11_MAP_WRITE_DISCARD, 0,
		&d3dMappedResource);
	VS_CB_CAMERA *pcbViewProjection = (VS_CB_CAMERA *)d3dMappedResource.pData;
	
	XMMATRIX xmmtxView = XMLoadFloat4x4(&m_d3dxmtxView);
	XMMATRIX xmmtxProjection = XMLoadFloat4x4(&m_d3dxmtxProjection);

	XMStoreFloat4x4(&(pcbViewProjection->m_d3dxmtxView), XMMatrixTranspose(xmmtxView));
	XMStoreFloat4x4(&(pcbViewProjection->m_d3dxmtxProjection), XMMatrixTranspose(xmmtxProjection));

	pd3dDeviceContext->Unmap(m_pd3dcbCamera, 0);

	// 상수 버퍼를 슬롯(VS_SLOT_CAMERA)에 설정한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_CAMERA, 1, &m_pd3dcbCamera);
}
