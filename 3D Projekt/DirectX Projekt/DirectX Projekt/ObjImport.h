#ifndef OBJIMPORT_H
#define OBJIMPORT_H

#include "Root.h"


namespace  oMath {
	static XMFLOAT3 x_cross(const XMFLOAT3 &a, const XMFLOAT3 &b) {
		return XMFLOAT3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
			);
	}

	static XMFLOAT3 x_sum(const XMFLOAT3 &a, const XMFLOAT3 &b)
	{
		return XMFLOAT3(a.x + b.x,
			a.y + b.y, a.z + b.z);

	}
};
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
	//~o_SurfaceMaterial();
	/*o_SurfaceMaterial()
	{
		oM_materialName;
		oM_difColor;
		oM_trFilter;
		oM_specColor;
		oM_niOpticalDensity;
		oM_texIndex;
		oM_hasTexture;
		oM_isTransparent;
	}*/
};



class ObjImport{
//private:
public:
	ID3D11BlendState*						o_Transparency;					// mesh transparency Blend State
	ID3D11Buffer*							o_meshVertBuff;					// mesh vertex buffer
	ID3D11Buffer*							o_meshIndexBuff;				// mesh index buffer

	XMMATRIX								o_meshWorldMTX;					// mesh world orientation matrix
	int										o_meshGroups;					// mesh groups or subsets
	vector<int>								o_meshGroupIndexStart;			// mesh group Index start vector. Will contain the indices for when a group begins and ends.
	vector<int>								o_meshGroupTexture;				// mesh group texture index.
	vector<ID3D11ShaderResourceView*>		o_meshSRV;						// mesh Shader Resource View POINTER
	vector<wstring>							o_textureNameArray;				// mesh Texture Name Array to prevent the loading of a material more than once.
	
	vector<o_SurfaceMaterial>				o_materials;					//MaterialVector
	
public:

	ObjImport();
	ObjImport(wstring fileName, ID3D11Device* device, bool rh, bool cnormals);
	~ObjImport();
	bool o_OBJIMPORT(wstring fileName,
					ID3D11Buffer** vBuff,
					ID3D11Buffer** iBuff,
					vector<int>& groupIndexStart,
					vector<int>& groupMaterialIndex,
					vector<o_SurfaceMaterial>&material,
					ID3D11Device* device,
					int& groupCount,
					bool isRHCoordSys,
					bool createNormals
	);

	ID3D11Buffer* getMeshVB(void);
	ID3D11Buffer* getMeshIB(void);


};




#endif