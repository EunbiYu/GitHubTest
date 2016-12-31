#pragma once
#include "Player.h"
#include "Timer.h"
#include "Scene.h"

class CGameFramework
{
private:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	ID3D11Device *m_pd3dDevice;
	IDXGISwapChain *m_pDXGISwapChain;
	ID3D11DeviceContext *m_pd3dDeviceContext;
	ID3D11RenderTargetView *m_pd3dRenderTargetView;

	CTimer m_Timer;
	CScene *m_pScene;
	_TCHAR m_pszBuffer[50];

	CPlayer *m_pPlayer;

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	bool CreateRenderTargetView();
	bool CreateDirect3DDisplay();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);
};

