#pragma once

#include "DXUtil.h"
#include "Mesh.h"

namespace FBXLoader
{
	//FbxManager* fbxManager;

	HRESULT LoadFBX(const char* fileName, std::vector<Vertex>* pOutVertices, std::vector<DWORD>* pOutIndices);

	// 정점 UV 읽어오는 메소드.
	XMFLOAT2 ReadUV(FbxMesh* mesh, int controlPointIndex, int vertexCounter);

	// 정점 노멀 읽어오는 메소드.
	XMFLOAT3 ReadNormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter);

	// 정점 탄젠트 읽기.
	XMFLOAT3 ReadTangent(FbxMesh* mesh, int controlPointIndex, int vertexCounter);

	// 정점 바이노멀 읽기.
	XMFLOAT3 ReadBinormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter);
};