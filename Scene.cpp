#include "stdafx.h"
#include "TerrainMesh.h"
#include "Scene.h"


CScene::CScene()
{
	m_ppObjects = NULL;
	m_nObjects = 0;
}


CScene::~CScene()
{
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CScene::BuildObjects(ID3D11Device * pd3dDevice)
{
	CShader *pShader = new CShader();
	pShader->CreateShader(pd3dDevice);

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	XMFLOAT3 color = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMVECTOR xmmtxColor = XMLoadFloat3(&color);
	
	CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f, xmmtxColor);
	
	CRotatingObject *pObject = new CRotatingObject();
	pObject->SetMesh(pMesh);
	pObject->SetShader(pShader);

	m_ppObjects[0] = pObject;
}

void CScene::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}
}

bool CScene::ProcessInput()
{
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nObjects; i++)
		m_ppObjects[i]->Animate(fTimeElapsed);
}

void CScene::Render(ID3D11DeviceContext * pd3dDeviceContext, CCamera *pCamera)
{
	for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->Render(pd3dDeviceContext);
	}
}
