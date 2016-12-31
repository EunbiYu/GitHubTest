#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDeviceContext = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	m_pPlayer = NULL;
	_tcscpy_s(m_pszBuffer, _T("LapProject ("));
}


CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	if (!CreateDirect3DDisplay())
		return(false);

	BuildObjects();

	return (true);
}

bool CGameFramework::CreateRenderTargetView()
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0,
		__uuidof(ID3D11Texture2D), (LPVOID*)&pd3dBackBuffer)))
		return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer,
		NULL, &m_pd3dRenderTargetView)))
		return(false);

	if (pd3dBackBuffer)
		pd3dBackBuffer->Release();

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, NULL);

	return (true);
}

bool CGameFramework::CreateDirect3DDisplay()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	UINT dwCreateDeviceFlags = 0;
#ifdef DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG
	
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D_DRIVER_TYPE);

	D3D_FEATURE_LEVEL d3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(d3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hResult = S_OK;

	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL,
			nd3dDriverType, NULL, dwCreateDeviceFlags, d3dFeatureLevels, nFeatureLevels,
			D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain,
			&m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext)))
			break;
	}
	if (!m_pDXGISwapChain || !m_pd3dDevice || !m_pd3dDeviceContext)
		return(false);

	if (!CreateRenderTargetView())
		return(false);

	return(true);
}

void CGameFramework::BuildObjects()
{
	m_pScene = new CScene();

	// 플레이어 객체를 생성한다.
	m_pPlayer = new CPlayer();

	// 카메라 객체를 생성하고 뷰-포트를 설정한다.
	CCamera *pCamera = new CCamera();
	pCamera->CreateShaderVariables(m_pd3dDevice);
	pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth,
		m_nWndClientHeight, 0.0f, 1.0f);
	// 투영 변환 행렬을 생성한다.
	pCamera->GenerateProjectionMatrix(1.0f, 500.0f,
		float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
	// 카메라 변환 행렬을 생성한다.
	XMVECTOR d3dxvEyePosition = XMLoadFloat3(&XMFLOAT3(0.0f, 15.0f, -35.0f));
	XMVECTOR d3dxvLookAt = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
	XMVECTOR d3dxvUp = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
	pCamera->GenerateViewMatrix(d3dxvEyePosition, d3dxvLookAt, d3dxvUp);
	// 카메라 객체를 플레이어 객체에 설정한다.
	m_pPlayer->SetCamera(pCamera);
	m_pPlayer->CreateShaderVariables(m_pd3dDevice);

	if (m_pScene)
		m_pScene->BuildObjects(m_pd3dDevice);
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
	if (m_pPlayer) delete m_pPlayer;
}

void CGameFramework::ProcessInput()
{
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene)
		m_pScene->AnimateObjects(m_Timer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	m_Timer.Tick();
	ProcessInput();
	AnimateObjects();

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView,
		fClearColor);
	
	if (m_pPlayer) m_pPlayer->UpdateShaderVariables(m_pd3dDeviceContext);
	CCamera *pCamera = (m_pPlayer) ? m_pPlayer->GetCamera() : NULL;
	if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, pCamera);
	
	m_pDXGISwapChain->Present(0, 0);
	
	m_Timer.GetFrameRate(m_pszBuffer + 12, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);

		m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

		if (m_pd3dRenderTargetView)
			m_pd3dRenderTargetView->Release();

		m_pDXGISwapChain->ResizeBuffers(2, m_nWndClientWidth,
			m_nWndClientHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

		CreateRenderTargetView();

		CCamera *pCamera = m_pPlayer->GetCamera();
		if (pCamera)
			pCamera->SetViewport(m_pd3dDeviceContext, 0, 0,
				m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);

		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_pd3dDeviceContext)
		m_pd3dDeviceContext->ClearState();
	if (m_pd3dRenderTargetView)
		m_pd3dRenderTargetView->Release();
	if (m_pDXGISwapChain)
		m_pDXGISwapChain->Release();
	if (m_pd3dDeviceContext)
		m_pd3dDeviceContext->Release();
	if (m_pd3dDevice)
		m_pd3dDevice->Release(); 
}