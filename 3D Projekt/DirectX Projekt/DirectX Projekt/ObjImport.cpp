
#include "ObjImport.h"
//using namespace Microsoft::WRL;
// --- Initialization of variables for 1 object. EACH .OBJ IMPORT NEEDS A SET OF THESE.
// !!!!*****!!!! Will ideally be placed in a class later.									!!!!*****!!!!

//ID3D11BlendState*						o_Transparency;					// mesh transparency Blend State
//ID3D11Buffer*							o_meshVertBuff;					// mesh vertex buffer
//ID3D11Buffer*							o_meshIndexBuff;				// mesh index buffer
//
//XMMATRIX									o_meshWorldMTX;					// mesh world orientation matrix
//int										o_meshGroups = 0;				// mesh groups or subsets
//vector<int>								o_meshGroupIndexStart;			// mesh group Index start vector. Will contain the indices for when a group begins and ends.
//vector<int>								o_meshGroupTexture;				// mesh group texture index.
//vector<ID3D11ShaderResourceView*>			o_meshSRV;						// mesh Shader Resource View POINTER
//vector<wstring>							o_textureNameArray;				// mesh Texture Name Array to prevent the loading of a material more than once.

// --- Material struct which contains the data regarding a material, followed by a vector of this struct which will contain our materials.


//XMFLOAT3 cross(const XMFLOAT3 &a, const XMFLOAT3 &b) {
//	return XMFLOAT3(
//		a.y * b.z - a.z * b.y,
//		a.z * b.x - a.x * b.z,
//		a.x * b.y - a.y * b.x
//		);
//}
//
//XMFLOAT3 x_sum(const XMFLOAT3 &a, const XMFLOAT3 &b)
//{
//	return XMFLOAT3(a.x + b.x,
//		a.y + b.y, a.z + b.z);
//
//}

//vector<o_SurfaceMaterial>				o_materials;

ObjImport::ObjImport()
{
	o_Transparency =	nullptr;					// mesh transparency Blend State
	o_meshVertBuff =	nullptr;					// mesh vertex buffer
	o_meshIndexBuff =	nullptr;				// mesh index buffer

	o_meshWorldMTX;					// mesh world orientation matrix
	o_meshGroups = 0;				// mesh groups or subsets
	o_meshGroupIndexStart;			// mesh group Index start vector. Will contain the indices for when a group begins and ends.
	o_meshGroupTexture;				// mesh group texture index.
	o_meshSRV;						// mesh Shader Resource View POINTER
	o_textureNameArray;				// mesh Texture Name Array to prevent the loading of a material more than once.
	o_materials;
}

ObjImport::ObjImport(wstring fileName, ID3D11Device* device, bool rh, bool cnormals)
{
	ObjImport();

	if (!o_OBJIMPORT(
		fileName,
		&o_meshVertBuff,
		&o_meshIndexBuff,
		o_meshGroupIndexStart,
		o_meshGroupTexture,
		o_materials,
		device,
		o_meshGroups,
		rh,
		cnormals
		))
	{
		//Error message
		wstring o_eMessage = L"Could not create object from: ";
		o_eMessage += fileName;

		MessageBox(0, o_eMessage.c_str(), L"Error", MB_OK);
		
	}

}

ObjImport::~ObjImport()
{
	//delete o_Transparency;
	//o_meshVertBuff->Release();
	//o_meshIndexBuff->Release();
}

ID3D11Buffer* ObjImport::getMeshVB()
{
	return o_meshVertBuff;
}

bool ObjImport::o_OBJIMPORT(wstring o_fileName,
	ID3D11Buffer** vBuff,
	ID3D11Buffer** iBuff,
	vector<int>& groupIndexStart,
	vector<int>& groupMaterialIndex,
	vector<o_SurfaceMaterial>& material,
	ID3D11Device* device,
	int& groupCount,
	bool isRHCoordSys,
	bool createNormals
	){
	HRESULT hr = 0;

	wifstream	o_fileIn(o_fileName.c_str());			// Opens the file
	wstring		o_matLibName;							// Saves the name of the mtllib provided in the obj.

	vector<DWORD>		o_indices;
	vector<XMFLOAT3>	o_vertPos;
	vector<XMFLOAT3>	o_vertNorm;
	vector<XMFLOAT2>	o_texCoord;
	vector<wstring>		o_strmaterials;
	vector<int>			o_vertPosIndex;					//Index Vectors for Position,
	vector<int>			o_vertNormIndex;				//Normals and
	vector<int>			o_texCoordIndex;				//Texture Coordinates.

	bool				o_hasTexCoord;					//Booleans to check whether object has
	bool				o_hasNormals;					//defined UV and Normals.

	//Temporary variables
	wstring				oTemp_materials;
	int					oTemp_vertPosIndex;
	int					oTemp_vertNormIndex;
	int					oTemp_texCoordIndex;

	//Counts
	int vIndex = 0;
	int faceVertCount = 0;
	int o_totalVertices = 0;
	int o_totalTriangles = 0;

	

	if (o_fileIn)
	{
		
		while (o_fileIn)
		{
			wstring lineData;
			getline(o_fileIn, lineData);

			if (lineData.c_str()[0] == '#') // Finds comments in .obj and continues to the next line.
			{
				cout << "This is a comment, fool";
				continue;
			}

			// SCAN THROUGH VERTEX RELATED INFORMATION
			else if (lineData.c_str()[0] == 'v')
			{
				if (lineData.c_str()[1] == ' ')										// FIND VERTEX POSITION AND ADD TO o_vertPos VECTOR.
				{
					float tempX, tempY, tempZ;
					swscanf_s(lineData.c_str(), L"v %f %f %f", &tempX, &tempY, &tempZ);
					//cout << "Printing a vertex: " << tempX << " " << tempY << " " << tempZ << endl;
					if (isRHCoordSys)												//If object is imported from a RH Coord System (such as Maya)
					{																//the Z-axis will be inverted by multiplying with -1.0f
						o_vertPos.push_back(XMFLOAT3(tempX, tempY, tempZ * -1.0f));
					}
					else
					{
						o_vertPos.push_back(XMFLOAT3(tempX, tempY, tempZ));
					}
				}
				else if (lineData.c_str()[1] == 't')								// FIND VERT TEXTURE COORDS AND ADD TO o_texCoord VECTOR.
				{
					float tempU, tempV;
					swscanf_s(lineData.c_str(), L"vt %f %f", &tempU, &tempV);

					if (isRHCoordSys)
					{
						o_texCoord.push_back(XMFLOAT2(tempU, 1.0f - tempV));		//Inverts v axis for Right hand coord system.
					}
					else
					{
						o_texCoord.push_back(XMFLOAT2(tempU, tempV));
					}
					o_hasTexCoord = true;
					//cout << "Printing UVs: " << tempU << " " << tempV << endl;

				}
				else if (lineData.c_str()[1] == 'n')								//FIND VERT NORMALS AND ADD TO o_vertNorm VECTOR.
				{
					float tempXn, tempYn, tempZn;
					swscanf_s(lineData.c_str(), L"vn %f %f %f", &tempXn, &tempYn, &tempZn);

					if (isRHCoordSys)
					{
						o_vertNorm.push_back(XMFLOAT3(tempXn, tempYn, tempZn*-1.0f));
					}
					else
					{
						o_vertNorm.push_back(XMFLOAT3(tempXn, tempYn, tempZn));
					}
					//cout << "Printing vertnormals: " << tempXn << " " << tempYn << " "<<tempZn << endl;
					o_hasNormals = true;
				}
			}
			else if (lineData.c_str()[0] == 'g' && lineData.c_str()[1] == ' ')		// FIND GROUP / SUBSET
			{
				groupIndexStart.push_back(vIndex);			//Sets the index for this group
				groupCount++;								//Updates the group count.
			}
			else if (lineData.c_str()[0] == 'f' && lineData.c_str()[1] == ' ')
			{
				// Checks Face-string for number of faces by looking up whitespaces. Provides an error if the model is not triangulated correctly
				// and ignores whitespace in the end of the line.
				int spaceCtr = 0;
				wstring::size_type foundSpace = 0;
				for (int i = 0; i < lineData.size(); i++)
				{
					wstring::size_type newFound = lineData.find(L" ", foundSpace + 1);
					if (newFound < lineData.size() - 1 && newFound != foundSpace)
					{
						spaceCtr += 1;
						foundSpace = newFound;
					}
				}

				if (spaceCtr > 3)
				{
					wstring o_eMessage = L"Found a face with too many vertices, fool. Make sure the mesh is properly triangulated and try again. Filename: ";
					o_eMessage += o_fileName;

					MessageBox(0, o_eMessage.c_str(), L"Error", MB_OK);
					return false;
				}
				else
				{
					int		t_vp_f[3], t_vt_f[3], t_vn_f[3];
					if (!o_hasTexCoord && !o_hasNormals)		//Interprets face data if obj does NOT have defined normals or texcoords defined.
					{
						swscanf_s(lineData.c_str(), L"f %d %d %d",
							&t_vp_f[2], &t_vp_f[1], &t_vp_f[0]);

						t_vt_f[2], t_vt_f[1], t_vt_f[0], t_vn_f[2], t_vn_f[1], t_vn_f[0] = 0; // Default Texcoords and Normals to 0
					}
					else if (o_hasTexCoord && !o_hasNormals)	//Face data has texcoords but NOT normals
					{
						swscanf_s(lineData.c_str(), L"f %d/%d %d/%d %d/%d",
							&t_vp_f[2], &t_vt_f[2], &t_vp_f[1], &t_vt_f[1], &t_vp_f[0], &t_vt_f[0]);

						t_vn_f[0], t_vn_f[1], t_vn_f[2] = 0;		// Default Normals to 0
					}
					else if (!o_hasTexCoord && o_hasNormals)	//Face data has normals but NOT 
					{
						swscanf_s(lineData.c_str(), L"f %d//%d %d//%d %d//%d",
							&t_vp_f[2], &t_vn_f[2], &t_vp_f[1], &t_vn_f[1], &t_vn_f[0], &t_vn_f[0]);

						t_vt_f[0], t_vt_f[1], t_vt_f[2] = 0;	// Default Texcoords to 0

					}
					else
					{											//Face data has BOTH normals and texcoords.
						swscanf_s(lineData.c_str(), L"f %d/%d/%d %d/%d/%d %d/%d/%d",
							&t_vp_f[2], &t_vt_f[2], &t_vn_f[2], &t_vp_f[1], &t_vt_f[1], &t_vn_f[1], &t_vp_f[0], &t_vt_f[0], &t_vn_f[0]);

						for (int i = 0; i < 3; i++)
						{
							t_vp_f[i] -= 1;
							t_vt_f[i] -= 1;
							t_vn_f[i] -= 1;
						}
						//cout << t_vp_f[0];
					}

					if (groupCount == 0)						// Makes sure that a start index is defined for groups even if
					{											// they are not defined in the .obj.
						groupIndexStart.push_back(vIndex);
						groupCount++;
					}
					//bool vertExists = false;
					if (o_totalVertices >= 3)
					{
						for (int vertCheck = 0; vertCheck < 3; ++vertCheck)
						{
							bool vertExists = false;
							for (int indexCheck = 0; indexCheck < o_totalVertices; ++indexCheck)
							{
								//if (/*t_vp_f[vertCheck] == o_vertPosIndex[indexCheck] &&*/ /*t_vt_f[vertCheck] == o_texCoordIndex[indexCheck]
								//	 && t_vn_f[vertCheck] == o_vertNormIndex[indexCheck] &&*/ !vertExists)
								//{
								//	o_indices.push_back(indexCheck);
								//	vertExists = true;
								//	cout << "Found duplicate vertex" << endl;
								//}
							}

							if (!vertExists)
							{
								o_vertPosIndex.push_back(t_vp_f[vertCheck]);
								o_texCoordIndex.push_back(t_vt_f[vertCheck]);
								o_vertNormIndex.push_back(t_vn_f[vertCheck]);
								o_totalVertices++;
								o_indices.push_back(o_totalVertices - 1);

								cout << "Added NEW vertex and new count is " << o_totalVertices << endl;
								vIndex++;
							}
						}
					}
					else
					{
						for (int v = 0; v < 3; ++v)
						{
							o_vertPosIndex.push_back(t_vp_f[v]);
							o_texCoordIndex.push_back(t_vt_f[v]);
							o_vertNormIndex.push_back(t_vn_f[v]);
							o_totalVertices++;
							o_indices.push_back(o_totalVertices - 1);
							vIndex++;
							cout << "Added NEW vertex and new count is " << o_totalVertices << endl;
						}
					}
					o_totalTriangles++;
				}
			}
			// MATERIAL LIBRARY OBJ INTERPRETATION
			else if (lineData.c_str()[0] == 'm' && lineData.c_str()[1] == 't')
			{
				//swscanf_s(lineData.c_str(), L"%ls shprThingy_01.mtl", &o_matLibName);
				wstring libNameTemp;
				libNameTemp = lineData.erase(0, 7);		// Might be unsafe, may have to fix later.
				o_matLibName = L"Assets/";				// This is not dynamic, should find a solution to get the directory as well
				o_matLibName += libNameTemp.c_str();
			}

			// MATERIAL VECTOR
			else if (lineData.c_str()[0] == 'u' && lineData.c_str()[1] == 's')
			{
				wstring matTemp = L"";
				matTemp = lineData.erase(0, 7);
				o_strmaterials.push_back(matTemp);
			}





		}
	}
	////////// END OF OBJ READING LOOP ///////////////////////////////////////////////
	else
	{
		//Error message
		wstring o_eMessage = L"Could not open the file: ";
		o_eMessage += o_fileName;

		MessageBox(0, o_eMessage.c_str(), L"Error", MB_OK);
		return false;
	}

	//Makes sure the last group has an end index
	groupIndexStart.push_back(vIndex);

	if (groupIndexStart[1] == 0)
	{
		groupIndexStart.erase(groupIndexStart.begin() + 1);
		groupCount--;
	}

	// Close the .obj
	o_fileIn.close();


	//Open .mtl
	wifstream	o_matFileIn(o_matLibName.c_str());

	//if Diffuse is not set, ambient color will be used instead.
	bool mat_kDset = false;
	//if no transparency is given, a default value of one should be assigned.
	bool mat_transSet = false;
	// Count of materials.
	int mat_count = material.size();

	if (o_matFileIn)
	{
		cout << "Successfully opened materialfile." << endl;
		while (o_matFileIn)
		{
			wstring mlineData;
			getline(o_matFileIn, mlineData);

			if (mlineData.c_str()[0] == '#')							//Ignores comments
			{
				cout << "This is a material file comment, fool!" << endl;
				continue;
			}
			// Create new material based on the newmtl-line
			else if (mlineData.c_str()[0] == 'n' &&  mlineData.c_str()[1] == 'e' && mlineData.c_str()[2] == 'w')
			{
				wstring tempMatName;
				tempMatName = mlineData.erase(0, 7);
				o_SurfaceMaterial tempMaterial;
				material.push_back(tempMaterial);
				// Set defaults
				material[mat_count].oM_materialName = tempMatName;
				material[mat_count].oM_isTransparent = false;
				material[mat_count].oM_texIndex = 0;
				material[mat_count].oM_hasTexture = false;
				wcout << "Created new material: " << material[mat_count].oM_materialName << endl;
				mat_count++;
				mat_kDset = false;

			}


			else if (mlineData.c_str()[0] == 'K')		//Reads diffuse color data
			{
				if (mlineData.c_str()[1] == 'd')
				{
					swscanf_s(mlineData.c_str(), L"Kd %f %f %f", &material[mat_count - 1].oM_difColor.x,
						&material[mat_count - 1].oM_difColor.y,
						&material[mat_count - 1].oM_difColor.z);
					mat_kDset = true;
					cout << "Loaded Kd" << endl;
				}
				else if (mlineData.c_str()[1] == 'a')		//Reads ambients color data
				{
					if (!mat_kDset)
					{
						swscanf_s(mlineData.c_str(), L"Ka %f %f %f", &material[mat_count - 1].oM_difColor.x,
							&material[mat_count - 1].oM_difColor.y,
							&material[mat_count - 1].oM_difColor.z);
						mat_kDset = true;
						cout << "Loaded Ka" << endl;
					}
				}
				else if (mlineData.c_str()[1] == 's')		// Reads specular color data
				{
					swscanf_s(mlineData.c_str(), L"Ks %f %f %f", &material[mat_count - 1].oM_specColor.x,
						&material[mat_count - 1].oM_specColor.y,
						&material[mat_count - 1].oM_specColor.z);
					cout << "Loaded Ks" << endl;
				}
			}
			else if (mlineData.c_str()[0] == 'T')
			{
				if (mlineData.c_str()[1] == 'f')			// Reads Transmission filter. Will likely not be used, but including anyway.
				{
					swscanf_s(mlineData.c_str(), L"Tf %f %f %f", &material[mat_count - 1].oM_trFilter.x,
						&material[mat_count - 1].oM_trFilter.y,
						&material[mat_count - 1].oM_trFilter.z);
					cout << "Loaded Tf" << endl;
				}
				else if (mlineData.c_str()[1] == 'r')
				{
					float transTemp;
					swscanf_s(mlineData.c_str(), L"r %f", &transTemp);

					//Possibly d should have this calculation instead. Examples show this the other way around, though I feel
					//that this makes more sense.
					transTemp = 1.0f - transTemp;

					material[mat_count - 1].oM_difColor.w = transTemp;
					cout << "Loaded Tr" << endl;
					if (transTemp < 1.0f)	//Same here, if this turns out to not work properly, this should be transTemp > 0.0f
					{
						material[mat_count - 1].oM_isTransparent = true;
					}
				}
			}
			else if (mlineData.c_str()[0] == 'd' && mlineData.c_str()[1] == ' ') //transparency is sometimes defined as 'd'
			{
				float transTemp;
				swscanf_s(mlineData.c_str(), L"d %f", &transTemp);

				material[mat_count - 1].oM_difColor.w = transTemp;

				if (transTemp < 1.0f)
				{
					material[mat_count - 1].oM_isTransparent = true;
				}

			}
			else if (mlineData.c_str()[0] == 'N' && mlineData.c_str()[1] == 'i')		// Reads Ni - Optical density data
			{
				swscanf_s(mlineData.c_str(), L"Ni %f", &material[mat_count - 1].oM_niOpticalDensity);
				cout << "Loaded Ni" << endl;
			}

			else if (mlineData.c_str()[0] == 'm' && mlineData.c_str()[1] == 'a' && mlineData.c_str()[2] == 'p')
			{
				if (mlineData.c_str()[4] == 'K' && mlineData.c_str()[5] == 'd')
				{
					wstring tex_fileName = L"Assets/";
					tex_fileName += mlineData.erase(0, 7);

					bool tex_isLoaded = false;
					for (int i = 0; i < o_textureNameArray.size(); i++)
					{
						if (tex_fileName == o_textureNameArray[i])
						{
							tex_isLoaded = true;
							material[mat_count - 1].oM_texIndex = i;
							material[mat_count - 1].oM_hasTexture = true;
						}
					}
					if (!tex_isLoaded)
					{
						CoInitialize(NULL);
						ID3D11ShaderResourceView* temp_SRV = nullptr;
						HRESULT br = CreateWICTextureFromFile(device, tex_fileName.c_str(), nullptr, &temp_SRV);
						if (SUCCEEDED(br))
						{
							o_textureNameArray.push_back(tex_fileName.c_str());
							material[mat_count - 1].oM_texIndex = o_meshSRV.size();
							o_meshSRV.push_back(temp_SRV);
							material[mat_count - 1].oM_hasTexture = true;
						}
					}

				}


				//If needed, other maps such as transparency/speculat etc maps can be added here.
			}




		}
	}
	o_matFileIn.close();				// Close the Materialfile.


	// Assign group material to the correct group
	for (int i = 0; i < o_meshGroups; ++i)
	{
		bool matAssigned = false;
		for (int j = 0; j < material.size(); ++j)
		{
			if (o_strmaterials[i] == material[j].oM_materialName)
			{
				groupMaterialIndex.push_back(j);
				matAssigned = true;
			}
		}
		if (!matAssigned)
		{

			groupMaterialIndex.push_back(0);		// This defaults to the first material.
		}
	}

	// Store vector information in vector struct/class.
	vector<Vertex>o_vertices;
	Vertex temp_vert;
	for (int o = 0; o < o_totalVertices; ++o)
	{
		temp_vert.pos = o_vertPos[o_vertPosIndex[o]];
		temp_vert.normal = o_vertNorm[o_vertNormIndex[o]];
		temp_vert.UV = o_texCoord[o_texCoordIndex[o]];
		o_vertices.push_back(temp_vert);
	}
	int loops = 0;
	cout << "Successfully created " << o_vertices.size() << " vertices" << endl;

	// If the normals have not been given, or we wish to recreate/average them, this function will create them for us
	// by first calculating the face normals and then the vertex normals. Requiers 'createNormals' to be true.
	if (createNormals == true)
	{
		vector<XMFLOAT3>normal_temp;			//Temporary vector for our computed face normals.
		XMFLOAT3 edge1, edge2;					//Holds edge vector data.
		XMFLOAT3 temp_unnormalized_FN = XMFLOAT3(0.0f, 0.0f, 0.0f);	//Storage for an unnormalized face normal. This is needed
		//due to memory issues with XMVECTOR vectors.

		XMFLOAT3 sumNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 tempSum = XMFLOAT3(0.0f, 0.0f, 0.0f);
		int count_face_w_vert;



		float edge_x, edge_y, edge_z;			//Temporary variables for storing edge vectors

		for (int i = 0; i < o_totalTriangles; i++)
		{
			edge_x = o_vertices[o_indices[i * 3]].pos.x - o_vertices[o_indices[(i * 3) + 1]].pos.x;
			edge_y = o_vertices[o_indices[i * 3]].pos.y - o_vertices[o_indices[(i * 3) + 1]].pos.y;
			edge_z = o_vertices[o_indices[i * 3]].pos.z - o_vertices[o_indices[(i * 3) + 1]].pos.z;

			edge1 = XMFLOAT3(edge_x, edge_y, edge_z);

			edge_x = o_vertices[o_indices[i * 3]].pos.x - o_vertices[o_indices[(i * 3) + 2]].pos.x;
			edge_y = o_vertices[o_indices[i * 3]].pos.y - o_vertices[o_indices[(i * 3) + 2]].pos.y;
			edge_z = o_vertices[o_indices[i * 3]].pos.z - o_vertices[o_indices[(i * 3) + 2]].pos.z;

			edge2 = XMFLOAT3(edge_x, edge_y, edge_z);

			temp_unnormalized_FN = XMFLOAT3(oMath::x_cross(edge1, edge2));
			normal_temp.push_back(temp_unnormalized_FN);

		}

		for (int v = 0; v < o_totalVertices; ++v)
		{
			for (int f = 0; f < o_totalTriangles; ++f)
			{
				if (o_indices[f * 3] == v || o_indices[(f * 3) + 1] == v || o_indices[(f * 3) + 2] == v)
				{
					tempSum = oMath::x_sum(sumNormal, normal_temp[f]);
					count_face_w_vert++;
				}
			}

			//Average and normalize
			o_vertices[v].normal.x = 1 / (sumNormal.x / count_face_w_vert);
			o_vertices[v].normal.y = 1 / (sumNormal.y / count_face_w_vert);
			o_vertices[v].normal.z = 1 / (sumNormal.z / count_face_w_vert);

			sumNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			count_face_w_vert = 0;
		}

	}

	cout << "Looped through " << loops << " faces." << endl;
	cout << "Total vertices defined: " << o_totalVertices << " and " << o_totalTriangles << " triangles." << endl;
	wcout << "added mtllib " << o_matLibName.c_str() << endl;
	wcout << o_strmaterials[0] << endl;

	//Index bufffer
	D3D11_BUFFER_DESC iBuffDesc;
	memset(&iBuffDesc, 0, sizeof(iBuffDesc));
	iBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	//iBuffDesc.ByteWidth = sizeof(DWORD) * o_totalTriangles * 3;
	iBuffDesc.ByteWidth = sizeof(DWORD) * o_totalTriangles *3;
	iBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBuffDesc.CPUAccessFlags = 0;
	iBuffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA i_initData;
	i_initData.pSysMem = &o_indices[0];
	device->CreateBuffer(&iBuffDesc, &i_initData, &o_meshIndexBuff);

	//Create Vertex Buffer
	D3D11_BUFFER_DESC vBuffDesc;
	memset(&vBuffDesc, 0, sizeof(vBuffDesc));

	vBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vBuffDesc.ByteWidth = sizeof(Vertex) * o_totalVertices;
	vBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBuffDesc.CPUAccessFlags = 0;
	vBuffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vBuffData;

	memset(&vBuffData, 0, sizeof(vBuffData));
	vBuffData.pSysMem = &o_vertices[0];
	hr = device->CreateBuffer(&vBuffDesc, &vBuffData, &o_meshVertBuff);

	return true;
}

//int main(){
//
//	o_OBJIMPORT(L"Assets\\shprThingy_01.obj", &o_meshVertBuff, &o_meshIndexBuff, o_meshGroupIndexStart, o_meshGroupTexture, o_materials, o_meshGroups, true, true);
//
//	int run = 1;
//
//	while (run != 0)
//	{
//		cin >> run;
//	}
//
//
//
//
//}