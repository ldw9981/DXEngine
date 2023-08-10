#pragma once

#include "DXUtil.h"
#include "Mesh.h"

namespace FBXLoader
{
	//extern FbxManager* fbxManager;

	HRESULT LoadFBX(const char* fileName, std::vector<Vertex>* pOutVertices, std::vector<DWORD>* pOutIndices);

	// ���� UV �о���� �޼ҵ�.
	Vector2 ReadUV(FbxMesh* mesh, int controlPointIndex, int vertexCounter);

	// ���� ��� �о���� �޼ҵ�.
	Vector3 ReadNormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter);

	// ���� ź��Ʈ �б�.
	Vector3 ReadTangent(FbxMesh* mesh, int controlPointIndex, int vertexCounter);

	// ���� ���̳�� �б�.
	Vector3 ReadBinormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter);
};