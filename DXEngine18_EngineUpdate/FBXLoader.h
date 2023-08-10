#pragma once

#include "DXUtil.h"
#include "Mesh.h"

namespace FBXLoader
{
	//extern FbxManager* fbxManager;

	HRESULT LoadFBX(const char* fileName, std::vector<Vertex>* pOutVertices, std::vector<DWORD>* pOutIndices);

	// 정점 UV 읽어오는 메소드.
	Vector2 ReadUV(FbxMesh* mesh, int controlPointIndex, int vertexCounter);

	// 정점 노멀 읽어오는 메소드.
	Vector3 ReadNormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter);

	// 정점 탄젠트 읽기.
	Vector3 ReadTangent(FbxMesh* mesh, int controlPointIndex, int vertexCounter);

	// 정점 바이노멀 읽기.
	Vector3 ReadBinormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter);
};