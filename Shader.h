#pragma once
struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX m_d3dxmtxWorld;
};

class CShader
{
private:
	int m_nReferences;

public:
	CShader();
	virtual ~CShader();

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	ID3D11PixelShader *m_pd3dPixelShader;

	// 월드 변환 행렬을 위한 상수 버퍼에 대한 인터페이스 포인터를 선언한다.
	ID3D11Buffer *m_pd3dcbWorldMatrix;

public:
	void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR
		*pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel,
		ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout,
		UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);

	void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName,
		LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	// 쉐이더 클래스의 상수 버퍼를 생성하고 반환하는 멤버 함수를 선언한다.
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	// 쉐이더 클래스의 상수 버퍼를 갱신하는 멤버 함수를 선언한다.
	virtual void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext,
		D3DXMATRIX *pd3dxmtxWorld);
	
};

