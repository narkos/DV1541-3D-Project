#include <windows.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>


using namespace DirectX;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")


class Main
{

public:

	void CreateShaders();
	void CreateBuffers();
	void SetViewport();
	void Render();
	int wWinMain(HINSTANCE , HINSTANCE , LPWSTR , int );
	HRESULT CreateDirect3DContext(HWND wndHandle);
	HWND InitWindow(HINSTANCE hInstance);
	//LRESULT CALLBACK WndProc(HWND , UINT , WPARAM , LPARAM);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);





protected:


	IDXGISwapChain* gSwapChain = nullptr;
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
	ID3D11RenderTargetView* gBackbufferRTV = nullptr;
	ID3D11ShaderResourceView* gTextureView = nullptr;

	ID3D11InputLayout* gVertexLayout = nullptr;
	ID3D11DepthStencilView* gDepthStencilView = nullptr;
	ID3D11Texture2D* gDepthStencilBuffer = nullptr;

	ID3D11Buffer* gVertexBuffer = nullptr;

	ID3D11VertexShader* gVertexShader = nullptr;
	ID3D11PixelShader* gPixelShader = nullptr;
	ID3D11GeometryShader* gGeometryShader = nullptr;


};
