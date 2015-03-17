#ifndef ROOT_H
#define ROOT_H


#include <windows.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "DirectXTK\Inc\WICTextureLoader.h"
#include <dwrite.h>
#include <dinput.h>


using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x, y, z),
		UV(u, v),
		normal(nx, ny, nz){}

	XMFLOAT3 pos;
	XMFLOAT2 UV;
	XMFLOAT3 normal;
};

//
//HRESULT CreateWICTextureFromFileEx(
//	)
//


#endif