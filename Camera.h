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
	// ī�޶� ��ȯ ��İ� ���� ��ȯ ����� ��Ÿ���� ��� ������ ����.
	XMFLOAT4X4 m_d3dxmtxView;
	XMFLOAT4X4 m_d3dxmtxProjection;

	// ��-��Ʈ�� ��Ÿ���� ��� ������ �����Ѵ�.
	D3D11_VIEWPORT m_d3dViewport;

	// ī�޶� ��ȯ ��İ� ���� ��ȯ ����� ���� ��� ���� �������̽� �����͸� �����Ѵ�.
	ID3D11Buffer *m_pd3dcbCamera;

	// ī�޶� ����� �÷��̾� ��ü�� ���� �����͸� �����Ѵ�.
	CPlayer *m_pPlayer;

public:
	CCamera();
	virtual ~CCamera();

public:
	void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }

	XMFLOAT4X4& GetViewMatrixXMFLOAT4X4() { return (m_d3dxmtxView); }
	XMFLOAT4X4& GetProjectionMatrixXMFLOAT4X4() { return (m_d3dxmtxProjection); }

	// ��-��Ʈ�� �����ϴ� ��� �Լ��� �����Ѵ�.
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext,
		DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight,
		float fMinZ = 0.0f, float fMaxZ = 1.0f);

	// ī�޶� ��ȯ ��İ� ���� ��ȯ ����� �����ϴ� ��� �Լ��� �����Ѵ�.
	void GenerateViewMatrix(XMVECTOR d3dxvEyePosition, XMVECTOR
		d3dxvLookAt, XMVECTOR d3dxvUp);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float
		fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	// ��� ���۸� �����ϰ� ������ �����ϴ� ��� �Լ��� �����Ѵ�.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
};

