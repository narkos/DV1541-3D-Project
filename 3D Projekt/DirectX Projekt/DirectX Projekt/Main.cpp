#include <windows.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
#include <string.h>
#include "GameTime.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
#include <vector>



using namespace DirectX;
using namespace std;


#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")


HRESULT CreateDirect3DContext(HWND wndHandle);
HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


IDXGISwapChain* gSwapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
ID3D11RenderTargetView* gBackbufferRTV = nullptr;
ID3D11ShaderResourceView* gTextureView = nullptr;

ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11DepthStencilView* gDepthStencilView = nullptr;
ID3D11Texture2D* gDepthStencilBuffer = nullptr;

ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11Buffer* gConstantBufferCamera = nullptr;

ID3D11VertexShader* gVertexShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;
ID3D11GeometryShader* gGeometryShader = nullptr;

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

GameTimer mTimer;
std::wstring mMainWndCaption;
HWND handle;


//----------------------Camera------------------------------------------------------
XMMATRIX WVP;
XMMATRIX world;
XMMATRIX camView;
XMMATRIX camProjection;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;


XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;


float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;
//-------------------------------------------------------------------------------------

HRESULT hr;

//---------------------------Sphere------------------------------------------------------
ID3D11Buffer* sphereIndexBuffer;
ID3D11Buffer* sphereVertBuffer;

ID3D11VertexShader* skyMapVertexShader;
ID3D11PixelShader* skyMapPixelShader;

ID3D11ShaderResourceView* smrv;

ID3D11DepthStencilState* DSLessEqual;
ID3D11RasterizerState* RSCullNone;

int NumSphereVertices;
int NumSphereFaces;

XMMATRIX sphereWorld;
//---------------------------------------------------------------------------------------------

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



//----------------------------------PixelShader and VertexShader for Skymap-------------------------------------------------------
	ID3D10Blob* pSkyPs = nullptr;
	CompileShader(L"SkymapPixelShader.hlsl", "skymapPixelShader_main", "pSkyPs_5_0", &pSkyPs);

	gDevice->CreatePixelShader(pSkyPs->GetBufferPointer(), pSkyPs->GetBufferSize(), nullptr, &skyMapPixelShader);
	pSkyPs->Release();

	ID3D10Blob* vSkyVs = nullptr;
	CompileShader(L"SkymapVertexShader.hlsl", "skymapVertexShader_main", "vSkyVs_5_0", &vSkyVs);

	gDevice->CreateVertexShader(vSkyVs->GetBufferPointer(), vSkyVs->GetBufferSize(), nullptr, &skyMapVertexShader);
	vSkyVs->Release();
//---------------------------------------------------------------------------------------------------------------------------------	
}

 struct MatrixBuffer
{
	XMMATRIX  WVP;
};

 MatrixBuffer cbPerObj;


 
 bool InitDirectInput(HINSTANCE hInstance)
{
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(handle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}
 
 
void UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	//First-Person Camera
	/* XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camPitch);

	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);
	*/

	//Free-Look Camera
	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;


	camTarget = camPosition + camTarget;

	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
 
 
}

void DetectInput()
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	//Exit program when escape is pushed down
	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(handle, WM_DESTROY, 0, 0);

	float speed = 0.001f;

	if (keyboardState[DIK_A] & 0x80 || keyboardState[DIK_LEFT] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if (keyboardState[DIK_D] & 0x80 || keyboardState[DIK_RIGHT] & 0x80)
	{
		moveLeftRight += speed;
	}
	if (keyboardState[DIK_W] & 0x80 || keyboardState[DIK_UP] & 0x80)
	{
		moveBackForward += speed;
	}
	if (keyboardState[DIK_S] & 0x80 || keyboardState[DIK_DOWN] & 0x80)
	{
		moveBackForward -= speed;
	}
	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.001f;

		camPitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}


	return;
}

void FpsCounter()
{
	// Code computes the avarage frame time and frames per second
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	//Function is beeing initialiased every time a frame is made
	frameCount++;

	// Compute averages over one second period
	if (((mTimer.TotalTime()) - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCount / 1; // fps = framecount / 1
		float mspf = 1000.0f / fps;
		
		// Makes a String for the window handler
		std::wostringstream outs;

		outs.precision(6);
		outs << mMainWndCaption << L" "
			<< L"        FPS: " << fps << L" "
			<< L"        Frame Time: " << mspf << L" (ms)";
			
		//Prints the text in the window handler
		SetWindowText(handle, outs.str().c_str());

		// Reset for next fps.
		frameCount = 0;
		timeElapsed += 0.25f;

	}

}


void CreateSphere(int LatLines, int LongLines)
{
	//NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	//NumSphereFaces = ((LatLines - 3)*(LongLines)* 2) + (LongLines * 2);

	//float sphereYaw = 0.0f;
	//float spherePitch = 0.0f;

	//std::vector<Vertex> vertices(NumSphereVertices);

	//XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	//vertices[0].pos.x = 0.0f;
	//vertices[0].pos.y = 0.0f;
	//vertices[0].pos.z = 1.0f;

	//for (DWORD i = 0; i < LatLines - 2; ++i)
	//{
	//	spherePitch = (i + 1) * (3.14 / (LatLines - 1));
	//	Rotationx = XMMatrixRotationX(spherePitch);
	//	for (DWORD j = 0; j < LongLines; ++j)
	//	{
	//		sphereYaw = j * (6.28 / (LongLines));
	//		Rotationy = XMMatrixRotationZ(sphereYaw);
	//		currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
	//		currVertPos = XMVector3Normalize(currVertPos);
	//		vertices[i*LongLines + j + 1].pos.x = XMVectorGetX(currVertPos);
	//		vertices[i*LongLines + j + 1].pos.y = XMVectorGetY(currVertPos);
	//		vertices[i*LongLines + j + 1].pos.z = XMVectorGetZ(currVertPos);
	//	}
	//}

	//vertices[NumSphereVertices - 1].pos.x = 0.0f;
	//vertices[NumSphereVertices - 1].pos.y = 0.0f;
	//vertices[NumSphereVertices - 1].pos.z = -1.0f;


	//D3D11_BUFFER_DESC vertexBufferDesc;
	//ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	//vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//vertexBufferDesc.ByteWidth = sizeof(Vertex) * NumSphereVertices;
	//vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vertexBufferDesc.CPUAccessFlags = 0;
	//vertexBufferDesc.MiscFlags = 0;

	//D3D11_SUBRESOURCE_DATA vertexBufferData;

	//ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	//vertexBufferData.pSysMem = &vertices[0];
	//hr = gDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &sphereVertBuffer);


	//std::vector<DWORD> indices(NumSphereFaces * 3);

	//int k = 0;
	//for (DWORD l = 0; l < LongLines - 1; ++l)
	//{
	//	indices[k] = 0;
	//	indices[k + 1] = l + 1;
	//	indices[k + 2] = l + 2;
	//	k += 3;
	//}

	//indices[k] = 0;
	//indices[k + 1] = LongLines;
	//indices[k + 2] = 1;
	//k += 3;

	//for (DWORD i = 0; i < LatLines - 3; ++i)
	//{
	//	for (DWORD j = 0; j < LongLines - 1; ++j)
	//	{
	//		indices[k] = i*LongLines + j + 1;
	//		indices[k + 1] = i*LongLines + j + 2;
	//		indices[k + 2] = (i + 1)*LongLines + j + 1;

	//		indices[k + 3] = (i + 1)*LongLines + j + 1;
	//		indices[k + 4] = i*LongLines + j + 2;
	//		indices[k + 5] = (i + 1)*LongLines + j + 2;

	//		k += 6; // next quad
	//	}

	//	indices[k] = (i*LongLines) + LongLines;
	//	indices[k + 1] = (i*LongLines) + 1;
	//	indices[k + 2] = ((i + 1)*LongLines) + LongLines;

	//	indices[k + 3] = ((i + 1)*LongLines) + LongLines;
	//	indices[k + 4] = (i*LongLines) + 1;
	//	indices[k + 5] = ((i + 1)*LongLines) + 1;

	//	k += 6;
	//}

	//for (DWORD l = 0; l < LongLines - 1; ++l)
	//{
	//	indices[k] = NumSphereVertices - 1;
	//	indices[k + 1] = (NumSphereVertices - 1) - (l + 1);
	//	indices[k + 2] = (NumSphereVertices - 1) - (l + 2);
	//	k += 3;
	//}

	//indices[k] = NumSphereVertices - 1;
	//indices[k + 1] = (NumSphereVertices - 1) - LongLines;
	//indices[k + 2] = NumSphereVertices - 2;

	//D3D11_BUFFER_DESC indexBufferDesc;
	//ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	//indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//indexBufferDesc.ByteWidth = sizeof(DWORD) * NumSphereFaces * 3;
	//indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//indexBufferDesc.CPUAccessFlags = 0;
	//indexBufferDesc.MiscFlags = 0;

	//D3D11_SUBRESOURCE_DATA iinitData;

	//iinitData.pSysMem = &indices[0];
	//gDevice->CreateBuffer(&indexBufferDesc, &iinitData, &sphereIndexBuffer);

}

void CreateBuffers()
{

	struct TriangleVertex
	{
		float x, y, z;
		float r, g, b;
		float Nx, Ny, Nz;
	}
	triangleVertices[4] =
	{
		-1.0f, -1.0f, 0.0f,	//v0 pos
		0.0f, 1.0f, 0.0f, 	//v0 Color
		0.0f, 0.0f, -1.0f,


		-1.0, 1.0f, 0.0f,	//v1
		0.0f, 0.0f, 1.0f, 	//v1 
		0.0f, 0.0f, -1.0f,


		1.0f, -1.0f, 0.0f, //v2
		1.0f, 1.0f, 0.0f,	//
		0.0f, 0.0f, -1.0f,


		1.0f, 1.0f, 0.0f, //v3
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


	D3D11_BUFFER_DESC cBufferDesc;
	memset(&cBufferDesc, 0, sizeof(cBufferDesc));
	
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferDesc.ByteWidth = sizeof(MatrixBuffer);

	gDevice->CreateBuffer(&cBufferDesc, NULL, &gConstantBufferCamera);

	

}



void SetViewport()
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

void Render()
{
	world = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	
	DetectInput();
	UpdateCamera();

	float clearColor[] = { 0, 0, 0, 1 };
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Camera Infomartion
	/*camPosition = XMVectorSet(0.0f, 0.0f, 0.5f, 0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);*/

	//Returns a blank matrix
	
	//Set the view/projection matrix
	//world = XMMatrixRotationY();
	//camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
	camProjection = XMMatrixPerspectiveFovLH((3.14f*(0.4f)), (640.0f / 480.0f), 0.5f, 1000.0f);

	
	//Set the World/View/Projection matrix, then send it to constant buffer in effect file
	WVP = XMMatrixMultiply(world, XMMatrixMultiply(camView, camProjection));
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	
	gDeviceContext->UpdateSubresource(gConstantBufferCamera, 0, NULL, &cbPerObj, 0, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBufferCamera);
	//gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBufferCamera);

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
	//gDeviceContext->PSSetShaderResources(0, 1, &gTextureView);

	//Draw Object with 4 vertices
	gDeviceContext->Draw(4, 0);




}


int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);

	if (wndHandle)
	{
		CreateDirect3DContext(wndHandle);

		FpsCounter();

		SetViewport();

		CreateShaders();

		CreateBuffers();
		
		InitDirectInput(hInstance);
		
		ShowWindow(wndHandle, nCmdShow);
		
		mTimer.Reset();

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				
				mTimer.Tick();
				FpsCounter();
				Render(); //Rendera
				gSwapChain->Present(0, 0); //Växla front- och back-buffer
				
			}
		}

		
		gVertexBuffer->Release();
		gConstantBufferCamera->Release();
		gVertexLayout->Release();
		gVertexShader->Release();
		gPixelShader->Release();

		gBackbufferRTV->Release();
		gSwapChain->Release();
		gDevice->Release();
		gDeviceContext->Release();

		//gTextureView->Release();
		gDepthStencilBuffer->Release();
		gDepthStencilView->Release();
		
		DIKeyboard->Unacquire();
		DIMouse->Unacquire();
		DirectInput->Release();

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
	wcex.lpszClassName = L"DirectX 3D Projekt";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	mMainWndCaption = L"Direct3D Projekt";

		handle = CreateWindow(
		L"DirectX 3D Projekt",
		mMainWndCaption.c_str(),
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

HRESULT CreateDirect3DContext(HWND wndHandle)
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

