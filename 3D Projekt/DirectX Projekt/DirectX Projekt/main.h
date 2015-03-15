#ifndef MAIN_H
#define MAIN_H

//#include <windows.h>
//#include <Windows.h>
//#include <d3d11.h>
//#include <d3dcompiler.h>
//#include <DirectXMath.h>
//#include <DirectXMathMatrix.inl>
//#include <string.h>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include "Root.h"
#include "ObjImport.h"
#include "GameTime.h"


//
//#pragma comment(lib,"d3d11.lib")
//#pragma comment(lib,"d3dcompiler.lib")
//

class Main
{

public:

	void CreateShaders();
	void CreateBuffers();
	void SetViewport();
	void Update();
	void Render();
	void FpsCounter();
	//int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
	HRESULT CreateDirect3DContext(HWND wndHandle);
	HWND InitWindow(HINSTANCE hInstance);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//LRESULT CALLBACK CallWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
	struct MatrixBuffer;
	
	GameTimer mTimer;
	std::wstring mMainWndCaption;
	HWND handle;

	XMMATRIX WVPMatrix;
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX worldView;
	XMMATRIX projMatrix;

	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;


	ObjImport* o_import = nullptr;

	Main();
	~Main();

	IDXGISwapChain* gSwapChain = nullptr;
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
	ID3D11RenderTargetView* gBackbufferRTV = nullptr;
	ID3D11ShaderResourceView* gTextureView = nullptr;

	ID3D11InputLayout* gVertexLayout = nullptr;
	ID3D11DepthStencilView* gDepthStencilView = nullptr;
	ID3D11Texture2D* gDepthStencilBuffer = nullptr;

	ID3D11Buffer* gVertexBuffer = nullptr;
	ID3D11Buffer* gIndexBuffer = nullptr;
	ID3D11VertexShader* gVertexShader = nullptr;
	ID3D11PixelShader* gPixelShader = nullptr;
	ID3D11GeometryShader* gGeometryShader = nullptr;

	//Objects
	ObjImport* sphrThingy;


protected:





};


#endif