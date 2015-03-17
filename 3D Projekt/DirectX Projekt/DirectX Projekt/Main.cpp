
#include "main.h"
 
namespace { Main* mainName = 0; }

LRESULT CALLBACK CallWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return mainName->WndProc(hWnd, message, wParam, lParam);
}

HRESULT Main::CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
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
	Main::CompileShader(L"VertexShader.hlsl", "VS_main", "vs_5_0", &pVS);
	
	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);
	
	
	//------------Create input layout-----------------------------------------------------------------------------------------------
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

bool Main::InitDirectInput(HINSTANCE hInstance)
{
	HRESULT hr;
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
 
 
void Main::UpdateCamera()
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

void Main::DetectInput()
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



void Main::FpsCounter()
{
	

	// Code computes the avarage frame time and frames per second

	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	//Function is beeing initialiased every time a frame is made
	frameCount++;

	// Compute averages over one second period
	if (((mTimer.TotalTime()) - timeElapsed) >= 0.50f)
	{
		float fps = (float)frameCount; // fps = framecount / 1
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
		-0.5f, -0.5f, 5.0f,	//v0 pos
		0.0f, 1.0f, 	//v0 Color
		0.0f, 0.0f, -1.0f,


		-0.5f, 0.5f, 5.0f,	//v1
		0.0f, 0.0f, 	//v1 
		0.0f, 0.0f, -1.0f,


		0.5f, -0.5f, 5.0f, //v2
		1.0f, 1.0f,	//
		0.0f, 0.0f, -1.0f,


		0.5f, 0.5f, 5.0f, //v3
		1.0f, 0.0f,	
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

	// Import Obj Data
	sphrThingy = new ObjImport(L"Assets\\testCube.obj", gDevice, true, true);


	// Create Constant Buffer(s)
	D3D11_BUFFER_DESC cBufferDesc;
	memset(&cBufferDesc, 0, sizeof(cBufferDesc));

	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferDesc.ByteWidth = sizeof(MatrixBuffer);

	gDevice->CreateBuffer(&cBufferDesc, NULL, &gConstantBufferCamera);

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

void Main::Update()
{
	// Hold and update space matricies before rendering.
	sphrThingy->o_meshWorldMTX = XMMatrixIdentity();
	Rotation = XMMatrixRotationY(3.14f);
	Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	Translation = XMMatrixTranslation(-1.0f, 0.0f, 3.0f);
	sphrThingy->o_meshWorldMTX = Rotation *  Scale * Translation;


}



void Main::Render()
{
	float clearColor[] = { 0, 0, 0, 1 };
	UINT32 vertexSize = sizeof(float) * 8;
	UINT32 offset = 0;

	World = XMMatrixIdentity();


	gDeviceContext->IASetInputLayout(gVertexLayout);
	/*World = XMMatrixTranslation(0.0f, 0.0f, 0.0f);*/

	DetectInput();
	UpdateCamera();

	camProjection = XMMatrixPerspectiveFovLH((3.14f*(0.4f)), (640.0f / 480.0f), 0.5f, 1000.0f);
	WVP = XMMatrixMultiply(World, XMMatrixMultiply(camView, camProjection));
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	cbPerObj.hasTexture = false;

	gDeviceContext->UpdateSubresource(gConstantBufferCamera, 0, NULL, &cbPerObj, 0, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBufferCamera);

	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	

	gDeviceContext->IASetInputLayout(gVertexLayout);
	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//Set Shaders and texture
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
	//gDeviceContext->PSSetShaderResources(0, 1, &gTextureView);

	//Draw Object with 4 vertices
	gDeviceContext->Draw(4, 0);


	Update();
	//Render Quad
	/*UINT32 vertexSize = sizeof(float) * 9;
	UINT32 offset = 0;
	gDeviceContext->IASetInputLayout(gVertexLayout);
	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);*/

	
	for (int i = 0; i < sphrThingy->o_meshGroups; ++i)
	{
		gDeviceContext->IASetIndexBuffer(sphrThingy->o_meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);
		gDeviceContext->IASetVertexBuffers(0, 1, &sphrThingy->o_meshVertBuff, &vertexSize, &offset);
		/*ID3D11ShaderResourceView* srvTest;
		CreateWICTextureFromFile(gDevice, gDeviceContext, sphrThingy->o_textureNameArray[i], nullptr, &srvTest);*/
		/*WVP = sphrThingy->o_meshWorldMTX * camView * camProjection;*/
		cbPerObj.WVP = XMMatrixTranspose(WVP);
		cbPerObj.diffuseColor = sphrThingy->o_materials[sphrThingy->o_meshGroupTexture[i]].oM_difColor;
		cbPerObj.hasTexture = sphrThingy->o_materials[sphrThingy->o_meshGroupTexture[i]].oM_hasTexture;
		gDeviceContext->UpdateSubresource(gConstantBufferCamera, 0, NULL, &cbPerObj, 0, 0); 
		gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBufferCamera);
		gDeviceContext->PSSetConstantBuffers(0, 1, &gConstantBufferCamera);
		
		//Checks if the imported submesh have a texture
		if (sphrThingy->o_materials[sphrThingy->o_meshGroupTexture[i]].oM_hasTexture == true)
		{
			gDeviceContext->PSSetShaderResources(0, 1, &sphrThingy->o_meshSRV[sphrThingy->o_materials[sphrThingy->o_meshGroupTexture[i]].oM_texIndex]);
		}
		int indexStart = sphrThingy->o_meshGroupIndexStart[i];
		int indexDrawAmount = sphrThingy->o_meshGroupIndexStart[i + 1] - sphrThingy->o_meshGroupIndexStart[i];
		gDeviceContext->DrawIndexed(indexDrawAmount, indexStart, 0);
			
	}

	
}


int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Main mainObject;
	MSG msg = { 0 };
	HWND wndHandle = mainObject.InitWindow(hInstance);

	mainObject.mTimer.Reset();

	if (wndHandle)
	{
		mainObject.CreateDirect3DContext(wndHandle);

		mainObject.SetViewport();

		mainObject.CreateShaders();

		mainObject.CreateBuffers();

		mainObject.InitDirectInput(hInstance);




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
				mainObject.mTimer.Tick();
				mainObject.FpsCounter();
				mainObject.Render(); //Rendera
				mainObject.gSwapChain->Present(0, 0); //Växla front- och back-buffer
			}
		}




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
	mMainWndCaption = L"Direct3D Projekt";

		handle = CreateWindow(
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

	//Stäng programmet med escape!!
	case WM_CHAR: //en tanget har tryckts ner
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;


		}
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

Main::Main()
{
	gSwapChain =		nullptr;
	gDevice =			nullptr;
	gDeviceContext =	nullptr;
	gBackbufferRTV =	nullptr;
	gTextureView = nullptr;
	gVertexLayout = nullptr;
	gDepthStencilView = nullptr;
	gDepthStencilBuffer = nullptr;
	gVertexBuffer = nullptr;
	gIndexBuffer = nullptr;
	gVertexBuffer = nullptr;
	gPixelShader = nullptr;
	gGeometryShader = nullptr;
	
	gConstantBufferCamera = nullptr;
	
	DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	camYaw = 0.0f;
	camPitch = 0.0f;

	mainName = this;
}




Main::~Main()
{
	gVertexBuffer->Release();
	//gConstantBuffer->Release();
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

	gConstantBufferCamera->Release();
	DIKeyboard->Release();
	DIMouse->Release();
	delete sphrThingy;


}