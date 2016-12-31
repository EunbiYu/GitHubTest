#pragma once

struct VS_CB_CAMERA
{
	XMFLOAT4X4 m_d3dxmtxView;
	XMFLOAT4X4 m_d3dxmtxProjection;
};

class CPlayer;

class CCamera
{
protected:
	// 카메라 변환 행렬과 투영 변환 행렬을 나타내는 멤버 변수를 선언.
	XMFLOAT4X4 m_d3dxmtxView;
	XMFLOAT4X4 m_d3dxmtxProjection;

	// 뷰-포트를 나타내는 멤버 변수를 선언한다.
	D3D11_VIEWPORT m_d3dViewport;

	// 카메라 변환 행렬과 투영 변환 행렬을 위한 상수 버퍼 인터페이스 포인터를 선언한다.
	ID3D11Buffer *m_pd3dcbCamera;

	// 카메라에 연결된 플레이어 객체에 대한 포인터를 선언한다.
	CPlayer *m_pPlayer;

public:
	CCamera();
	virtual ~CCamera();

public:
	void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }

	XMFLOAT4X4& GetViewMatrixXMFLOAT4X4() { return (m_d3dxmtxView); }
	XMFLOAT4X4& GetProjectionMatrixXMFLOAT4X4() { return (m_d3dxmtxProjection); }

	// 뷰-포트를 설정하는 멤버 함수를 선언한다.
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext,
		DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight,
		float fMinZ = 0.0f, float fMaxZ = 1.0f);

	// 카메라 변환 행렬과 투영 변환 행렬을 생성하는 멤버 함수를 선언한다.
	void GenerateViewMatrix(XMVECTOR d3dxvEyePosition, XMVECTOR
		d3dxvLookAt, XMVECTOR d3dxvUp);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float
		fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	// 상수 버퍼를 생성하고 내용을 갱신하는 멤버 함수를 선언한다.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
};

