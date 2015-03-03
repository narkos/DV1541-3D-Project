#include "main.h"

int arnold = 5;

namespace
{
	Main*pMain; // pekare till applikationen
}

LRESULT CALLBACK Main::CallWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return pMain->WndProc(hWnd, message, wParam, lParam);
}


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


void Main::CreateShaders()
{

	//------------VertexShader-----------------------------------------------------------------------------------------------------------
	ID3DBlob* pVS = nullptr;
	CompileShader(L"VertexShader.hlsl", "VS_main", "vs_5_0", &pVS);
	
	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);
	
	
	//------------Create input layout-----------------------------------------------------------------------------------------------
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }

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


void Main::CreateBuffers()
{

	struct TriangleVertex
	{
		float x, y, z;
		float r, g, b;
		float Nx, Ny, Nz;
	}
	triangleVertices[4] =
	{
		-0.5f, -0.5f, 0.0f,	//v0 pos
		0.0f, 1.0f, 0.0f, 	//v0 Color
		0.0f, 0.0f, -1.0f,


		-0.5f, 0.5f, 0.0f,	//v1
		0.0f, 0.0f, 1.0f, 	//v1 
		0.0f, 0.0f, -1.0f,


		0.5f, -0.5f, 0.0f, //v2
		1.0f, 1.0f, 0.0f,	//
		0.0f, 0.0f, -1.0f,


		0.5f, 0.5f, 0.0f, //v3
		1.0f, 0.0f, 0.0f,	
		0.0f, 0.0f, -1.0f



	};

	D3D11_BUFFER_DESC vBufferDesc;
	memset(&vBufferDesc, 0, sizeof(vBufferDesc));
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	gDevice->CreateBuffer(&vBufferDesc, &data, &gVertexBuffer);

}


void Main::SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Height = (float)480;
	vp.Width = (float)640;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}

void Main::Render()
{
	float clearColor[] = { 0, 0, 0, 1 };
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Render Quad
	UINT32 vertexSize = sizeof(float) * 9;
	UINT32 offset = 0;
	gDeviceContext->IASetInputLayout(gVertexLayout);
	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Set Shaders and texture
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
	gDeviceContext->PSSetShaderResources(0, 1, &gTextureView);

	//Draw Object with 4 vertices
	gDeviceContext->Draw(4, 0);

}



int Main::wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);

	if (wndHandle)
	{
		CreateDirect3DContext(wndHandle);

		SetViewport();

		CreateShaders();

		CreateBuffers();

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
				Render(); //Rendera
				gSwapChain->Present(0, 0); //Växla front- och back-buffer
			}
		}

		gVertexBuffer->Release();
		//gConstantBuffer->Release();
		gVertexLayout->Release();
		gVertexShader->Release();
		gPixelShader->Release();

		gBackbufferRTV->Release();
		gSwapChain->Release();
		gDevice->Release();
		gDeviceContext->Release();

		gTextureView->Release();
		gDepthStencilBuffer->Release();
		gDepthStencilView->Release();


		DestroyWindow(wndHandle);
	}

	return (int)msg.wParam;
}

HWND Main::InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CallWndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"DirectX 3D Projekt";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"DirectX 3D Projekt",
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

LRESULT Main::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}




HRESULT Main::CreateDirect3DContext(HWND wndHandle)
{
	//create a struct to hold inforamtion about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	//Clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;									// windowed/full-screen mode


	//create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr)){

		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();
		
		//DepthBuffer
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
		depthStencilDesc.Width = 640;
		depthStencilDesc.Height = 480;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		HRESULT hr1 = gDevice->CreateTexture2D(&depthStencilDesc, NULL, &gDepthStencilBuffer);
		HRESULT hr2 = gDevice->CreateDepthStencilView(gDepthStencilBuffer, NULL, &gDepthStencilView);



		// set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencilView);
	}
	return hr;

}

