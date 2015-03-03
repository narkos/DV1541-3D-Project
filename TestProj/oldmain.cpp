////--------------------------------------------------------------------------------------
//// BTH - Stefan Petersson 2014.
////--------------------------------------------------------------------------------------
//#define PI (3.141592653589793)
//#include <windows.h>
//
//#include <d3d11.h>
//#include <d3dcompiler.h>
//#include <DirectXMath.h>
//#include <DirectXMathMatrix.inl>
//
//#include "bth_image.h"
//
//#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dcompiler.lib")
//
//using namespace DirectX;
//
//HWND InitWindow(HINSTANCE hInstance);
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//HRESULT CreateDirect3DContext(HWND wndHandle);
//
//IDXGISwapChain* gSwapChain = nullptr;
//ID3D11Device* gDevice = nullptr;
//ID3D11DeviceContext* gDeviceContext = nullptr;
//ID3D11RenderTargetView* gBackbufferRTV = nullptr;
//ID3D11ShaderResourceView* gTextureView = nullptr;
//ID3D11DepthStencilView* gDepthStencilView = nullptr;
//ID3D11Buffer* gVertexBuffer = nullptr;
//ID3D11InputLayout* gVertexLayout = nullptr;
//ID3D11VertexShader* gVertexShader = nullptr;
//ID3D11PixelShader* gPixelShader = nullptr;
//ID3D11GeometryShader* gGeometryShader = nullptr;
//
//ID3D11Buffer* gConstantBuffer = nullptr;
//
//
//XMMATRIX WVPMatrix;
//XMMATRIX worldMatrix;
//XMMATRIX viewMatrix;
//XMMATRIX projMatrix;
//XMMATRIX wMatrix;
//
//XMVECTOR camPosVector;
//XMVECTOR camTargetVector;
//XMVECTOR camUpVector;
//
//struct constantBuff
//{
//	XMMATRIX WVPMatrix;
//};
//
//constantBuff cbInstance;
//
//HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
//{
//	if (!srcFile || !entryPoint || !profile || !blob)
//		return E_INVALIDARG;
//
//	*blob = nullptr;
//
//	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
//#if defined( DEBUG ) || defined( _DEBUG )
//	flags |= D3DCOMPILE_DEBUG;
//#endif
//
//	const D3D_SHADER_MACRO defines[] =
//	{
//		"EXAMPLE_DEFINE", "1",
//		NULL, NULL
//	};
//
//	ID3DBlob* shaderBlob = nullptr;
//	ID3DBlob* errorBlob = nullptr;
//	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		entryPoint, profile,
//		flags, 0, &shaderBlob, &errorBlob);
//	if (FAILED(hr))
//	{
//		if (errorBlob)
//		{
//			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
//			errorBlob->Release();
//		}
//
//		if (shaderBlob)
//			shaderBlob->Release();
//
//		return hr;
//	}
//
//	*blob = shaderBlob;
//
//	return hr;
//}
//
//
//void CreateShaders()
//{
//
//	//create vertex shader
//	ID3DBlob* pVS = nullptr;
//	CompileShader(L"VertexShader.hlsl", "VS_main", "vs_5_0", &pVS);
//	/*D3DCompile(vertex_shader, strlen(vertex_shader), NULL, nullptr,
//	NULL, "VS_main", "vs_5_0", 0, NULL, &pVS, nullptr);*/
//
//	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);
//
//	//create input layout (verified using vertex shader)
//	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
//	pVS->Release();
//
//	//create geometry shader
//	ID3DBlob* pGS = nullptr;
//	CompileShader(L"GeometryShader.hlsl", "GS_main", "gs_5_0", &pGS);
//	/*D3DCompile(geometry_shader, strlen(geometry_shader), NULL, nullptr,
//	NULL, "GS_main", "gs_5_0", 0, NULL, &pGS, nullptr);*/
//	/*HRESULT hr =*/ gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);
//	pGS->Release();
//
//	//create pixel shader
//	ID3DBlob* pPS = nullptr;
//	CompileShader(L"PixelShader.hlsl", "PS_main", "ps_5_0", &pPS);
//	/*D3DCompile(pixel_shader, strlen(pixel_shader), NULL, nullptr,
//	NULL, "PS_main", "ps_5_0", 0, NULL, &pPS, nullptr);*/
//
//	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
//	pPS->Release();
//
//	// Texture Description Creation
//	D3D11_TEXTURE2D_DESC textureDesc;
//	ZeroMemory(&textureDesc, sizeof(textureDesc));
//	textureDesc.Width = BTH_IMAGE_WIDTH;
//	textureDesc.Height = BTH_IMAGE_HEIGHT;
//	textureDesc.MipLevels = textureDesc.ArraySize = 1;
//	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	textureDesc.SampleDesc.Count = 1;
//	textureDesc.SampleDesc.Quality = 0;
//	textureDesc.Usage = D3D11_USAGE_DEFAULT;
//	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//	textureDesc.MiscFlags = 0;
//	textureDesc.CPUAccessFlags = 0;
//
//	// Raw data Texture Creation
//	ID3D11Texture2D *pTexture = NULL;
//	D3D11_SUBRESOURCE_DATA textureData;
//	ZeroMemory(&textureData, sizeof(textureData));
//	textureData.pSysMem = (void*)BTH_IMAGE_DATA;
//	textureData.SysMemPitch = BTH_IMAGE_WIDTH * 4 * sizeof(char);
//	HRESULT hr = gDevice->CreateTexture2D(&textureDesc, &textureData, &pTexture);
//
//	// Resource View Description
//	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
//	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
//	resViewDesc.Format = textureDesc.Format;
//	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	resViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
//	resViewDesc.Texture2D.MostDetailedMip = 0;
//	hr = gDevice->CreateShaderResourceView(pTexture, &resViewDesc, &gTextureView);
//
//	pTexture->Release();
//
//}
//
//
//
//void CreateTriangleData()
//{
//	//struct TriangleVertex
//	//{
//	//	float x, y, z;
//	//	float u, v;
//	//	float nX, nY, nZ;
//	//}
//	//triangleVertices[4] =
//	//{
//	//	-0.5f, -0.5f, 0.0f,	//v0 pos
//	//	0.0f, 1.0f, 	//v0 color
//	//	0.0f, 0.0f, 1.0f,
//
//	//	-0.5f, 0.5f, 0.0f,	//v1
//	//	0.0f, 0.0f,	//v1 color
//	//	0.0f, 0.0f, 1.0f,
//
//	//	0.5f, -0.5f, 0.0f,	//v2
//	//	1.0f, 1.0f, 	//v2 color
//	//	0.0f, 0.0f, 1.0f,
//
//	//	0.5f, 0.5f, 0.0f,	//v3
//	//	1.0f, 0.0f,	//v3 color
//	//	0.0f, 0.0f, 1.0f
//	//};
//
//	struct TriangleVertex
//	{
//		float x, y, z;
//		float u, v;
//
//	}
//	triangleVertices[4] =
//	{
//		-0.5f, -0.5f, 0.0f,	//v0 pos
//		0.0f, 1.0f, 	//v0 color
//
//
//		-0.5f, 0.5f, 0.0f,	//v1
//		0.0f, 0.0f,	//v1 color
//
//
//		0.5f, -0.5f, 0.0f,	//v2
//		1.0f, 1.0f, 	//v2 color
//
//
//		0.5f, 0.5f, 0.0f,	//v3
//		1.0f, 0.0f	//v3 color
//
//	};
//
//	D3D11_BUFFER_DESC bufferDesc;
//	memset(&bufferDesc, 0, sizeof(bufferDesc));
//	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	bufferDesc.ByteWidth = sizeof(triangleVertices);
//
//	D3D11_SUBRESOURCE_DATA data;
//	data.pSysMem = triangleVertices;
//	gDevice->CreateBuffer(&bufferDesc, &data, &gVertexBuffer);
//
//	// Create ConstantBuffer
//	D3D11_BUFFER_DESC cbDesc;
//	ZeroMemory(&cbDesc, sizeof(cbDesc));
//	cbDesc.Usage = D3D11_USAGE_DEFAULT;
//	cbDesc.ByteWidth = sizeof(constantBuff);
//	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbDesc.CPUAccessFlags = 0;
//	cbDesc.MiscFlags = 0;
//
//	HRESULT hr = gDevice->CreateBuffer(&cbDesc, NULL, &gConstantBuffer);
//
//
//
//}
//
//void SetViewport()
//{
//	D3D11_VIEWPORT vp;
//	vp.Width = (float)640;
//	vp.Height = (float)480;
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//	gDeviceContext->RSSetViewports(1, &vp);
//}
//
//void Render()
//{
//	static float yRotation = 0.01f;
//	yRotation += 0.001f;
//	// clear the back buffer to a deep blue
//	float clearColor[] = { 0, 0, 0, 1 };
//	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
//	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
//	UINT32 vertexSize = sizeof(float) * 8;  // !!!!!!!!!! 5 !!!!!!!!!!!!!
//	UINT32 offset = 0;
//
//
//	//Camera Information
//	camPosVector = XMVectorSet(0.0f, 0.0f, -2.0f, 1.0f);
//	camTargetVector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//	camUpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
//	viewMatrix = XMMatrixLookAtLH(camPosVector, camTargetVector, camUpVector);
//
//	//Projection Space
//	projMatrix = XMMatrixPerspectiveFovLH(0.4f*PI, (640.0f / 480.0f), 0.5f, 20.0f);
//
//	// ConstantBuffer and Worl Matrix settings
//	worldMatrix = XMMatrixIdentity(); // Resets the worldmatrix
//	worldMatrix = XMMatrixRotationY(yRotation);
//	WVPMatrix = worldMatrix*viewMatrix*projMatrix;
//	//WVPMatrix = mul(mul(worldMatrix, viewMatrix), projMatrix);
//
//	//wMatrix = XMMatrixInverse(nullptr, worldMatrix);
//
//
//	cbInstance.WVPMatrix = XMMatrixTranspose(WVPMatrix);
//	gDeviceContext->UpdateSubresource(gConstantBuffer, 0, NULL, &cbInstance, 0, 0);
//	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
//
//
//
//
//	//Shaders
//	gDeviceContext->IASetInputLayout(gVertexLayout);
//	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
//	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
//	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
//	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
//	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
//	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
//
//	gDeviceContext->PSSetShaderResources(0, 1, &gTextureView);
//	gDeviceContext->Draw(4, 0);
//}
//
//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
//{
//	MSG msg = { 0 };
//	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster
//
//	if (wndHandle)
//	{
//		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context
//
//		SetViewport(); //3. Sätt viewport
//
//		CreateShaders(); //4. Skapa vertex- och pixel-shaders
//
//		CreateTriangleData(); //5. Definiera triangelvertiser, 6. Skapa vertex buffer, 7. Skapa input layout
//
//		ShowWindow(wndHandle, nCmdShow);
//
//		while (WM_QUIT != msg.message)
//		{
//			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//			{
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//			}
//			else
//			{
//				Render(); //8. Rendera
//
//				gSwapChain->Present(0, 0); //9. Växla front- och back-buffer
//			}
//		}
//
//		gVertexBuffer->Release();
//		gVertexLayout->Release();
//		gVertexShader->Release();
//		gPixelShader->Release();
//		gGeometryShader->Release();
//		gBackbufferRTV->Release();
//		gSwapChain->Release();
//		gDevice->Release();
//		gDeviceContext->Release();
//		DestroyWindow(wndHandle);
//		gConstantBuffer->Release();
//	}
//
//	return (int)msg.wParam;
//}
//
//HWND InitWindow(HINSTANCE hInstance)
//{
//	WNDCLASSEX wcex = { 0 };
//	wcex.cbSize = sizeof(WNDCLASSEX);
//	wcex.style = CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc = WndProc;
//	wcex.hInstance = hInstance;
//	wcex.lpszClassName = L"BTH_D3D_DEMO";
//	if (!RegisterClassEx(&wcex))
//		return false;
//
//	RECT rc = { 0, 0, 640, 480 };
//	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
//
//	HWND handle = CreateWindow(
//		L"BTH_D3D_DEMO",
//		L"BTH Direct3D Demo",
//		WS_OVERLAPPEDWINDOW,
//		CW_USEDEFAULT,
//		CW_USEDEFAULT,
//		rc.right - rc.left,
//		rc.bottom - rc.top,
//		nullptr,
//		nullptr,
//		hInstance,
//		nullptr);
//
//	return handle;
//}
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch (message)
//	{
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//	}
//
//	return DefWindowProc(hWnd, message, wParam, lParam);
//}
//
//HRESULT CreateDirect3DContext(HWND wndHandle)
//{
//	// create a struct to hold information about the swap chain
//	DXGI_SWAP_CHAIN_DESC scd;
//
//	// clear out the struct for use
//	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
//
//	// fill the swap chain description struct
//	scd.BufferCount = 1;                                    // one back buffer
//	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
//	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
//	scd.OutputWindow = wndHandle;                           // the window to be used
//	scd.SampleDesc.Count = 4;                               // how many multisamples
//	scd.Windowed = TRUE;                                    // windowed/full-screen mode
//
//	// create a device, device context and swap chain using the information in the scd struct
//	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
//		D3D_DRIVER_TYPE_HARDWARE,
//		NULL,
//		NULL,
//		NULL,
//		NULL,
//		D3D11_SDK_VERSION,
//		&scd,
//		&gSwapChain,
//		&gDevice,
//		NULL,
//		&gDeviceContext);
//
//	if (SUCCEEDED(hr))
//	{
//		// get the address of the back buffer
//		ID3D11Texture2D* pBackBuffer = nullptr;
//		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
//
//		// use the back buffer address to create the render target
//		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
//		pBackBuffer->Release();
//
//		// set the render target as the back buffer
//		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, NULL);
//	}
//	return hr;
//}