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

struct MatrixBuffer
{
	XMMATRIX WVP;
	/*XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;*/
	XMFLOAT4 diffuseColor;
	bool hasTexture;


};


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
	/*struct MatrixBuffer;*/
	

	//Controller Functions
	bool InitDirectInput(HINSTANCE hInstance);
	void UpdateCamera(void);
	void DetectInput();


	GameTimer mTimer;
	std::wstring mMainWndCaption;
	HWND handle;

	/*XMMATRIX WVPMatrix;
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX worldView;
	XMMATRIX projMatrix;
	*/
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;


	MatrixBuffer cbPerObj;

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

	//New from Attila
	ID3D11Buffer* gConstantBufferCamera;

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;

	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	//Objects
	ObjImport* sphrThingy;


	//Camera Objects
	XMMATRIX WVP;
	XMMATRIX World;
	XMMATRIX camView;
	XMMATRIX camProjection;

	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;


	XMVECTOR DefaultForward ;
	XMVECTOR DefaultRight ;
	XMVECTOR camForward ;
	XMVECTOR camRight ;

	XMMATRIX camRotationMatrix;
	XMMATRIX groundWorld;

	float moveLeftRight /*= 0.0f*/;
	float moveBackForward /*= 0.0f*/;

	float camYaw /*= 0.0f*/;
	float camPitch /*= 0.0f*/;


protected:





};


#endif