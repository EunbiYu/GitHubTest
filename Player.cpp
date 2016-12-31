#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer()
{
	m_pCamera = NULL;
}


CPlayer::~CPlayer()
{
	if (m_pCamera)
		delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D11Device * pd3dDevice)
{
}

void CPlayer::UpdateShaderVariables(ID3D11DeviceContext * pd3dDeviceContext)
{
	if (m_pCamera)
		m_pCamera->UpdateShaderVariables(pd3dDeviceContext);
}

//test test