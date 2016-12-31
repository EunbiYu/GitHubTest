#include "stdafx.h"
#include "Shader.h"

CShader::CShader()
{
	m_pd3dVertexShader = NULL;
	m_pd3dPixelShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pd3dcbWorldMatrix = NULL;

	m_nReferences = 0;
}


CShader::~CShader()
{
	if (m_pd3dVertexShader)
		m_pd3dVertexShader->Release();
	if (m_pd3dPixelShader)
		m_pd3dPixelShader->Release();
	if (m_pd3dVertexLayout)
		m_pd3dVertexLayout->Release();
	
	ReleaseShaderVariables();
}

void CShader::CreateVertexShaderFromFile(ID3D11Device * pd3dDevice, WCHAR * pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader ** ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC * pd3dInputLayout, UINT nElements, ID3D11InputLayout ** ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;

	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName,
		NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0,
		NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreateVertexShader(pd3dShaderBlob->GetBufferPointer(),
			pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements,
			pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(),
			ppd3dVertexLayout);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreatePixelShaderFromFile(ID3D11Device * pd3dDevice, WCHAR * pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader ** ppd3dPixelShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;

	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName,
		NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0,
		NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreatePixelShader(pd3dShaderBlob->GetBufferPointer(),
			pd3dShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader);
		pd3dShaderBlob->Release();
	}

}

void CShader::CreateShader(ID3D11Device * pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	UINT nElements = ARRAYSIZE(d3dInputLayout);

	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VS", "vs_5_0",
		&m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);

	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PS", "ps_5_0",
		&m_pd3dPixelShader);

	CreateShaderVariables(pd3dDevice);

}

void CShader::Render(ID3D11DeviceContext * pd3dDeviceContext)
{
	if (m_pd3dVertexLayout)
		pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	if (m_pd3dVertexShader)
		pd3dDeviceContext->VSSetShader(m_pd3dVertexShader,
			NULL, 0);
	if (m_pd3dPixelShader)
		pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}

void CShader::CreateShaderVariables(ID3D11Device * pd3dDevice)
{
	// 월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
}

void CShader::ReleaseShaderVariables()
{
	// 월드 변환 행렬을 위한 상수 버퍼를 반환한다.
	if (m_pd3dcbWorldMatrix)
		m_pd3dcbWorldMatrix->Release();
}

void CShader::UpdateShaderVariable(ID3D11DeviceContext * pd3dDeviceContext, D3DXMATRIX * pd3dxmtxWorld)
{
	// 월드 변환 행렬을 상수 버퍼에 복사한다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0,
		&d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX*)
		d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	// 상수 버퍼를 디바이스 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1,
		&m_pd3dcbWorldMatrix);
}
