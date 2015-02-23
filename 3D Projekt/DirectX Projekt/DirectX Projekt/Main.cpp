#include <windows.h>
#include <stdio.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
#include <Windows.h>
#include <string.h>
#include <memory.h>

using namespace DirectX;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT CreateDirect3DContext(HWND wndHandle);

ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;

ID3D11InputLayout* gVertexLayout = nullptr;

ID3D11Buffer* VertexBuffer = nullptr;

ID3D11VertexShader* gVertexShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;
ID3D11GeometryShader* gGeometryShader = nullptr;




HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}


void CreateShaders()
{
	//------------VertexShader-----------------------------------------------------------------------------------------------------------
	ID3DBlob* pVS = nullptr;
	CompileShader(L"VertexShader.hlsl", "VS_main", "vs_5_0", &pVS);
	
	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);
	
	
	//Create input layout
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
	pVS->Release();
	//--------------PixelShader----------------------------------------------------------------------------------------------------------------
	ID3DBlob* pPS = nullptr;
	CompileShader(L"PixelShader.hlsl", "PS_main", "ps_5_0", &pPS);
	
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
	pPS->Release();

	//----------------GeometryShader-------------------------------------------------------------------------------------------------------------
	ID3DBlob* pGS = nullptr;
	CompileShader(L"GeometryShader.hlsl", "GS_main", "gs_5_0", &pGS);
		
	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);
	pGS->Release();
	
}

//--------------------------Vertex Buffer--------------------------------------
D3D11_BUFFER_DESC vBufferDesc;
memset(&vBufferDesc, 0, sizeof(vBufferDesc));
vBufferDesc.BíndFlags = D3D11_BIND_VERTEX_BUFFER;
vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
vBufferDesc.ByteWidth = sizeof();




void SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Height = (float)480;
	vp.Width = (float)640;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDevice->RSSetViewports(1, &vp);
}



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);

	if (wndHandle)
	{
		ShowWindow(wndHandle, nCmdShow);

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// update/render goes here
			}
		}

		DestroyWindow(wndHandle);
	}

	return (int)msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BasicWindow";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BasicWindow",
		L"DirectX 3D Projekt",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}