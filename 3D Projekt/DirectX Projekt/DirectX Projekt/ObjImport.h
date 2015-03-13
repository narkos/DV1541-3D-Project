#ifndef OBJIMPORT_H
#define OBJIMPORT_H

#include "Root.h"

using namespace DirectX;
using namespace std;

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


XMFLOAT3 x_cross(const XMFLOAT3 &a, const XMFLOAT3 &b) {
	return XMFLOAT3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
		);
}

XMFLOAT3 x_sum(const XMFLOAT3 &a, const XMFLOAT3 &b)
{
	return XMFLOAT3(a.x + b.x,
		a.y + b.y, a.z + b.z);

}

struct o_SurfaceMaterial
{
	wstring								oM_materialName;
	XMFLOAT4							oM_difColor;
	XMFLOAT3							oM_trFilter;
	XMFLOAT3							oM_specColor;
	float								oM_niOpticalDensity;
	int									oM_texIndex;
	bool								oM_hasTexture;
	bool								oM_isTransparent;
};



class ObjImport{
private:
	ID3D11BlendState*						o_Transparency;					// mesh transparency Blend State
	ID3D11Buffer*							o_meshVertBuff;					// mesh vertex buffer
	ID3D11Buffer*							o_meshIndexBuff;				// mesh index buffer

	XMMATRIX								o_meshWorldMTX;					// mesh world orientation matrix
	int										o_meshGroups;					// mesh groups or subsets
	vector<int>								o_meshGroupIndexStart;			// mesh group Index start vector. Will contain the indices for when a group begins and ends.
	vector<int>								o_meshGroupTexture;				// mesh group texture index.
	vector<ID3D11ShaderResourceView*>		o_meshSRV;						// mesh Shader Resource View POINTER
	vector<wstring>							o_textureNameArray;				// mesh Texture Name Array to prevent the loading of a material more than once.
	
	
public:

	ObjImport();
	~ObjImport();
	bool o_OBJIMPORT(wstring fileName,
					ID3D11Buffer** vBuff,
					ID3D11Buffer** iBuff,
					vector<int>& groupIndexStart,
					vector<int>& groupMaterialIndex,
					vector<o_SurfaceMaterial>material,
					ID3D11Device* device,
					int& groupCount,
					bool isRHCoordSys,
					bool createNormals
	);

};




#endif